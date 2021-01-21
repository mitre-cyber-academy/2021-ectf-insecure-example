# 2021 Collegiate eCTF
# Radio creation Dockerfile
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# NOTE: YOU MAY NOT MODIFY THIS FILE

FROM ubuntu:focal

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y python3

# add radio_waves.py
ADD radio_waves.py /radio.py
