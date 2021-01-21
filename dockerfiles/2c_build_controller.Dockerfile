# 2021 Collegiate eCTF
# SCEWL Bus Controller build Dockerfile
# Ben Janis
#
# (c) 2021 The MITRE Corporation

ARG DEPLOYMENT

###################################################################
# if you want to copy files from the sss container,               #
# first create an intermediate stage:                             #
#                                                                 #
# FROM ${DEPLOYMENT}:sss as sss                                   #
#                                                                 #
# Then see box below                                              #
###################################################################

# load the base controller image
FROM ${DEPLOYMENT}/controller:base

# map in controller to /sed
# NOTE: only cpu/ and its subdirectories in the repo are accessible to this Dockerfile as .
ADD . /sed

###################################################################
# Copy files from the SSS container                               #
#                                                                 #
# COPY --from=sss /secrets/${SCEWL_ID}.secret /sed/sed.secret     #
#                                                                 #
###################################################################
# IT IS NOT RECOMMENDED TO KEEP DEPLOYMENT-WIDE SECRETS IN THE    #
# SED FILE STRUCTURE PAST BUILDING, SO CLEAN UP HERE AS NECESSARY #
###################################################################

# generate any other secrets and build controller
WORKDIR /sed
ARG SCEWL_ID
RUN make SCEWL_ID=${SCEWL_ID}
RUN mv /sed/gcc/controller.bin /controller

# NOTE: If you want to use the debugger with the scripts we provide, 
#       the ELF file must be at /controller.elf
RUN mv /sed/gcc/controller.axf /controller.elf
