/*
 * 2021 Collegiate eCTF
 * SCEWL bus driver header
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 *
 * THIS FILE MUST NOT BE CHANGED IN YOUR SUBMISSION
 * You may only change it for your own debugging or testing
 */

#ifndef SCEWL_H
#define SCEWL_H

#include <stdint.h>
#include <stddef.h>


// type of a SCEWL ID
typedef uint16_t scewl_id_t;

// SCEWL_ID defined at compile
#ifndef SCEWL_ID
#warning SCEWL_ID not defined, using bad default of 0
#define SCEWL_ID 0
#endif


// SCEWL bus channel header
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
 * scewl_init
 * 
 * Initializes the SCEWL Bus. Must be called before any other SCEWL function
 */
void scewl_init();


/*
 * scewl_register
 *
 * Registers the device with the SSS
 *
 * Returns:
 *   SCEWL_OK on success, SCEWL_ERR on failure or if already registered
 */
int scewl_register();


/*
 * scewl_deregister
 *
 * Deregisters the device from the SSS
 *
 * Returns:
 *   SCEWL_OK on success, SCEWL_ERR on failure or if already deregistered
 */
int scewl_deregister();


/*
 * scewl_recv
 *
 * Securely receives a message from another SCEWL device
 *
 * Args:
 *   buf - pointer to a buffer that will be filled with the message
 *   src_id - pointer to a src_id_t which will be filled with the device
 *            ID of the message sender
 *   tgt_id - pointer to a src_id_t which will be filled with the device
 *            ID of the target (either this device's ID or SCEWL_BROADCAST_ID)
 *   n - the maximum number of bytes from a message that will be put in the buffer
 *   blocking - boolean of whether or not to block until a message is received
 *
 * Returns:
 *   Length of data received if message was received
 *   SCEWL_NO_MSG if blocking is false and no message was available
 *   SCEWL_ERR if error
 */
int scewl_recv(char *buf, scewl_id_t *src_id, scewl_id_t *tgt_id,
               size_t n, int blocking);


/*
 * scewl_send
 *
 * Securely sends data to a device on the SCEWL network
 *
 * Args:
 *   tgt_id - SCEWL ID of the target device
 *   len - length of data to send in bytes (up to 16,384)
 *   buf - buffer with data to send
 *
 * Returns:
 *   SCEWL_OK on success
 *   SCEWL_ERR on failure
 */
int scewl_send(scewl_id_t tgt_id, uint16_t len, char *buf);


/*
 * scewl_brdcst
 *
 * Securely sends data to all devices on the SCEWL network
 *
 * Args:
 *   len - length of data to send in bytes (up to 16,384)
 *   buf - buffer with data to send
 *
 * Returns:
 *   SCEWL_OK on success
 *   SCEWL_ERR on failure
 */
int scewl_brdcst(uint16_t len, char *buf);


#endif // SCEWL_H
