# 2021 Collegiate eCTF
# Create SCEWL Bus Controller in 
# Ben Janis
#
# (c) 2021 The MITRE Corporation

ARG DEPLOYMENT
ARG SCEWL_ID
ARG NAME

# 
FROM ${DEPLOYMENT}/controller:${NAME}_${SCEWL_ID} as controller

FROM sc-qemu
# map in controller to /sed
# NOTE: to maximize the useage of container cache, map in only the files/directories you need
#       (e.g. only mapping in the controller directory rather than the entire repo)
ADD . /sed

COPY --from=controller /controller /controller

