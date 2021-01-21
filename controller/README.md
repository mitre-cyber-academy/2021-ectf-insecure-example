# SCEWL Bus Controller
The SCEWL Bus Controller implements the security and functionality of the SCEWL
protocol and is one of two components your team must implement (the other being
the SSS in `/sss/`). The SCEWL Bus Controller runs on a Stellaris lm3s6965 chip,
which will be emulated using qemu-system-arm.

The SCEWL Bus Controller is split into a few files that may be of interest to you,
although you are free to change any and all files in this directory as long as
functional and build requirements are met:

* `controller.{c,h}`: Implements the main functionality of the SCEWL Bus
  Controller. It contains `main()` and handles the message passing of the system
* `interface.{c,h}`: Implements the interface to the hardware interfaces, reading
  and writing raw bytes. **NOTE: it is your responsibility to format bytes written
  to the interfaces as specified in Section 4.6 of the rules.** Malformed messages
  may be mangled or dropped completely by the network backend emulation. There is
  a good chance that you will not need to change `interface.{c,h}` in your design.
* `startup_gcc.c`: Implements the system startup code, including initializing the
  stack and reset vectors. There is a good chance that you will not need to change
  `startup_gcc.c` in your design.
* `controller.ld`: The linker script to set up memory regions. There is a good
  chance that you will not need to change `controller.ld` in your design.
* `lm3s/`: Contains files to help interface with the lm3s6965 chip. There is a
  good chance that you will not need to change anything in `lm3s/` in your design.
* `CMSIS/`: Contains files to help interface with the ARM Cortex-M3 proccessor.
  There is a good chance that you will not need to change anything in `CMSIS/`
  in your design.

## On Adding Crypto
To aid with development, we have included Makefile rules and example code for using
[tiny-AES-c](https://github.com/kokke/tiny-AES-c) (see line 54 of the Makefile and
lines 11 and 219 of controller.c). You are free to use the library for your crypto
or simply use build process as a template for another crypto library of your choice.

If you choose to use a different crypto library, we recommend using the following
steps to integrate it into your system. **NOTE: All added libraries must compile
from the `all` rule of `controller/Makefile` to follow the functional requirements.

1. Find a crypto library suitable for your embedded system. **Make sure it does not
   require any system calls or dynamic memory allocation (i.e. malloc), as the
   controller runs on bare metal without an operating system**
2. Add the library to the `controller/` directory either as a submodule in git or
   as a copy of the library source code
3. Run the included tests of the library to verify it works properly on your machine
   before you integrate it with your code
4. Add the directory path to the list of include paths (`IPATH+=/path/to/crypto`)
   in `controller/Makefile`
5. Add the directory path to the list of source files (`VPATH+=/path/to/crypto`)
   in `controller/Makefile`
6. Add each object file you wish to link to the LDFLAGS list
   (`LDFLAGS+=${COMPILER}/source_file_name.o`) in `controller/Makefile`
7. Add each object file you wish to link to the `all` rule 
   (`all: ${COMPILER}/source_file_name.o`) in `controller/Makefile`
