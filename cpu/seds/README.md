# SEDs
This directory contains the source code for the user code of the SEDs. For the
design phase, two SEDs are provided for your testing: `echo_server` and `echo_client`.
As their names imply, `echo_server` is a server that echoes any messages received
from SCEWL, while `echo_client` sends a message to `echo_server` and prints a flag
if the response is correct.

During the attack phase, the attack phase SEDs (UAV, C2, and DZ from Section 6 of
the rules) will be added to this directory and built, although they are not
available to your teams during the design phase.

For your own testing, you may copy `echo_server` or `echo_client` into a new
directory in `cpu/seds/` and modify the protocol(s) however you wish. You may
also simply modify `echo_server` and/or `echo_client` for your own testing.
**NOTE: Any modifications made to SEDs or new SEDs that you add will not be
considered during testing, as they are not required components of your design.**
