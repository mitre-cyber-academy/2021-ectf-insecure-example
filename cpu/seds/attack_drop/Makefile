
CC=arm-linux-gnueabi-gcc
SBD=/scewl_bus_driver
COMMON=/common

IPATH=-I.
IPATH+=-I/

check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1)))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1))

all:
	$(call check_defined, SCEWL_ID C2_ID INTERCEPT_T_FLAG INVALID_FLAG)
	make -C $(COMMON)
	$(CC) -o main main.c $(SBD)/sbd.o $(COMMON)/common.o $(IPATH) -DSCEWL_ID=$(SCEWL_ID) \
        -DC2_ID=$(C2_ID) -DINTERCEPT_T_FLAG=$(INTERCEPT_T_FLAG) -DINVALID_FLAG=$(INVALID_FLAG)

clean:
	-rm main 2>/dev/null
