/*
 * 2021 Collegiate eCTF
 * UAV SED
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
 *  INTERCEPT_B_FLAG
 *  INVALID_FLAG
 *  CUSTOM_FLAG
 *  CE_FLAG
 */

#define DLEN 0x1000
#define TY "U"

const char cur_src = UAV;



/**************************** send messages ****************************/

void broadcast(state_t *s) {
  brdcst_msg_t brdcst;

  fill_hdr(&brdcst.hdr, BRDCST_CMD);
  brdcst.uid = DEVICE_ID;
  brdcst.loc.x = s->x;
  brdcst.loc.y = s->y;

  scewl_brdcst(&s->scewl, sizeof(brdcst_msg_t), (char *)&brdcst);
}


// This will be at 0x10000, call it to get the code execution flag
void ce_target() __attribute__((section(".cetarget")));
void ce_target() {
  insec_t insec;
  insec_init(&insec, WI0, INSEC_ID);
  insec_write_str(&insec, STR(CE_FLAG) "1");
}



/**************************** handle messages ****************************/

void handle_brdcst(state_t *s, brdcst_msg_t *msg, scewl_id_t src_id,
                   scewl_id_t tgt_id, int len) {
  mission_msg_t mission;

  // check for malformed message
  ASSERT(len == sizeof(brdcst_msg_t) && msg->hdr.src == cur_src
         && msg->hdr.cmd == BRDCST_CMD && msg->uid == src_id,
         s, STR(INVALID_FLAG) "2");

  // fill and send mission
  fill_hdr(&mission.hdr, UAV_CMD);
  mission.kid = s->kid;
  mission.did = s->did;
  mission.loc.x = s->x_tgt;
  mission.loc.y = s->y_tgt;

  scewl_send(&s->scewl, src_id, sizeof(mission_msg_t), (char *)&mission);
}


void handle_cmd(state_t *s, char *msg, scewl_id_t src_id, scewl_id_t tgt_id,
                int len, int expected_cmd) {
  char cmd;
  scewl_id_t id;

  // extract and check message
  cmd = ((mission_msg_t *)msg)->hdr.cmd;

  ASSERT(cmd == expected_cmd, s, STR(INVALID_FLAG) "3");
  if (expected_cmd == LOGIN_CMD) {
    s->kid = ((mission_msg_t *)(msg))->kid;
    s->did = ((mission_msg_t *)(msg))->did;
    s->x_tgt = ((mission_msg_t *)(msg))->loc.x;
    s->y_tgt = ((mission_msg_t *)(msg))->loc.y;

    ASSERT(!s->registered && len == sizeof(mission_msg_t) && s->kid && s->did,
           s, STR(INVALID_FLAG) "4");
  } else {
    id = ((id_msg_t *)(msg))->id;

    ASSERT(s->registered && len == sizeof(id_msg_t) && id == DEVICE_ID,
           s, STR(INVALID_FLAG) "5");
  }

  // flip registration
  s->registered = !s->registered;
}


void handle_uav(state_t *s, mission_msg_t *msg, scewl_id_t src_id,
                scewl_id_t tgt_id, int len) {
  // check for malformed message
  ASSERT(msg->hdr.cmd == UAV_CMD && msg->kid == s->kid && msg->did == s->did
         && len == sizeof(mission_msg_t), s, STR(INVALID_FLAG) "6");

  // no response, since this is just the acknowledgement
}


void handle_check(state_t *s, id_msg_t *msg, scewl_id_t src_id,
                  scewl_id_t tgt_id, int len) {
  // check for malformed message
  ASSERT(msg->hdr.cmd == UAV_CMD && msg->id == DEVICE_ID
         && len == sizeof(id_msg_t), s, STR(INVALID_FLAG) "7");

  // no response, since this is just the acknowledgement
}


void handle_drop(state_t *s, id_msg_t *msg, scewl_id_t src_id,
                 scewl_id_t tgt_id, int len) {
  // check for malformed message or duplication
  ASSERT(msg->hdr.cmd == UAV_CMD && msg->id == DEVICE_ID
         && len == sizeof(id_msg_t), s, STR(INVALID_FLAG) "8");

  // no response, since this is just the acknowledgement
}


void handle_admin(state_t *s, admin_msg_t *msg, scewl_id_t src_id,
                  scewl_id_t tgt_id, int len) {
  char *login = "user:root,pwd:root";
  int corr = 1;

  for (int i = 0; login[i]; i++) {
    corr = corr && login[i] == msg->login[i];
  }

  ASSERT(corr && msg->hdr.cmd == ADMIN_CMD && len == sizeof(admin_msg_t),
         s, STR(INVALID_FLAG) "9");
  dispense_flag(s, STR(CUSTOM_FLAG));
}


void handle_message(state_t *s, char *msg, scewl_id_t src_id,
                    scewl_id_t tgt_id, int len, int expected_cmd) {
  // ignore errors or no packet
  if (len == SCEWL_ERR || len == SCEWL_NO_PKT) {
    return;
  }

  // should not receive any 0-length packets
  ASSERT(len, s, STR(INVALID_FLAG) "10");

  // check for valid broadcast, otherwise ignore
  if (tgt_id == SCEWL_BRDCST_ID) {
    handle_brdcst(s, (brdcst_msg_t *)msg, src_id, tgt_id, len);
    return;
  }

  // if not a broadcast, target should be self
  ASSERT(tgt_id == DEVICE_ID, s, STR(INVALID_FLAG) "11");

  switch (msg[0]) {
    case CMD:
      handle_cmd(s, msg, src_id, tgt_id, len, expected_cmd);
      break;
    case UAV:
      handle_uav(s, (mission_msg_t *)msg, src_id, tgt_id, len);
      break;
    case CHK:
      handle_check(s, (id_msg_t *)msg, src_id, tgt_id, len);
      break;
    case DRP:
      handle_drop(s, (id_msg_t *)msg, src_id, tgt_id, len);
      break;
    case ADM:
      handle_admin(s, (admin_msg_t *)msg, src_id, tgt_id, len);
      break;
    default: // invalid source
      dispense_flag(s, STR(INVALID_FLAG) "12");
  }
}



