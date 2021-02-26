#!/bin/bash

# 2021 Collegiate eCTF
# SED launcher script
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

# DO NOT MODIFY THIS FILE

set -e

# launch controller
CONTROLLER=`docker run -d \
    -v ${SOCK_ROOT}:/socks ${CONT_DOCK_OPT} \
    ${DEPLOYMENT}/${SC}controller:${NAME}_${SCEWL_ID} \
    qemu-system-arm -M lm3s6965evb -nographic -monitor none \
        ${GDB} \
        -kernel /controller \
        -serial unix:/socks/scewl_bus_${SCEWL_ID}.sock,server \
        -serial unix:/socks/sss.sock \
        -serial unix:/socks/antenna_${SCEWL_ID}.sock`
    
if [ -z "$GDB" ]; then
    # kill controller when CPU dies
    trap "docker kill $CONTROLLER 2>/dev/null" EXIT

    sleep 1

    # launch CPU
    docker run ${CPU_DOCK_OPT} \
        -v ${SOCK_ROOT}/sss.sock:/socks/sss.sock \
        -v ${SOCK_ROOT}/antenna_${SCEWL_ID}.sock:/socks/antenna.sock \
        -v ${SOCK_ROOT}/scewl_bus_${SCEWL_ID}.sock:/socks/scewl_bus.sock \
        ${DEPLOYMENT}/cpu:${NAME}_${SCEWL_ID} \
        qemu-arm -L /usr/arm-linux-gnueabi /cpu
else
    # launch CPU
    CPU=`docker run -d \
        -v ${SOCK_ROOT}/sss.sock:/socks/sss.sock \
        -v ${SOCK_ROOT}/antenna_${SCEWL_ID}.sock:/socks/antenna.sock \
        -v ${SOCK_ROOT}/scewl_bus_${SCEWL_ID}.sock:/socks/scewl_bus.sock \
        ${DEPLOYMENT}/cpu:${NAME}_${SCEWL_ID} \
        qemu-arm -L /usr/arm-linux-gnueabi /cpu`

    # kill devices when finished
    trap "docker kill $CONTROLLER $CPU 2>/dev/null" EXIT

    # fix socket permissions
    docker run -v ${SOCK_ROOT}/gdb.sock:/socks/gdb.sock \
        ${DEPLOYMENT}/cpu:${NAME}_${SCEWL_ID} \
        chmod 777 /socks/gdb.sock

    # hook up gdb
    docker cp $CONTROLLER:/controller.elf controller.elf.deleteme
    gdb-multiarch controller.elf.deleteme -ex 'target remote socks/gdb.sock'

    # clean up
    rm -f controller.elf.deleteme
fi

