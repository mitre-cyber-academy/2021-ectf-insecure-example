# 2021 Collegiate eCTF
# SED CPU build Dockerfile
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# NOTE: YOU MAY NOT MODIFY THIS FILE

FROM ubuntu:focal

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y make binutils-arm-linux-gnueabi gcc-arm-linux-gnueabi qemu-user

ARG SED
ARG SCEWL_ID
ARG CUSTOM

# build SCEWL Bus Driver
ADD scewl_bus_driver /scewl_bus_driver
WORKDIR /scewl_bus_driver
RUN make SCEWL_ID=${SCEWL_ID}

# build CPU
ADD seds/common /common
ADD seds/${SED} /sed
WORKDIR /sed
RUN make SED=${SED} SCEWL_ID=${SCEWL_ID} ${CUSTOM}
RUN mv /sed/main /cpu
