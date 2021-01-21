/*
 * 2021 Collegiate eCTF
 * Dropzone SED
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 * For internal use only
 */

#include "common/common.h"

#ifndef C2_ID
#warning C2_ID not defined, using bad default of 0xffff
#define C2_ID ((scewl_id_t)0xffff)
#endif

#ifndef INTERCEPT_T_FLAG
#warning INTERCEPT_T_FLAG not defined, using bad default of 0xffff
#define INTERCEPT_T_FLAG ((scewl_id_t)0xffff)
#endif

#ifndef INVALID_FLAG
#warning INVALID_FLAG not defined, using bad default of 0xffff
#define INVALID_FLAG ((scewl_id_t)0xffff)
#endif



/**************************** defines ****************************
 *
 * Flags that need to be defined at compile:
 *  SCEWL_ID
 *  C2_ID
 *  INTERCEPT_T_FLAG
 *  INVALID_FLAG
 */

#define DLEN 0x1000
#define TY "D"

const char cur_src = DRP;



/**************************** handle message ****************************/

void handle_brdcst(state_t *s, char *msg, scewl_id_t src_id, scewl_id_t tgt_id,
                   int len) {
  char src, cmd;
  scewl_id_t id;

  src = ((brdcst_msg_t *)msg)->hdr.src;
  cmd = ((brdcst_msg_t *)msg)->hdr.cmd;
  id = ((brdcst_msg_t *)msg)->uid;
  ASSERT(len == sizeof(brdcst_msg_t) && src == UAV && cmd == BRDCST_CMD
         && id == src_id, STR(INVALID_FLAG) "1");
}


void handle_uav(state_t *s, char *msg, scewl_id_t src_id, scewl_id_t tgt_id,
                int len) {
  // extract KID
  scewl_id_t kid = ((id_msg_t *)(msg))->id;
  ASSERT(msg[1] == DRP_CMD && kid == SCEWL_ID && len == sizeof(id_msg_t),
         STR(INVALID_FLAG) "2");

  // send response
  send_id(src_id, src_id, UAV_CMD);
}


void handle_cmd(state_t *s, char *msg, scewl_id_t src_id, scewl_id_t tgt_id,
                int len) {
  scewl_id_t id;
  static int recvd = 0;

  // extract ID
  id = ((id_msg_t *)(msg))->id;

  // check for malformed message or duplication
  ASSERT(!recvd++ && msg[1] == DRP_CMD && id == SCEWL_ID
         && len == sizeof(id_msg_t), STR(INVALID_FLAG) "3");

  // no response, since this is just the acknowledgement
}


void handle_message(state_t *s, char *msg, scewl_id_t src_id,
                    scewl_id_t tgt_id, int len) {
  // ignore errors or no packet
  if (len == SCEWL_ERR || len == SCEWL_NO_MSG) {
    return;
  }

  // check for valid broadcast, otherwise ignore
  if (tgt_id == SCEWL_BRDCST_ID) {
    handle_brdcst(s, msg, src_id, tgt_id, len);
    return;
  }

  // if not a broadcast, target should be self
  ASSERT(tgt_id == SCEWL_ID, STR(INVALID_FLAG) "4");

  switch (msg[0]) {
    case UAV:
      handle_uav(s, msg, src_id, tgt_id, len);
      break;
    case CMD:
      handle_cmd(s, msg, src_id, tgt_id, len);
      break;
    default: // invalid source
      send_faa_str(STR(INVALID_FLAG) "5");
  }
}



/**************************** Checkpoint control ****************************/

int reg(state_t *s) {
  if (scewl_register() != SCEWL_OK) {
    send_faa_str(FMT_MSG("BAD REGISTRATION! Reregistering...\n"));
    if (scewl_deregister() != SCEWL_OK) {
      send_faa_str(FMT_MSG("BAD DEREGISTRATION!\n"));
      return 0;
    }
    if (scewl_register() != SCEWL_OK) {
      send_faa_str(FMT_MSG("BAD REGISTRATION! CANNOT RECOVER\n"));
      return 0;
    }
  }
  return 1;
}


/**************************** main ****************************/

int main(void) {
  scewl_id_t src_id, tgt_id;
  int len;
  char data[DLEN];
  state_t s;

  scewl_init();
  send_faa_str(FMT_MSG("Dropzone booted"));

  if (reg(&s)) {
    // log in with C2
    send_id(SCEWL_ID, C2_ID, LOGIN_CMD);

    // loop until quit received
    while (1) {
      // receive packet
      len = scewl_recv(data, &src_id, &tgt_id, DLEN, 1);

      handle_message(&s, data, src_id, tgt_id, len);
    }
  }

  // should never shut down, so never deregister
  send_faa_str(FMT_MSG("Dropzone shutting down"));
}