/**************************** UAV control ****************************/

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


void dereg(state_t *s) {
  if (scewl_deregister(&s->scewl) != SCEWL_OK) {
    insec_write_str(&s->insec, FMT_MSG("BAD DEREGISTRATION!\n"));
  }
}


int login(state_t *s) {
  scewl_id_t src_id, tgt_id;
  int len, i = 0, retries = 0;
  char data[DLEN];

  s->registered = 0;
  s->x = 0;
  s->y = 0;

  // send C2 login and get mission
  do {
    // resend login message every 16 received messages
    if ((i++ & 0xf) == 0) {
      if (retries++ == 3) {
        insec_write_str(&s->insec, FMT_MSG("UAV failed to login with C2"));
        return 0;
      }
      send_id(s, DEVICE_ID, C2_ID, LOGIN_CMD);
    }

    // get a response
    len = scewl_recv(&s->scewl, data, &src_id, &tgt_id, DLEN, 1);
    handle_message(s, data, src_id, tgt_id, len, LOGIN_CMD);
  } while (src_id != C2_ID); // ignore non-C2 messages

  // check for correct response
  if (!s->registered || !s->x_tgt || !s->y_tgt) {
    insec_write_str(&s->insec, FMT_MSG("UAV failed to login with C2"));
    return 0;
  }

  return 1;
}


void logout(state_t *s) {
  scewl_id_t src_id, tgt_id;
  int len, i = 0, retries = 0;
  char data[DLEN];

  // send C2 login and get mission
  do {
    // resend login message every 16 received messages
    if ((i++ & 0xf) == 0) {
      if (retries++ == 3) {
        insec_write_str(&s->insec, FMT_MSG("UAV failed to log out from C2"));
        return;
      }
      send_id(s, DEVICE_ID, C2_ID, LOGOUT_CMD);
    }

    // get a response
    len = scewl_recv(&s->scewl, data, &src_id, &tgt_id, DLEN, 1);
    handle_message(s, data, src_id, tgt_id, len, LOGOUT_CMD);
  } while (src_id != C2_ID); // ignore non-C2 messages
}


void deliver_package(state_t *s) {
  scewl_id_t src_id, tgt_id;
  int len, sent_checkpoint = 0;
  char data[DLEN];

  // go out on mission
  while (s->x < s->x_tgt || s->y < s->y_tgt) {
    // handle all incoming packets
    do {
      len = scewl_recv(&s->scewl, data, &src_id, &tgt_id, DLEN, 0);
      handle_message(s, data, src_id, tgt_id, len, 0);
    } while (len >= 0);

    // send checkpoint halfway through
    if (!sent_checkpoint && s->x + s->y > (s->x_tgt + s->y_tgt) / 2) {
      send_id(s, s->kid, s->kid, CHK_CMD);
      sent_checkpoint++;
    }

    // broadcast position every 0x4000 distance
    if (((s->x + s->y) % 0x4000) < 2) {
      broadcast(s);
    }

    // move
    if (s->x < s->x_tgt) s->x++;
    if (s->y < s->y_tgt) s->y++;

    SLEEP(10); // 10-20 minute round trip
  }

  send_id(s, s->did, s->did, DRP_CMD);
  insec_write_str(&s->insec, FMT_MSG("UAV delivered package"));
}


void return_home(state_t *s) {
  scewl_id_t src_id, tgt_id;
  int len, sent_checkpoint = 0;
  char data[DLEN];

  // return from mission
  sent_checkpoint = 0;
  while (s->x || s->y) {
    // handle all incoming packets
    do {
      len = scewl_recv(&s->scewl, data, &src_id, &tgt_id, DLEN, 0);
      handle_message(s, data, src_id, tgt_id, len, 0);
    } while (len >= 0);

    // send checkpoint halfway through
    if (!sent_checkpoint && s->x + s->y < (s->x_tgt + s->y_tgt) / 2) {
      send_id(s, s->kid, s->kid, CHK_CMD);
      sent_checkpoint++;
    }

    // broadcast position every 0x4000 distance
    if (((s->x + s->y) % 0x4000) < 2) {
      broadcast(s);
    }

    // move
    if (s->x) s->x--;
    if (s->y) s->y--;
    SLEEP(10); // 10-20 minute round trip
  }
}



/**************************** main ****************************/

int main(void) {
  state_t s;

  // initialize interfaces and scewl
  wi_init(WI0);
  wi_init(WI1);
  insec_init(&s.insec, WI0, INSEC_ID);
  scewl_init(&s.scewl, WI1, DEVICE_ID);
  insec_write_str(&s.insec, FMT_MSG("UAV booted"));

  // only launch if SCEWL registration works
  if (reg(&s)) {
    // execute mission
    if (login(&s)) {
      deliver_package(&s);
      return_home(&s);
      logout(&s);
    }

    // deregister from SSS
    dereg(&s);
  }

  // should never shut down, so never deregister
  insec_write_str(&s.insec, FMT_MSG("UAV shutting down"));
}

