# 2021 Collegiate eCTF
# Common SED Makefile
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
	$(call check_defined, INTERCEPT_T_FLAG SCEWL_ID)
	$(CC) -c -o common.o common.c $(IPATH) -DINTERCEPT_T=$(INTERCEPT_T) -DSCEWL_ID=$(SCEWL_ID)

clean:
	-rm common.o 2>/dev/null
