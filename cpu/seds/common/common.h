/*
 * 2021 Collegiate eCTF
 * Common SED code header
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */

#ifndef COMMON_H
#define COMMON_H

#include "scewl_bus_driver/scewl_bus.h"

#include <string.h>

#define FLEN 32
#define STR_(X) #X
#define STR(X) STR_(X)
#define TAG TY STR(INSEC_ID) ":" STR(SCEWL_ID) ":"
#define FMT_MSG(M) TAG M ";"

#define ASSERT(C, F) if (!(C)) { send_faa_str(F); return; }
#define SLEEP(MSEC) for (int _i = 0; _i < MSEC * 50000; _i++);

#define send_faa_str(M) scewl_send(SCEWL_FAA_ID, strlen(M), M)

/**************************** message types ****************************/

enum src_ty_t {CMD = 'C', UAV = 'U', CHK = 'K', DRP = 'D', ADM = 'A'};
enum cmd_ty_t {LOGIN_CMD = 'I', LOGOUT_CMD = 'O', BRDCST_CMD = 'B',
               CHK_CMD = 'K', DRP_CMD = 'D', UAV_CMD = 'U', ADMIN_CMD = 'L'};

typedef struct state_t {
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

void fill_hdr(msg_hdr_t *hdr, int cmd);

void send_id(scewl_id_t id, scewl_id_t tgt_id, char cmd);

void send_faa_str(char *msg);

#endif // COMMON_H
