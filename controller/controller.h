/*
 * 2021 Collegiate eCTF
 * SCEWL Bus Controller header
 * Ted Clifford
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "interface.h"
#include "lm3s/lm3s_cmsis.h"

#include <stdint.h>
#include <string.h>

#define SCEWL_MAX_DATA_SZ 0x4000

// type of a SCEWL ID
typedef uint16_t scewl_id_t;

// SCEWL_ID defined at compile
#ifndef SCEWL_ID
#warning SCEWL_ID not defined, using bad default of 0
#define SCEWL_ID 0
#endif


// SCEWL bus channel header
// NOTE: This is the required format to comply with Section 4.6 of the rules
typedef struct scewl_hdr_t {
  uint8_t magicS;  // all messages must start with the magic code "SC"
  uint8_t magicC;
  scewl_id_t tgt_id;
  scewl_id_t src_id;
  uint16_t len;
  /* data follows */
} scewl_hdr_t;

// registration message
typedef struct scewl_sss_msg_t {
  scewl_id_t dev_id;
  uint16_t   op;
} scewl_sss_msg_t;

// SCEWL status codes
enum scewl_status { SCEWL_ERR = -1, SCEWL_OK, SCEWL_ALREADY, SCEWL_NO_MSG };

// registration/deregistration options
enum scewl_sss_op_t { SCEWL_SSS_ALREADY = -1, SCEWL_SSS_REG, SCEWL_SSS_DEREG };

// reserved SCEWL IDs
enum scewl_ids { SCEWL_BRDCST_ID, SCEWL_SSS_ID, SCEWL_FAA_ID };

/*
 * read_msg
 *
 * Gets a message in the SCEWL pkt format from an interface
 *
 * Args:
 *   intf - pointer to the physical interface device
 *   buf - pointer to the message buffer
 *   src_id - pointer to a src_id
 *   tgt_id - pointer to a tgt_id
 *   n - maximum characters to be read into buf
 *   blocking - whether to wait for a message or not
 */
int read_msg(intf_t *intf, char *buf, scewl_id_t *src_id, scewl_id_t *tgt_id,
             size_t n, int blocking);

/*
 * send_msg
 * 
 * Sends a message in the SCEWL pkt format to an interface
 * 
 * Args:
 *   intf - pointer to the physical interface device
 *   src_id - the id of the sending device
 *   tgt_id - the id of the receiving device
 *   len - the length of message
 *   data - pointer to the message
 */
int send_msg(intf_t *intf, scewl_id_t src_id, scewl_id_t tgt_id, uint16_t len, char *data);

/*
 * handle_scewl_recv
 * 
 * Interprets a SCEWL tranmission from another SED and sends the message to the CPU
 */
int handle_scewl_recv(char* data, scewl_id_t src_id, uint16_t len);

/*
 * handle_scewl_send
 * 
 * Sends a message to another SED from the CPU
 */
int handle_scewl_send(char* buf, scewl_id_t tgt_id, uint16_t len);

/*
 * handle_brdcst_recv
 * 
 * Interprets a broadcast message from another SED and passes it to the CPU
 */
int handle_brdcst_recv(char* data, scewl_id_t src_id, uint16_t len);

/*
 * handle_brdcst_send
 * 
 * Broadcasts a message from the CPU to SEDS over the antenna
 */
int handle_brdcst_send(char *data, uint16_t len);

/*
 * handle_faa_recv
 * 
 * Receives an FAA message from the antenna and passes it to the CPU
 */
int handle_faa_recv(char* data, uint16_t len);

/*
 * handle_faa_send
 * 
 * Sends an FAA message from the CPU to the antenna
 */
int handle_faa_send(char* data, uint16_t len);

/*
 * handle_registration
 * 
 * Interprets a CPU registration message
 * 
 * args:
 *   op - pointer to the operation message received by the CPU
 */
void handle_registration(char* op);

/*
 * sss_register
 * 
 * Performs a registration with the SSS
 */
int sss_register();

/*
 * sss_deregister
 * 
 * Performs a deregistration with the SSS
 */
int sss_deregister();


#endif

