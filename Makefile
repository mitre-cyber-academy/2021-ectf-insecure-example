# 2021 Collegiate eCTF
# Top level Makefile
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!
#
# NOTE: THIS FILE SHOULD NOT BE MODIFIED


# function to check required arguments
check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1)))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1))

# Step 0: Create the radio waves emulator
# does not need to be run by your team, as we have pushed
# an image of this container to Docker Hub
create_radio:
	docker build radio \
		-f dockerfiles/0_create_radio.Dockerfile \
		-t ectf/ectf-radio:latest

############################################################
# Step 1: Build the base images for the SSS and SEDs
create_deployment:
	$(call check_defined, DEPLOYMENT)
	docker build sss \
		-f dockerfiles/1a_create_sss.Dockerfile \
		-t ${DEPLOYMENT}/sss
	docker build controller \
		-f dockerfiles/1b_create_controller_base.Dockerfile \
		-t ${DEPLOYMENT}/controller:base

############################################################
# Step 2: Add a SED to the deployment
add_sed:
	$(call check_defined, DEPLOYMENT SED SCEWL_ID NAME)
	docker build cpu \
		-f dockerfiles/2a_build_cpu.Dockerfile \
		-t ${DEPLOYMENT}/cpu:${NAME}_${SCEWL_ID} \
		--build-arg SED=${SED} \
		--build-arg SCEWL_ID=${SCEWL_ID} \
		--build-arg CUSTOM=$(CUSTOM)
	docker build sss \
		-f dockerfiles/2b_create_sed_secrets.Dockerfile \
		-t ${DEPLOYMENT}/sss \
		--build-arg DEPLOYMENT=${DEPLOYMENT} \
		--build-arg SCEWL_ID=${SCEWL_ID}
	docker build controller \
		-f dockerfiles/2c_build_controller.Dockerfile \
		-t ${DEPLOYMENT}/controller:${NAME}_${SCEWL_ID} \
		--build-arg DEPLOYMENT=${DEPLOYMENT} \
		--build-arg SCEWL_ID=${SCEWL_ID}

############################################################
# Step 3: Remove an SED from the deployment
remove_sed:
	$(call check_defined, DEPLOYMENT SCEWL_ID NAME)
	docker rmi -f ${DEPLOYMENT}/cpu:${NAME}_${SCEWL_ID}
	docker rmi -f ${DEPLOYMENT}/controller:${NAME}_${SCEWL_ID}
	docker build sss \
		-f dockerfiles/3_remove_sed.Dockerfile \
		-t ${DEPLOYMENT}/sss \
		--build-arg DEPLOYMENT=${DEPLOYMENT} \
		--build-arg SCEWL_ID=${SCEWL_ID}

############################################################
# Step 4: Launch the radio and SSS
deploy: launch_radio_d launch_sss_d

# launch the radio waves emulator
launch_radio:
	$(call check_defined, DEPLOYMENT FAA_SOCK MITM_SOCK START_ID END_ID SC_PROBE_SOCK SC_RECVR_SOCK SOCK_ROOT)
	docker run $(DOCKOPT) -v $(SOCK_ROOT):/socks ectf/ectf-radio \
		python3 -u /radio.py $(START_ID) $(END_ID) $(FAA_SOCK) $(MITM_SOCK) $(SC_PROBE_SOCK) $(SC_RECVR_SOCK)

# launch the radio waves emulator detatched
launch_radio_d: DOCKOPT=-d
launch_radio_d: launch_radio

# launch the SSS
launch_sss:
	$(call check_defined, DEPLOYMENT SOCK_ROOT)
	docker run $(DOCKOPT) -v $(SOCK_ROOT):/socks $(DEPLOYMENT)/sss \
		/sss /socks/sss.sock

# launch the SSS detatched
launch_sss_d: DOCKOPT=-d
launch_sss_d: launch_sss

############################################################
# Step 5: Launch an SED
launch_sed:
	$(call check_defined, DEPLOYMENT SCEWL_ID NAME SOCK_ROOT)
	GDB=$(GDB) SC=$(SC) CONT_DOCK_OPT=$(CONT_DOCK_OPT) CPU_DOCK_OPT=$(CPU_DOCK_OPT) ./tools/launch_sed.sh $(BG)

launch_sed_d: BG=& 2>/dev/null >/dev/null
launch_sed_d: launch_sed

launch_sed_i: CPU_DOCK_OPT=-i
launch_sed_i: launch_sed

launch_sed_gdb: GDB='-gdb unix:/socks/gdb.sock,server'
launch_sed_gdb: launch_sed

launch_sed_sc: SC=sc-
launch_sed_sc: CONT_DOCK_OPT='--network host'
launch_sed_sc: launch_sed_d
	
############################################################
# launch FAA transceiver
launch_faa:
	$(call check_defined, FAA_SOCK)
	python3 tools/faa.py $(FAA_SOCK)
	
############################################################
# launch MitM transceiver
launch_mitm:
	$(call check_defined, MITM_SOCK)
	python3 tools/mitm.py $(MITM_SOCK)

############################################################
# creates a side channel container
create_sc_container:
	$(call check_defined, DEPLOYMENT SCEWL_ID NAME)
	docker build controller \
		-f dockerfiles/4_create_sc_controller.Dockerfile \
		-t ${DEPLOYMENT}/sc-controller:${NAME}_${SCEWL_ID} \
		--build-arg DEPLOYMENT=${DEPLOYMENT} \
		--build-arg NAME=${NAME} \
		--build-arg SCEWL_ID=${SCEWL_ID}

############################################################
# clean up the repo
clean: seds/* scewl_bus_driver scewl_bus_controller
	for file in $^; do make -C $$file clean; done
	-rm *.sock 2>&1
