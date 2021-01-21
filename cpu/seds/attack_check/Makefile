#******************************************************************************
#
# Makefile - Rules for building the CMSIS main example.
#
# Copyright (c) 2009 Luminary Micro, Inc.  All rights reserved.
# Software License Agreement
# 
# Luminary Micro, Inc. (LMI) is supplying this software for use solely and
# exclusively on LMI's microcontroller products.
# 
# The software is owned by LMI and/or its suppliers, and is protected under
# applicable copyright laws.  All rights are reserved.  You may not combine
# this software with "viral" open-source software in order to form a larger
# program.  Any use in violation of the foregoing restrictions may subject
# the user to criminal sanctions under applicable laws, as well as to civil
# liability for the breach of the terms and conditions of this license.
# 
# THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
# OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
# LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 32 of the Stellaris CMSIS Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S6965

#
# The base directory for DriverLib.
#
ROOT=../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=${ROOT}/lm3s
SCEWLPATH=${ROOT}/scewl

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=${ROOT}
IPATH+=${ROOT}/include
IPATH+=${ROOT}/CMSIS/Include
IPATH+=${SCEWLPATH}/include
IPATH+=${VPATH}

CFLAGS+=-DDEVICE_ID=${DEVICE_ID}
CFLAGS+=-DINSEC_ID=${INSEC_ID}
CFLAGS+=-DC2_ID=${C2_ID}
CFLAGS+=-DINTERCEPT_T_FLAG=${INTERCEPT_T_FLAG}
CFLAGS+=-DINVALID_FLAG=${INVALID_FLAG}
CFLAGS+=-DSPOOF_FLAG=${SPOOF_FLAG}
CFLAGS+=-DPOSSIBLE_IDS=${POSSIBLE_IDS}
CFLAGS+=-DNPOSSIBLE_IDS=${NPOSSIBLE_IDS}

check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1)))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1))

#
# The default rule, which causes the CMSIS main example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/main.axf

scewl:
	$(call check_defined, DEVICE_ID INSEC_ID INTERCEPT_T_FLAG INVALID_FLAG \
      CUSTOM_FLAG CE_FLAG C2_ID SPOOF_FLAG POSSIBLE_IDS NPOSSIBLE_IDS)
	make -C ${SCEWLPATH}

LDFLAGS+=${SCEWLPATH}/${COMPILER}/scewl.o
LDFLAGS+=${SCEWLPATH}/${COMPILER}/wi.o
LDFLAGS+=${SCEWLPATH}/${COMPILER}/insec.o

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir ${COMPILER}

${COMPILER}/main.axf: scewl
${COMPILER}/main.axf: ${COMPILER}/common.o
${COMPILER}/main.axf: ${COMPILER}/main.o
${COMPILER}/main.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/main.axf: ${COMPILER}/system_lm3s.o
ifeq (${COMPILER}, gcc)
${COMPILER}/main.axf: ${VPATH}/main.ld
endif
SCATTERgcc_main=${VPATH}/main.ld
ifeq (${COMPILER}, sourcerygxx)
${COMPILER}/main.axf: main_sourcerygxx.ld
endif
SCATTERsourcerygxx_main=lm3s6965-rom.ld -T main_sourcerygxx.ld
ENTRY_main=Reset_Handler

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
