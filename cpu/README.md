# CPU
This directory contains everything that will be needed to build the code that
will run on the CPU of an SED. There are two components to the user code:
1. The user code that controls the operation of the SED (`/cpu/seds/*`)
2. The SCEWL Bus Driver code that provides an interface to the SCEWL Bus
   Controller (`/cpu/scewl_bus_driver/`)

For your design, you do not need to modify anything in these directories.
In fact, you **MAY NOT** change anything in `/cpu/scewl_bus_driver/`.
You are allowed to modify or add SEDs for your own testing purposes, but
that is outside of what is functionally required for your submission.
