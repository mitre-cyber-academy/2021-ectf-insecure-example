#include "common.h"

extern const char cur_src;


void strncpy_(char *dst, char *src, int n) {
  for (int i = 0; i < n - 1 && src[i]; i++) {
    dst[i] = src[i];
    dst[i + 1] = 0;
  }
}


void bzero(char *buf, int len) {
  while (len--) {
    buf[len] = 0;
  }
}


void fill_hdr(msg_hdr_t *hdr, int cmd) {
  hdr->src = cur_src;
  hdr->cmd = cmd;
  bzero(hdr->flag, FLEN);
  if (cmd == BRDCST_CMD) {
    strncpy_(hdr->flag, STR(INTERCEPT_B_FLAG), FLEN);
  } else {
    strncpy_(hdr->flag, STR(INTERCEPT_T_FLAG), FLEN);
  }
}


void dispense_flag(state_t *s, char *flag) {
  insec_write_str(&s->insec, flag);
}


void send_id(state_t *s, scewl_id_t id, scewl_id_t tgt_id, char cmd) {
  id_msg_t id_msg;

  fill_hdr(&id_msg.hdr, cmd);
  id_msg.id = id;

  scewl_send(&s->scewl, tgt_id, sizeof(id_msg_t), (char *)&id_msg);
}
