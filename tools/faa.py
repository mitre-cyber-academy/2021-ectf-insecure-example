# 2021 Collegiate eCTF
# FAA transceiver
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

import argparse
import socket
import struct
import logging
import select
import cmd
import os

INSEC_ID = 2

class FAATransceiver(cmd.Cmd):
    intro = 'Welcome to the FAA transceiver.\nPress enter to check for new messages.\nType help or ? to list commands.\n'
    prompt = 'FAA> '

    def __init__(self, sockf):
        super().__init__()

        # connect to radio waves emulator
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(sockf)
        self.sock = sock

    def emptyline(self):
        return False

    def send(self, tgt: int, payload: bytes):
        # pack into packet and send message
        msg = struct.pack(f'<2sHHH{len(payload)}s', b'SC', tgt, INSEC_ID, len(payload), payload)
        self.sock.send(msg)

        print(f'sent {INSEC_ID}->{tgt} ({len(msg)}B): {repr(msg)}')

    def do_send(self, arg: str):
        'Send a message to a device: send 10 message goes here'
        args = arg.split(' ')
        if len(args) < 2:
            print('Format: <scewl_id> <message>')
            return False

        try:
            tgt = int(args[0])
        except ValueError:
            print('SCEWL ID must be int')
            return False

        data = ' '.join(args[1:])
        self.send(tgt, data.encode())

    def do_send_hex(self, arg: str):
        'Send a binary message encoded in hex to a device: send 10 deadbeef'
        args = arg.split(' ')
        if len(args) < 2:
            print('Format: <scewl_id> <message>')
            return False

        try:
            tgt = int(args[0])
        except ValueError:
            print('SCEWL ID must be int')
            return False

        data = ' '.join(args[1:])

        # pack into packet and send message
        try:
            self.send(tgt, bytes.fromhex(data))
        except ValueError:
            print(f'{repr(data)} is not valid hex string')

    def do_docker(self, arg: str):
        'Run Docker command: e.g. docker ps'
        os.system('docker ' + arg)

    def do_exit(self, _):
        'Exits program'
        return True

    def do_quit(self, _):
        'Exits program'
        return True

    def postcmd(self, stop, line):
        # get all queued packets
        msgs = []
        while select.select([self.sock], [], [], 0)[0]:
            # receive and unpack packet header
            hdr = b''
            while len(hdr) < 8:
                hdr += self.sock.recv(8 - len(hdr))
            _, tgt, src, ln = struct.unpack('<HHHH', hdr)

            # receive packet body
            data = b''
            while len(data) < ln:
                data += self.sock.recv(ln - len(data))
            msgs.append(f'{src}->{tgt} ({len(data)}B): {repr(data)}')

        if msgs:
            print('=' * 10 + ' RECEIVED MESSAGES ' + '=' * 10)
            for msg in msgs:
                print(msg)

        return stop


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('sock', help='Path to MitM socket')

    return parser.parse_args()


def main():
    args = parse_args()

    try:
        FAATransceiver(args.sock).cmdloop()
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()