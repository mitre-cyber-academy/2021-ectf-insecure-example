/*
 * 2021 Collegiate eCTF
 * SCEWL Bus Driver implementation
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 *
 * THIS FILE MUST NOT BE CHANGED IN YOUR SUBMISSION
 * You may only change it for your own debugging or testing
 */

#include "scewl_bus.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

int sock;
FILE *logfp;


void scewl_init() {
  // NOTE: if you want to write logs to a file in the Docker container
  // filesystem for debugging, change stderr to a call to fopen
  logfp = stderr;

  // create socket
  // NOTE: This is how the CPU communicates with the SCEWL Bus Controller in the
  // emulated setup -- a Unix socket mapped into the CPU's Docker container
  char *sock_path = "/socks/scewl_bus.sock";
  struct sockaddr_un addr;
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 1) {
    fprintf(logfp, "Bad socket! %d\n", sock);
    exit(-1);
  }

  // connect socket
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);
  if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un))) {
    fprintf(logfp, "Could not connect to %s!", sock_path);
    exit(-1);
  }
}


int scewl_register() {
  scewl_id_t dummy; // we don't care about src/tgt here
  scewl_sss_msg_t msg;

  msg.dev_id = SCEWL_ID;
  msg.op = SCEWL_SSS_REG;

  // send registration
  if (scewl_send(SCEWL_SSS_ID, sizeof(msg), (char *)&msg) == SCEWL_ERR) {
    fprintf(logfp, "failed to register\n");
    return SCEWL_ERR;
  }

  // receive response
  if (scewl_recv((char *)&msg, &dummy, &dummy, sizeof(msg), 1) == SCEWL_ERR) {
    fprintf(logfp, "failed to register\n");
    return SCEWL_ERR;
  }

  // op should be REG on success
  if (msg.op == SCEWL_SSS_REG) {
    return SCEWL_OK;
  }
  fprintf(logfp, "already registered\n");
  return SCEWL_ALREADY;
}


int scewl_deregister() {
  scewl_id_t dummy; // we don't care about src/tgt here
  scewl_sss_msg_t msg;

  msg.dev_id = SCEWL_ID;
  msg.op = SCEWL_SSS_DEREG;

  // send deregistration
  if (scewl_send(SCEWL_SSS_ID, sizeof(msg), (char *)&msg) == SCEWL_ERR) {
    fprintf(logfp, "failed to deregister\n");
    return SCEWL_ERR;
  }

  // receive response
  if (scewl_recv((char *)&msg, &dummy, &dummy, sizeof(msg), 1) == SCEWL_ERR) {
    fprintf(logfp, "failed to deregister\n");
    return SCEWL_ERR;
  }

  // op should be DEREG on success
  if (msg.op == SCEWL_SSS_DEREG) {
    return SCEWL_OK;
  }
  fprintf(logfp, "already deregistered\n");
  return SCEWL_ALREADY;
}


// read a full number of bytes, breaking if error
// or if non-blocking and no bytes available
int full_read(int sock, void *vbuf, int n) {
  char *buf = (char *)vbuf;
  int bread, bread_tot = 0;

  do {
    bread = read(sock, buf + bread_tot, n - bread_tot);
    bread_tot += bread;
  // break loop if read returned error or if header was all read
  } while (bread > 0 && bread_tot < n);
  return bread_tot;
}


int scewl_recv(char *buf, scewl_id_t *src_id, scewl_id_t *tgt_id,
               size_t n, int blocking) {
  scewl_hdr_t hdr;
  int res, max, bread, flags, dummy;

  // set blocking
  flags = fcntl(sock, F_GETFL, 0);
  if (blocking) {
    flags &= ~O_NONBLOCK;
  } else {
    flags |= O_NONBLOCK;
  }
  fcntl(sock, F_SETFL, flags);

  // clear buffer and header
  memset(&hdr, 0, sizeof(hdr));
  memset(buf, 0, n);

  // find header start
  do {
    hdr.magicC = 0;

    // check for S
    if (read(sock, &hdr.magicS, 1) < 1) {
      return SCEWL_NO_MSG;
    }

    // check for SC
    if (hdr.magicS == 'S') {
      do {
        if (read(sock, &hdr.magicC, 1) < 1) {
          return SCEWL_NO_MSG;
        }
      } while (hdr.magicC == 'S'); // in case multiple 'S's in a row
    }
  } while (hdr.magicS != 'S' && hdr.magicC != 'C');

  // read rest of header
  if (full_read(sock, ((char *)&hdr) + 2, sizeof(hdr) - 2) < sizeof(hdr) - 2) {
    res = SCEWL_NO_MSG;
  }

  // unpack header
  *src_id = hdr.src_id;
  *tgt_id = hdr.tgt_id;

  // read body
  max = hdr.len < n ? hdr.len : n;
  bread = full_read(sock, buf, max);

  // throw away rest of message if too long
  for (int i = 0; hdr.len > max && i < hdr.len - max; i++) {
    read(sock, &dummy, 1);
  }

  // report if not blocking and full message not received
  if(res == SCEWL_NO_MSG || bread < max) {
    fprintf(logfp, "NO MSG: res: %d == %d || bread: %d < max %d\n", res, SCEWL_NO_MSG, bread, max);
    return SCEWL_NO_MSG;
  }

  return max;
}


int scewl_send(scewl_id_t tgt_id, uint16_t len, char *data) {
  scewl_hdr_t hdr;
  size_t written;

  // pack header
  hdr.magicS  = 'S';
  hdr.magicC  = 'C';
  hdr.src_id = SCEWL_ID;
  hdr.tgt_id = tgt_id;
  hdr.len    = len;

  // send header
  written = write(sock, &hdr, sizeof(hdr));
  if (written < sizeof(hdr)) {
    return SCEWL_ERR;
  }

  // send body
  written = write(sock, data, len);
  if (written < len) {
    return SCEWL_ERR;
  }

  return SCEWL_OK;
}


int scewl_brdcst(uint16_t len, char *data) {
  scewl_send(SCEWL_BRDCST_ID, len, data);
  return SCEWL_OK;
}
