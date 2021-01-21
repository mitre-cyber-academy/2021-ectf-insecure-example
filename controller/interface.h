/*
 * 2021 Collegiate eCTF
 * SCEWL Bus Controller interfaces header
 * Ben Janis
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 *
 * (c) 2021 The MITRE Corporation
 */

#ifndef INTERFACE_H
#define INTERFACE_H
#include "lm3s/lm3s_cmsis.h"

typedef UART_Type intf_t;
typedef unsigned int size_t;

#define CPU_INTF UART0
#define SSS_INTF UART1
#define RAD_INTF UART2

#define INTF_ERR       (-1)
#define INTF_NO_DATA   (-2)

/*
 * intf_init
 *
 * Initializes the interface
 *
 * Args:
 *   intf - pointer to the physical interface device
 */
void intf_init(intf_t *intf);


/*
 * intf_avail
 *
 * Checks if there is data available on an interface
 *
 * Args:
 *   intf - pointer to the physical interface device
 */
int intf_avail(intf_t *intf);


/*
 * intf_readb
 *
 * Reads a byte from interface
 *
 * Args:
 *   intf - pointer to an initialized interface device
 *   blocking - boolean of whether or not to block until a message is received
 *
 * Returns:
 *   an 8b value if a byte was available
 *   -1 if no bytes were available
 */
int intf_readb(intf_t *intf, int blocking);


/*
 * intf_read
 *
 * Reads bytes from the interface
 *
 * Args:
 *   intf - pointer to an initialized interface device
 *   buf - pointer to a buffer that will be filled with the message
 *   n - the maximum number of bytes from a message that will be put in the buffer
 *   blocking - boolean of whether or not to block until a message is received
 *
 * Returns:
 *   Length of data received if message was received
 *   INTF_NO_PKT if blocking is false and no message was available
 *   INTF_ERR if error
 */
int intf_read(intf_t *intf, char *buf, size_t n, int blocking);


/*
 * intf_write
 *
 * Writes a byte to a interface
 *
 * Args:
 *   intf - pointer to the initialized interface
 *   data - byte to write
 */
void intf_writeb(intf_t *intf, uint8_t data);


/*
 * intf_write
 *
 * Writes bytes to the interface
 *
 * Args:
 *   intf - pointer to the initialized interface
 *   buf - buffer intfth data to send
 *   len - length of data to send in bytes (up to 16,384)
 *
 * Returns:
 *   INTF_OK on success
 *   INTF_ERR on failure
 */
int intf_write(intf_t *intf, void *buf, int16_t len);

#endif // INTERFACE_H
