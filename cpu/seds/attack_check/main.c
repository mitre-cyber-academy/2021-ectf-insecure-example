/*
 * 2021 Collegiate eCTF
 * Example echo server
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 * For internal use only
 */

#include "common.h"



/**************************** defines ****************************
 *
 * Flags that need to be defined at compile:
 *  DEVICE_ID
 *  INSEC_ID
 *  C2_ID
 *  INTERCEPT_T_FLAG
 *  INVALID_FLAG
 *  SPOOF_FLAG
 *  POSSIBLE_IDS
 *  NPOSSIBLE_IDS
 */

#define DLEN 0x1000
#define TY "K"

const char cur_src = CHK;

scewl_id_t possible_ids[] = POSSIBLE_IDS;


/**************************** handle message ****************************/

void handle_brdcst(state_t *s, brdcst_msg_t *msg, scewl_id_t src_id,
                   scewl_id_t tgt_id, int len) {
  ASSERT(len == sizeof(brdcst_msg_t) && msg->hdr.src == UAV
         && msg->hdr.cmd == BRDCST_CMD && msg->uid == src_id,
         s, STR(INVALID_FLAG) "1");
}


void handle_uav(state_t *s, char *msg, scewl_id_t src_id, scewl_id_t tgt_id,
                int len) {
  scewl_id_t kid;

  // extract KID
  kid = ((id_msg_t *)(msg))->id;
  ASSERT(msg[1] == CHK_CMD && kid == DEVICE_ID && len == sizeof(id_msg_t),
         s, STR(INVALID_FLAG) "2");

  // generate and send response
  send_id(s, src_id, src_id, UAV_CMD);
}


void handle_cmd(state_t *s, char *msg, scewl_id_t src_id, scewl_id_t tgt_id,
                int len) {
  scewl_id_t id;
  static int recvd = 0;

  // extract ID
  id = ((id_msg_t *)(msg))->id;

  // check for malformed message or duplication
  ASSERT(!recvd++ && msg[1] == CHK_CMD && id == DEVICE_ID
         && len == sizeof(id_msg_t), s, STR(INVALID_FLAG) "3");

  // no response, since this is just the acknowledgement
}


int check_id(state_t *s, scewl_id_t id) {
  int found = 0;

  // search through IDs for match
  for (int i = 0; i < NPOSSIBLE_IDS; i++) {
    if (id == possible_ids[i]) {
      found = 1;
    }
  }

  // dispense flag for new ID
  if (!found) {
    dispense_flag(s, STR(SPOOF_FLAG) "4");
    return 1;
  }
  return 0;
}


void handle_message(state_t *s, char *msg, scewl_id_t src_id,
                    scewl_id_t tgt_id, int len) {
  // ignore errors or no packet
  if (len == SCEWL_ERR || len == SCEWL_NO_PKT) {
    return;
  }

  // throw out bad IDs
  if (check_id(s, src_id)) {
    return;
  }

  // check for valid broadcast, otherwise ignore
  if (tgt_id == SCEWL_BRDCST_ID) {
    handle_brdcst(s, (brdcst_msg_t *)msg, src_id, tgt_id, len);
    return;
  }

  // if not a broadcast, target should be self
  ASSERT(tgt_id == DEVICE_ID, s, STR(INVALID_FLAG) "5");

  switch (msg[0]) {
    case UAV:
      handle_uav(s, msg, src_id, tgt_id, len);
      break;
    case CMD:
      handle_cmd(s, msg, src_id, tgt_id, len);
      break;
    default: // invalid source
      dispense_flag(s, STR(INVALID_FLAG) "6");
  }
}



/**************************** Checkpoint control ****************************/

int reg(state_t *s) {
  if (scewl_register(&s->scewl) != SCEWL_OK) {
    insec_write_str(&s->insec, FMT_MSG("BAD REGISTRATION! Reregistering...\n"));
    if (scewl_deregister(&s->scewl) != SCEWL_OK) {
      insec_write_str(&s->insec, FMT_MSG("BAD DEREGISTRATION!\n"));
      return 0;
    }
    if (scewl_register(&s->scewl) != SCEWL_OK) {
      insec_write_str(&s->insec, FMT_MSG("BAD REGISTRATION! CANNOT RECOVER\n"));
      return 0;
    }
  }
  return 1;
}


/**************************** main ****************************/

int main(void) {
  state_t s;
  scewl_id_t src_id, tgt_id;
  int len;
  char data[DLEN];

  // initialize interfaces and scewl
  wi_init(WI0);
  wi_init(WI1);
  insec_init(&s.insec, WI0, INSEC_ID);
  scewl_init(&s.scewl, WI1, DEVICE_ID);

  insec_write_str(&s.insec, FMT_MSG("Checkpoint booted"));

  // register
  if (reg(&s)) {
    // log in with C2
    send_id(&s, DEVICE_ID, C2_ID, LOGIN_CMD);

    // handle packets forever
    while (1) {
      len = scewl_recv(&s.scewl, data, &src_id, &tgt_id, DLEN, 1);
      handle_message(&s, data, src_id, tgt_id, len);
    }
  }

  // should never shut down, so never deregister
  insec_write_str(&s.insec, FMT_MSG("Checkpoint shutting down"));
}

