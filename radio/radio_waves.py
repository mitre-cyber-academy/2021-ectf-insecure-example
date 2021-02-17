# 2021 Collegiate eCTF
# Radio waves emulator
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

# DO NOT CHANGE THIS FILE

import argparse
import os
import logging
import struct
import socket
import select
from typing import List, NamedTuple, Optional, Dict


Header = NamedTuple('Header', [('ty', bytes), ('tgt', int), ('src', int), ('len', int)])
Message = NamedTuple('Message', [('hdr', Header), ('raw_hdr', bytes), ('body', bytes)])

BRDCST_ID = 0
SSS_ID = 1
FAA_ID = 2


class ScewlSock:
    HDR_LEN = 8
    SCEWL_MAGIC = b'SC'
    MITM_MAGIC = b'MM'

    def __init__(self, sock_path: str, q_len=1, log_level=logging.INFO, mode=None):
        self.sock_path = sock_path
        self.buf = b''

        # Make sure the socket does not already exist
        try:
            os.unlink(sock_path)
        except OSError:
            if os.path.exists(sock_path):
                raise

        # set up socket
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock.bind(sock_path)
        self.sock.listen(q_len)
        self.csock = None

        # change permissions if necessary
        if mode:
            print(f'Changing {repr(sock_path)} to {oct(mode)}')
            os.chmod(sock_path, mode)
            os.system(f'ls -l {sock_path}')

        # set up logger
        fhandler = logging.FileHandler(f'radio_waves.log')
        fhandler.setLevel(log_level)
        fhandler.setFormatter(
            logging.Formatter(f'%(asctime)s - {sock_path} - %(levelname)s - %(message)s'))

        shandler = logging.StreamHandler()
        shandler.setLevel(log_level)
        shandler.setFormatter(
            logging.Formatter(f'%(asctime)s - {sock_path} - %(levelname)s - %(message)s'))

        self.logger = logging.getLogger(f'{sock_path}_log')
        self.logger.addHandler(shandler)
        self.logger.setLevel(log_level)

    @staticmethod
    def sock_ready(sock: socket.SocketType) -> bool:
        ready, _, _ = select.select([sock], [], [], 0)
        return bool(ready)

    def active(self) -> bool:
        # try to accept new client
        if not self.csock:
            if self.sock_ready(self.sock):
                self.logger.info(f'Connection opened on {self.sock_path}')
                self.csock, _ = self.sock.accept()
        return bool(self.csock)
    
    def deserialize(self) -> Optional[Message]:
        # find start of message
        while len(self.buf) > 2 and self.buf[:2] not in (self.SCEWL_MAGIC, self.MITM_MAGIC):
            self.logger.warning(f'Bad magic {repr(self.buf[:2])}: scanning...')
            self.buf = self.buf[1:]

        # unpack header
        if len(self.buf) >= self.HDR_LEN:
            raw_hdr = self.buf[:self.HDR_LEN]
            hdr = Header(*struct.unpack('<2sHHH', raw_hdr))

            # parse message if completely available
            if len(self.buf[self.HDR_LEN:]) >= hdr.len:
                # chop off MitM header if needed
                if hdr.ty == self.MITM_MAGIC:
                    raw_hdr = b''
                msg = self.buf[self.HDR_LEN:self.HDR_LEN + hdr.len]

                # remove message from buffer
                self.buf = self.buf[self.HDR_LEN + hdr.len:]
                self.logger.debug(f'Read message {msg}')
                return Message(hdr, raw_hdr, msg)
        return None

    def read_msg(self) -> Optional[Message]:
        if not self.active():
            return None

        try:
            if self.sock_ready(self.csock):
                data = self.csock.recv(4096)

                # connection closed
                if not data:
                    self.close()
                    return None
                
                self.buf += data

            return self.deserialize()
        except (ConnectionResetError, BrokenPipeError):
            # cleanly handle forced closed connection
            self.close()
            return None

    def read_all_msgs(self) -> List[Message]:
        msgs = []
        msg = self.read_msg()
        while msg:
            msgs.append(msg)
            msg = self.read_msg()
        return msgs

    def send_msg(self, msg: Message) -> bool:
        if not self.active():
            return False

        try:
            hdr, raw_hdr, body = msg
            raw_msg = raw_hdr + body
            self.logger.info(f'Sending {len(raw_msg)}B {hdr.src}->({hdr.tgt}@{self.sock_path}): '
                             f'{repr(raw_msg)}')
            self.csock.sendall(raw_msg)
            return True
        except (ConnectionResetError, BrokenPipeError):
            # cleanly handle forced closed connection
            self.close()
            return False

    def close(self):
        self.logger.warning(f'Conection closed on {self.sock_path}')
        self.csock = None
        self.buf = b''


class MitM(ScewlSock):
    def forward(self, msgs) -> List[Message]:
        # add the messages to the Sock fifo and poll for new messages
        for msg in msgs:
            self.send_msg(msg)

        recvd_msgs = self.read_all_msgs()

        # return received messages if sock ready
        if self.csock:
            return recvd_msgs

        # just return messages if sock not available
        return msgs + recvd_msgs


def poll(socks: Dict[int, ScewlSock], mitm: MitM, faa: ScewlSock, sock: ScewlSock):
    msgs = sock.read_all_msgs()

    # try to forward messages through the MitM interface
    if mitm.active():
        msgs = mitm.forward(msgs)

    # send queued messages
    for msg in msgs:
        # send directly to/from FAA transceiver except for broadcasts
        if FAA_ID in (msg.hdr.src, msg.hdr.tgt) and msg.hdr.tgt != BRDCST_ID:
            socks[msg.hdr.tgt].send_msg(msg)
        # otherwise broadcast to all SEDs
        else:
            for sock in socks.values():
                if sock != faa:
                    sock.send_msg(msg)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('start_id', type=int, help='Beginning of expected SCEWL ID range')
    parser.add_argument('end_id', type=int, help='End of expected SCEWL ID range')
    parser.add_argument('faa_sock', help='Name of FAA transceiver socket (will be created)')
    parser.add_argument('mitm_sock', help='Name of MitM transceiver socket (will be created)')
    parser.add_argument('--sock-root', default='/socks', help='Path to the socket directory')

    return parser.parse_args()


def main():
    args = parse_args()
    start_id, end_id, faa_sock, mitm_sock, sock_root = \
        args.start_id, args.end_id, args.faa_sock, args.mitm_sock, args.sock_root

    # open all sockets
    socks = {sid: ScewlSock(os.path.join(sock_root, f'antenna_{sid}.sock'))
             for sid in range(start_id, end_id)}
    faa = ScewlSock(os.path.join(sock_root, faa_sock), mode=0o777)
    socks[FAA_ID] = faa
    mitm = MitM(os.path.join(sock_root, mitm_sock), mode=0o777)

    # poll forever
    while True:
        # poll each socket
        for sock in socks.values():
            if sock.active():
                poll(socks, mitm, faa, sock)


if __name__ == '__main__':
    main()
