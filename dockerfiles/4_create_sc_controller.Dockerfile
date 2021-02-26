# 2021 Collegiate eCTF
# Create SCEWL Bus Controller with side-channel emulator
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# NOTE: YOU MAY NOT MODIFY THIS FILE

ARG DEPLOYMENT
ARG SCEWL_ID
ARG NAME

# load in old container
FROM ${DEPLOYMENT}/controller:${NAME}_${SCEWL_ID} as controller

FROM ectf/sc-qemu

# copy controller binary into new container
COPY --from=controller /controller /controller

