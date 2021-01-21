/*
 * 2021 Collegiate eCTF
 * Common SED code source
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */

#include "common.h"

extern const char cur_src;


void fill_hdr(msg_hdr_t *hdr, int cmd) {
  hdr->src = cur_src;
  hdr->cmd = cmd;
  bzero(hdr->flag, FLEN);
  strncpy(hdr->flag, STR(INTERCEPT_T_FLAG), FLEN);
}

void send_id(scewl_id_t id, scewl_id_t tgt_id, char cmd) {
  id_msg_t id_msg;

  fill_hdr(&id_msg.hdr, cmd);
  id_msg.id = id;

  scewl_send(tgt_id, sizeof(id_msg_t), (char *)&id_msg);
}
