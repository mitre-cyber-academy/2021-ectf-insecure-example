# 2021 Collegiate eCTF
# Example echo client SED
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

CC=arm-linux-gnueabi-gcc
SBD=/scewl_bus_driver

IPATH=-I.
IPATH+=-I/

check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1)))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1))

all:
	$(call check_defined, SCEWL_ID TGT_ID)
	$(CC) -o main main.c $(SBD)/sbd.o $(IPATH) -DSCEWL_ID=$(SCEWL_ID) -DTGT_ID=$(TGT_ID)

clean:
	-rm main 2>/dev/null
