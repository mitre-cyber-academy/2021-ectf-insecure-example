#  2021 Collegiate eCTF
#  SCEWL Bus Driver Makefile
#  Ben Janis
#
#  (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!
#
# THIS FILE MUST NOT BE CHANGED IN YOUR SUBMISSION
# You may only change it for your own debugging or testing


CC=arm-linux-gnueabi-gcc

check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1)))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1))

all: clean
	$(call check_defined, SCEWL_ID)
	$(CC) scewl_bus_driver.c -c -o sbd.o -DSCEWL_ID=$(SCEWL_ID)

clean:
	-rm sbd.o 2>/dev/null
