#ifndef COMMON_H
#define COMMON_H

#include "lm3s/lm3s_cmsis.h"
#include "wi.h"
#include "scewl.h"
#include "insec.h"

#define FLEN 32
#define STR_(X) #X
#define STR(X) STR_(X)
#define TAG TY STR(INSEC_ID) ":" STR(DEVICE_ID) ":"
#define FMT_MSG(M) TAG M ";"

#define ASSERT(C, I, F) if (!(C)) { dispense_flag(I, F); return; }
#define SLEEP(MSEC) for (int _i = 0; _i < MSEC * 50000; _i++);


/**************************** message types ****************************/

enum src_ty_t {CMD = 'C', UAV = 'U', CHK = 'K', DRP = 'D', ADM = 'A'};
enum cmd_ty_t {LOGIN_CMD = 'I', LOGOUT_CMD = 'O', BRDCST_CMD = 'B',
               CHK_CMD = 'K', DRP_CMD = 'D', UAV_CMD = 'U', ADMIN_CMD = 'L'};

typedef struct state_t {
  insec_t insec;
  scewl_t scewl;
  scewl_id_t kid;
  scewl_id_t did;
  uint16_t x;
  uint16_t y;
  uint16_t x_tgt;
  uint16_t y_tgt;
  int registered;
} state_t;

typedef struct msg_hdr_t {
  char src;
  char cmd;
  char flag[FLEN];
} msg_hdr_t;

typedef struct location_t {
  uint16_t x;
  uint16_t y;
} location_t;

typedef struct mission_msg_t {
  msg_hdr_t hdr;
  scewl_id_t kid;
  scewl_id_t did;
  location_t loc;
} mission_msg_t;

typedef struct brdcst_msg_t {
  msg_hdr_t hdr;
  scewl_id_t uid;
  location_t loc;
} brdcst_msg_t;

typedef struct id_msg_t {
  msg_hdr_t hdr;
  scewl_id_t id;
} id_msg_t;

typedef struct admin_msg_t {
  msg_hdr_t hdr;
  char login[18];
} admin_msg_t;

/**************************** utility functions ****************************/

void strncpy_(char *dst, char *src, int n);

void bzero(char *buf, int len);

void fill_hdr(msg_hdr_t *hdr, int cmd);

void dispense_flag(state_t *s, char *flag);

void send_id(state_t *s, scewl_id_t id, scewl_id_t tgt_id, char cmd);

#endif // COMMON_H
