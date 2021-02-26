#!/usr/bin/env python3

# 2021 Collegiate eCTF
# Example looping side-channel receiver
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

import asyncio
import random
import argparse
import struct
import tqdm
import h5py
import numpy as np
import secrets


async def read_trace(queue: asyncio.Queue, trigger: asyncio.Event, rsc_sock: asyncio.StreamReader):
    while True:
        # throw away data while not triggered
        await rsc_sock.read(1024)

        # only record data if triggered
        if trigger.is_set():
            buf = b''
            while trigger.is_set():
                buf += await rsc_sock.read(1024)
            await queue.put(np.frombuffer(buf, dtype='uint8'))


async def collect_traces(queue: asyncio.Queue, trigger: asyncio.Event, ntraces: int,
                         rsock: asyncio.StreamReader, wsock: asyncio.StreamWriter, ofile: str,
                         tgt: int, src: int):
    with h5py.File(ofile, 'w') as f:
        for i in tqdm.tqdm(range(ntraces)):
            payload = secrets.token_bytes(16)
            msg = struct.pack(f'<2sHHH16s', b'SC', tgt, src, 16, payload)
            mitm_hdr = struct.pack('<2sHHH', b'MM', tgt, src, len(msg))

            # trigger and send message
            trigger.set()
            wsock.write(mitm_hdr + msg)

            # receive and unpack packet header and body
            hdr = await rsock.readexactly(8)
            _, _, _, ln = struct.unpack('<HHHH', hdr)
            response = await rsock.readexactly(ln)

            # untrigger
            trigger.clear()

            # grab trace from queue
            trace = await queue.get()

            # NOTE: if you want to preprocess/transform the trace, you can do it here

            # write trace to file
            dset = f.create_dataset(f'trace{i}', data=trace)
            dset.attrs['input'] = payload.hex()
            dset.attrs['output'] = response.hex()
            f.flush()

            # clean up to make sure we don't run out of memory
            del trace

            # sleep to let the device run a little further
            await asyncio.sleep(.1)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('sc_sock', help='Path to SC receiver socket')
    parser.add_argument('mitm_sock', help='Path to MitM socket')
    parser.add_argument('ntraces', type=int, help='Number of traces to collect')
    parser.add_argument('tgt', type=int, help='Target SCEWL ID')
    parser.add_argument('ofile', help='File name of output file')
    parser.add_argument('--src', default=2, type=int, help='Source SCEWL ID (default insec ID)')
    return parser.parse_args()


async def main():
    args = parse_args()
    trigger = asyncio.Event()
    queue = asyncio.Queue()
    rsc_sock, _ = await asyncio.open_unix_connection(args.sc_sock)
    rmitm_sock, wmitm_sock = await asyncio.open_unix_connection(args.mitm_sock)

    # queue reader task
    reader = asyncio.create_task(read_trace(queue, trigger, rsc_sock))

    # collect all traces
    await collect_traces(queue, trigger, args.ntraces, rmitm_sock, wmitm_sock, args.ofile,
                         args.tgt, args.src)

    # cancel and reap reader
    reader.cancel()
    await asyncio.gather(reader)


asyncio.run(main())
