# 2021 Collegiate eCTF
# Example man-in-the-middle interface
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

import socket
import argparse
import struct
import logging


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('sock', help='Path to MitM socket')

    return parser.parse_args()


def main():
    args = parse_args()

    # connect to radio waves emulator
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.connect(args.sock)

    # set up log
    log_level = logging.DEBUG
    handler = logging.StreamHandler()
    handler.setLevel(log_level)
    handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))

    logger = logging.getLogger('{port}_log')
    logger.addHandler(handler)
    logger.setLevel(log_level)

    # serve forever
    while True:
        # receive and unpack message
        hdr = b''
        while len(hdr) < 8:
            hdr += sock.recv(8 - len(hdr))
        _, tgt, src, ln = struct.unpack('<HHHH', hdr)

        # receive message body
        data = b''
        while len(data) < ln:
            data += sock.recv(ln - len(data))

        # NOTE: this example MitM tool just prints and echoes back the message, but you may
        # do whatever you like to modify/drop/inject messages
        logger.info(f'{src}->{tgt} ({len(data)}B): {repr(data)}')

        # echo message back with MitM header
        # the MitM header allows malformed messages to be forwarded correctly
        # as the radio waves emulator will forward based on the MitM header rather
        # than the payload header
        mitm_hdr = struct.pack('<2sHHH', b'MM', tgt, src, len(hdr + data))
        sock.send(mitm_hdr + hdr + data)


if __name__ == '__main__':
    main()
