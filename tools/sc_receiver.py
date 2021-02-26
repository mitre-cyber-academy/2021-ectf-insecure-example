# 2021 Collegiate eCTF
# Example side-channel receiver
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

import socket
import argparse
import tqdm
import logging

logging.basicConfig(level=logging.INFO)

PACKET_SIZE = 1024

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('sock_path', help='Path to the SC receiver socket')
    parser.add_argument('ofile', help='File to output traces to')
    parser.add_argument('--max-file-size', default=-1, type=int, help='Data size to collect. Rounds down to mutiple of 1024B. Default collects until killed')
    return parser.parse_args()

def collect_trace(sock):
    return sock.recv(PACKET_SIZE)

def main():
    args = parse_args()

    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.connect(args.sock_path)

    fsize = 0

    try:
        f = open(args.ofile, 'wb')
        if args.max_file_size < 0:
            logging.info(f'Collecting traces to {args.ofile} (ctrl-c to stop)')
            while True:
                f.write(collect_trace(sock))
        else:
            ntraces = args.max_file_size // PACKET_SIZE
            logging.info(f'Collecting {ntraces} traces ({ntraces * PACKET_SIZE}B) to {args.ofile} (ctrl-c to stop)')
            for i in tqdm.tqdm(range(ntraces)):
                data = collect_trace(sock)
                fsize += len(data)
                f.write(data)
    except KeyboardInterrupt:
        logging.warning('Interrupt received. Aborting...')
        pass
    finally:
        f.close()

if __name__ == '__main__':
    main()
