#!/bin/bash

# 2021 Collegiate eCTF
# Launch a test echo deployment
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

set -e
set -m

if [ ! -d ".git" ]; then
    echo "ERROR: This script must be run from the root of the repo!"
    exit 1
fi

export DEPLOYMENT=echo
export SOCK_ROOT=$PWD/socks
export SSS_SOCK=sss.sock
export FAA_SOCK=faa.sock
export MITM_SOCK=mitm.sock
export START_ID=10
export END_ID=12
export SC_PROBE_SOCK=sc_probe.sock
export SC_RECVR_SOCK=sc_recvr.sock

# create deployment
make create_deployment
make add_sed SED=echo_server SCEWL_ID=10 NAME=echo_server

# create side-channel collector
make create_sc_container SCEWL_ID=10 NAME=echo_server

# launch deployment
make deploy

# launch collector
python3 tools/sc_receiver.py socks/$SC_RECVR_SOCK example.traces --max-file-size 100000 &

# launch seds detatched
make launch_sed_sc NAME=echo_server SCEWL_ID=10

# bring side channel receiver back up
fg

# wait for collection
sleep 2

echo "Killing docker containers..."
docker kill $(docker ps -q) 2>/dev/null
