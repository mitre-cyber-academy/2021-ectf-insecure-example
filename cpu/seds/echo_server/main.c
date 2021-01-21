/*
 * 2021 Collegiate eCTF
 * Example echo server
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */

#include "scewl_bus_driver/scewl_bus.h"

#include <stdio.h>


// SCEWL_ID needs to be defined at compile

#define DLEN 0x3fff

int main(void) {
  scewl_id_t src_id, tgt_id;
  int len;
  char data[DLEN];

  // open log file
  FILE *log = stderr;
  // NOTE: you can write to a file instead:
  // FILE *log = fopen("cpu.log", "a");

  scewl_init();

  // register
  if (scewl_register() != SCEWL_OK) {
    fprintf(log, "BAD REGISTRATION! Reregistering...\n");
    if (scewl_deregister() != SCEWL_OK) {
      fprintf(log, "BAD DEREGISTRATION!\n");
      return 1;
    }
    if (scewl_register() != SCEWL_OK) {
      fprintf(log, "BAD REGISTRATION! CANNOT RECOVER\n");
      return 1;
    }
  }

  // loop until quit received
  while (data[0] != 'q' || data[1] != 'u' ||
         data[2] != 'i' || data[3] != 't') {
    // print message
    fprintf(log, "***************************\nserver ID: %d\n", SCEWL_ID);
    fprintf(log, "Waiting for message...");

    // receive message
    len = scewl_recv(data, &src_id, &tgt_id, DLEN, 1);

    if (tgt_id == SCEWL_BRDCST_ID) {
      fprintf(log, " Received broadcast\n");
    } else {
      fprintf(log, " Received direct message\n");
    }

    // print message info
    fprintf(log, "src_id: %d\n", src_id);
    fprintf(log, "tgt_id: %d\n", tgt_id);
    fprintf(log, "len:    %d\n", len);
    fprintf(log, "data:   \"%s\"\n\n", data);

    // echo message
    scewl_send(src_id, len, data);
  }

  // degister
  fprintf(log, "Deregistering...\n");
  if (scewl_deregister() != SCEWL_OK) {
    fprintf(log, "BAD DEREGISTRATION!\n");
  }
  fprintf(log, "Exiting...\n");
}

