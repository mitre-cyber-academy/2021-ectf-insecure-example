# 2021 Collegiate eCTF
# SCEWL Bus Controller creation Dockerfile
# Ben Janis
#
# (c) 2021 The MITRE Corporation

# NOTE: you must use our Docker container (which imports from ubuntu:focal)
#       as your base if you want be able to write to flash, as flash is 
#       unimplemented in the standard QEMU distribution
FROM ectf/ectf-qemu:latest

# Add environment customizations here
# NOTE: do this first so Docker can used cached containers to skip reinstalling everything
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y binutils-arm-none-eabi gcc-arm-none-eabi make

# NOTE: only controller/ and its subdirectories in the repo are accessible to this Dockerfile as .
# NOTE: you can do whatever you need here to set up the base Docker container
#       that all controller images will inherit from
