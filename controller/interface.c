/*
 * 2021 Collegiate eCTF
 * SCEWL Bus Controller interface implementation
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */

#include "interface.h"


// read/write available status masks
enum {
 RXFE = 0x10,
 TXFF = 0x20,
};


// initialize the interface
extern void intf_init(intf_t *intf) {
  // per TRM p.439 https://www.ti.com/lit/ds/symlink/lm3s6965.pdf
  intf->CTL &= 0xfffffffe;
  intf->IBRD = (intf->IBRD & 0xffff0000) | 0x000a;
  intf->FBRD = (intf->FBRD & 0xffff0000) | 0x0036;
  intf->LCRH = 0x00000060;
  intf->CTL |= 0x00000001;
}


// returns if the interface is available to read from
int intf_avail(intf_t *intf) {
  return !(intf->FR & RXFE);
}


// read a byte from the interface
int intf_readb(intf_t *intf, int blocking) {
  // block if requested
  while (blocking && !intf_avail(intf));

  // return no data if no data is available
  if (!intf_avail(intf)) {
      return INTF_NO_DATA;
  }

  return intf->DR;
}


// read from the interface
int intf_read(intf_t *intf, char *buf, size_t n, int blocking) {
  int read;
  int b;

  for (read = 0; read < n; read++) {
    b = intf_readb(intf, blocking);
    if (b < 0) {
      return INTF_NO_DATA;
    }
    ((uint8_t *)buf)[read] = (uint8_t)b;

    // give QEMU some time to queue the next byte if it's there
    for (int i = 0; i < 100000; i++) *buf = *buf;
  }
  return read;
}


// write a byte to the interface
void intf_writeb(intf_t *intf, uint8_t data) {
  // wait for room in transmit FIFO
  while(intf->FR & TXFF);
  intf->DR = data;
}


// write the the interface
int intf_write(intf_t *intf, void *buf, int16_t len) {
  for (int i = 0; i < len; i++) {
    intf_writeb(intf, ((uint8_t *)buf)[i]);
  }
  return len;
}

