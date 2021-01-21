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
 * Flags that need to be defined at compile:
 *  DEVICE_ID
 *  INSEC_ID
 *  INTERCEPT_T_FLAG
 *  INVALID_FLAG
 *  PRNG_SEED
 */

#define DLEN 0x1000
#define TY "C"

const char cur_src = CMD;



/**************************** utility functions ****************************/

uint32_t prng(state_t *s) {
  s->prng_state = s->prng_state * 22695477 + 1;
  return s->prng_state;
}



/**************************** handle messages ****************************/

void handle_brdcst(state_t *s, brdcst_msg_t *msg, scewl_id_t src_id,
                   scewl_id_t tgt_id, int len) {
  // check for malformed message
  ASSERT(len == sizeof(brdcst_msg_t) && msg->hdr.src == UAV
         && msg->hdr.cmd == BRDCST_CMD && msg->uid == src_id,
         s, STR(INVALID_FLAG) "1");
}


void handle_uav(state_t *s, id_msg_t *msg, scewl_id_t src_id,
                scewl_id_t tgt_id, int len) {
  mission_msg_t mission;

  switch (msg->hdr.cmd) {
    // handle login
    case LOGIN_CMD:
      // check for malformed message
      ASSERT(msg->id == src_id && len == sizeof(id_msg_t),
             s, STR(INVALID_FLAG) "2");

      // generate and send mission
      fill_hdr(&mission.hdr, LOGIN_CMD);
      mission.kid = s->kid;
      mission.did = s->did;

      // generate random distance over 32k
      mission.loc.x = prng(s) | 0x8000;
      mission.loc.y = prng(s) | 0x8000;

      scewl_send(&s->scewl, src_id, sizeof(mission_msg_t), (char *)&mission);
      break;
    // handle logout
    case LOGOUT_CMD:
      // check for malformed message
      ASSERT(msg->id == src_id && len == sizeof(id_msg_t),
             s, STR(INVALID_FLAG) "3");

      // send response
      send_id(s, src_id, src_id, LOGOUT_CMD);
      break;
    default:
      dispense_flag(s, STR(INVALID_FLAG) "4");
  }
}


void handle_check(state_t *s, id_msg_t *msg, scewl_id_t src_id,
                  scewl_id_t tgt_id, int len) {
  // check for malformed message
  ASSERT(msg->hdr.cmd == LOGIN_CMD && msg->id == src_id
         && len == sizeof(id_msg_t), s, STR(INVALID_FLAG) "5");

  // set checkpoint ID
  s->kid = msg->id;

  // generate and send response
  send_id(s, src_id, src_id, CHK_CMD);
}


void handle_drop(state_t *s, id_msg_t *msg, scewl_id_t src_id,
                 scewl_id_t tgt_id, int len) {
  // check for malformed message
  ASSERT(msg->hdr.cmd == LOGIN_CMD && msg->id == src_id
         && len == sizeof(id_msg_t), s, STR(INVALID_FLAG) "6");

  // set dropzone ID
  s->did = msg->id;

  // generate and send response
  send_id(s, src_id, src_id, DRP_CMD);
}


void handle_message(state_t *s, char *msg, scewl_id_t src_id,
                    scewl_id_t tgt_id, int len) {
  // ignore errors or no packet
  if (len == SCEWL_ERR || len == SCEWL_NO_PKT) {
    return;
  }

  // check for valid broadcast, otherwise ignore
  if (tgt_id == SCEWL_BRDCST_ID) {
    handle_brdcst(s, (brdcst_msg_t *)msg, src_id, tgt_id, len);
    return;
  }

  // if not a broadcast, target should be self
  ASSERT(tgt_id == DEVICE_ID, s, STR(INVALID_FLAG) "7");

  switch (msg[0]) {
    case UAV:
      handle_uav(s, (id_msg_t *)msg, src_id, tgt_id, len);
      break;
    case CHK:
      handle_check(s, (id_msg_t *)msg, src_id, tgt_id, len);
      break;
    case DRP:
      handle_drop(s, (id_msg_t *)msg, src_id, tgt_id, len);
      break;
    default: // invalid source
      dispense_flag(s, STR(INVALID_FLAG) "8");
  }
}



/**************************** C2 control ****************************/

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

  // seed prng
  s.prng_state = PRNG_SEED;

  insec_write_str(&s.insec, FMT_MSG("C2 booted"));

  // register
  if (reg(&s)) {
    // handle packets forever
    while (1) {
      len = scewl_recv(&s.scewl, data, &src_id, &tgt_id, DLEN, 1);
      handle_message(&s, data, src_id, tgt_id, len);
    }
  }

  // should never shut down, so never deregister
  insec_write_str(&s.insec, FMT_MSG("C2 shutting down"));
}

