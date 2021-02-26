# 2021 MITRE Collegiate eCTF Getting Started

## Creating Your Own Fork
We suggest you create a fork of this repo so that you can begin to develop
your solution to the eCTF. To do this, you must fork the repo, change your fork to the `origin`, and
then add the MITRE repo as another remote. Follow these steps below.

1. Clone the eCTF repository using ssh or https 
```bash
git clone https://github.com/mitre-cyber-academy/2021-ectf-insecure-example --recursive
``` 

2. Change the current origin remote to another name
```bash
git remote rename origin mitre
```

3. Fork the MITRE repo on github (Note that you probably want to make the repo private for now so
   that other teams cannot borrow your development ideas) 

4. Add the fork as the new origin
```bash
git remote add origin <git_path>.git
```

You can now fetch and push as you normally would using `git fetch origin` and `git push origin`.
If we push out updated code, you can fetch this new code using `git fetch mitre`.

## Requirements
**You may skip this step if running on MITRE-provided development servers.**

The following packages are required:
 * `make`
 * `docker`
 * `python3`

### Setting up Docker
**You may skip this step if running on MITRE-provided development servers.**

To use Docker without sudo, you need to be in the `docker` group (from 
<https://docs.docker.com/engine/install/linux-postinstall/>).

1) Create the `docker` group (if not already done)
```
sudo groupadd docker
```

2) Add your user to the `docker` group
```
sudo usermod -aG docker $USER
```

3) Re-evaluate user group
```
newgrp docker
```

4) Verify you can run `docker` commands without `sudo`
```
docker run hello-world
```

## Creating and Launching a Deployment
To launch the example echo server and client (**and earn the Boot Reference flag**),
run the following:

**NOTE: the values of any arguments to make may be modified to suit your
deployment. You should just be aware that if multiple team members are working
at the same time on the same system, different SCEWL ID ranges and/or deployment
names should be used to avoid naming collisions**

### 1) Create deployment
```
make create_deployment DEPLOYMENT=echo
```

This will build a Docker container using `dockerfiles/1a_create_sss.Dockerfile`
and `dockerfiles/1b_create_controller_base.Dockerfile`, which creates a new
deployment with an SSS and a base image for SCEWL Bus Controllers to be built off
of. The SSS container will be tagged `echo/sss:latest` and the base SCEWL Bus
Controller container will be tagged `echo/controller:base`.

Arguments:
 * `DEPLOYMENT` - the name of the deployment that will prefix all Docker tags

Containers in deployment:
 * `echo/sss:latest` - Contiains the SSS and any deployment-wide secrets
 * `echo/controller:base` - Contains a base controller with any common packages
   or libraries pre-built

### 2) Add SEDs
```
make add_sed DEPLOYMENT=echo SED=echo_server SCEWL_ID=10 NAME=echo_server
make add_sed DEPLOYMENT=echo SED=echo_client SCEWL_ID=11 NAME=echo_client CUSTOM='TGT_ID=10'
make add_sed DEPLOYMENT=echo SED=echo_client SCEWL_ID=12 NAME=echo_client CUSTOM='TGT_ID=11'
```

These three invocations create first the echo server and then two echo clients.
Using the below arguments, three Dockerfiles are used to build the echo server and
then thes echo client. First, `2a_build_cpu.Dockerfile` is used to build the
user code that will run on each CPUs, which are tagged as the containers
`echo/cpu:echo_server_10`, `echo/cpu:echo_client_11`, and
`echo/cpu:echo_client_12`. Next, `2b_create_sed_secrets.Dockerfile` is used to
create device-specific secrets for each SED, modifying and retagging
`echo/sss:latest`. Finally, `2c_build_controller.Dockerfile` is used to build
the SCEWL Bus Controller, using the secrets stored in `echo/sss:latest`, and
tagging the containers as `echo/controller:echo_server_10`,
`echo/controller:echo_client_11`, and `echo/controller:echo_client_12`.

Arguments:
 * `DEPLOYMENT` - the same as before (here, `echo`)
 * `SED` - the directory in `cpu/seds/` that contains the SED (here, `echo_server`
   and `echo_client`);
 * `SCEWL_ID` - the SCEWL ID of the SED (here, 10, 11, and 12)
 * `NAME` - the name of the SED (here, `echo_server` and `echo_client`), which
   makes the tags of the Docker containers easier to decypher in the future
 * `CUSTOM` - (Optional) Passes any string to the SED's makefile, in this case
   defining the SCEWL ID of the echo servers.

Containers in deployment:
 * `echo/sss:latest` - Contiains the SSS, any deployment-wide secrets, and the
   SED-specific secrets for the echo server and clients
 * `echo/controller:base` - Contains a base controller with any common packages
   or libraries pre-built
 * `echo/cpu:echo_server_10` - Contains the CPU for the echo server
 * `echo/cpu:echo_client_11` - Contains the CPU for the first echo client
 * `echo/cpu:echo_client_12` - Contains the CPU for the second echo client
 * `echo/controller:echo_server_10` - Contains the SCEWL Bus Controller for the
   echo server
 * `echo/controller:echo_client_11` - Contains the SCEWL Bus Controller for the
   first echo client
 * `echo/controller:echo_client_12` - Contains the SCEWL Bus Controller for the
   second echo client

### 3) Remove SEDs
```
make remove_sed DEPLOYMENT=echo SCEWL_ID=12 NAME=echo_client
```

This would remove the second `echo_server` device from the deployment, removing
the CPU and SCEWL Bus Controller images and using `3_remove_sed.Dockerfile` to
remove any SED secrets from the SSS container and retagging it as `echo/sss:latest`.

Arguments:
 * The arguments `DEPLOYMENT`, `DEVICE_ID`, and `NAME` must be the same as what
   were given in `add_sed` for the desired SED.

Containers in deployment:
 * `echo/sss:latest` - Contiains the SSS, any deployment-wide secrets, and the
   SED-specific secrets for the echo server and client
 * `echo/controller:base` - Contains a base controller with any common packages
   or libraries pre-built
 * `echo/cpu:echo_server_10` - Contains the CPU for the echo server
 * `echo/cpu:echo_client_11` - Contains the CPU for the first echo client
 * `echo/controller:echo_server_10` - Contains the SCEWL Bus Controller for the
   second echo server
 * `echo/controller:echo_client_11` - Contains the SCEWL Bus Controller for the
   first echo client

### 4) Launch deployment
```
make deploy DEPLOYMENT=echo START_ID=10 END_ID=12 SOCK_ROOT=$PWD/socks FAA_SOCK=faa.sock \
    MITM_SOCK=mitm.sock SC_PROBE_SOCK=sc_probe.sock SC_RECVR_SOCK=sc_recvr.sock
```

This will launch `ectf/ectf-radio:latest` (from Docker Hub) and `echo/sss:latest`
in the background, deploying the radio waves emulator and SSS, respectively.
Once these are both live, they should stay up thoughout the lifetime of the system.

Arguments:
 * `DEPLOYMENT` - is the name of the deployment from before
 * `START_ID` and `END_ID` - the start and end ranges of possible SCEWL IDs of
   the deployment, respectively (start is inclusive, end is exclusive, so in
   this case only SCEWL IDs 10 and 11 are expected)
 * `SOCK_ROOT` - the **ABSOLUTE -- NOT RELATIVE** path to a directory to bind
   the backend Unix sockets to
 * `FAA_SOCK` - The name of the Unix socket for the FAA transceiver to attach to
 * `MITM_SOCK` - The name of the Unix socket for the MitM transceiver to attach to
 * `SC_PROBE_SOCK` - The name of the Unix socket for the side-channel probe to attach to
 * `SC_RECVR_SOCK` - The name of the Unix socket for the side-channel receiver to attach to

 Running containers:
  * `ectf/ectf-radio:latest` - the radio waves emulator
  * `echo/sss/:latest` - the SSS

### 5) Launch FAA transceiver
```
make launch_faa FAA_SOCK=socks/faa.sock
```

This launches the FAA transceiver, which will open a command prompt to control the
transceiver that you may use to interract with the FAA channel of the deployed SEDs.
Press `<enter>` to print any received messages. Type `help` or `?` for detailed
commands.


### 6) Launch SEDs
In another terminal:
```
make launch_sed_d DEPLOYMENT=echo SCEWL_ID=10 NAME=echo_server SOCK_ROOT=$PWD/socks
make launch_sed DEPLOYMENT=echo SCEWL_ID=11 NAME=echo_client SOCK_ROOT=$PWD/socks
```

This will launch two SEDs, first the echo server and then the echo client.
Arguments are as described above. NOTE: `launch_sed` is for launching
non-interactive SEDs of which you would like to see the output (though you
won't be able to input anything through STDIN), `launch_sed_d` is for launching
non-interactive SEDs that will be detached and backgrounded, and `launch_sed_i`
is for launching interactive SEDs, whose STDIN will be attached to the
terminal input.

After they spin up, if you press `<enter>` on the FAA transceiver, you should see
a new message with your Boot Reference flag.


### Faster method
Alternatively, you can define each of the static Makefile arguments as environmental variables,
reducing repetition and simplifying the invocations. The following is from `tools/deploy_echo.sh`:

```
```

To launch `tools/deploy_echo.sh`, make sure to do it from the root directory:

```
export DEPLOYMENT=echo
export SOCK_ROOT=$PWD/socks
export SSS_SOCK=sss.sock
export FAA_SOCK=faa.sock
export MITM_SOCK=mitm.sock
export START_ID=10
export END_ID=12

# create deployment
make create_deployment
make add_sed SED=echo_server SCEWL_ID=10 NAME=echo_server
make add_sed SED=echo_client SCEWL_ID=11 NAME=echo_client CUSTOM='TGT_ID=10'

# launch deployment
make deploy

# launch transceiver in background
python3 tools/faa.py $SOCK_ROOT/$FAA_SOCK &

# launch seds detatched
make launch_sed_d NAME=echo_server SCEWL_ID=10
sleep 1
make launch_sed_d NAME=echo_client SCEWL_ID=11

# bring transceiver back into foreground
fg
```

## Man-in-the-Middle (MitM) Interface
```
make launch_mitm MITM_SOCK=socks/mitm.sock
```
The MitM interface (`tools/mitm.py`) allows you to intercept, modify, inject,
or drop packets being sent over the wireless interface, although the reference
implementation only records and forwards the messages unmodified. The MitM
interface should be launched after the deployment is live, but may be started,
closed, and restarted at will (the radio waves emulator should automatically
detect if the connection opens or closes, and rout packets accordingly). The
`MITM_SOCK` argument should be the same as what was used in `make deploy`.


## Debugging an SED
To facilitate debugging, we have provided a tool to easily launch an SED and
attach GDB to it. To run it, first deploy the deployment (with relevant arguments):

```
make deploy DEPLOYMENT=<deployment> FAA_SOCK=<faa.sock> MITM_SOCK=<mitm.sock> \
    SOCK_ROOT=<socks> START_ID=<start_id> END_ID=<end_id>
```

And then launch the sed, automatically attatching GDB to the SCEWL Bus Controller:
```
make launch_sed_gdb DEPLOYMENT=<deployment> SCEWL_ID=<id> NAME=<name> SOCK_ROOT=<socks>
```

**NOTE: For this to work, your 2c_build_controller.Dockerfile must place the controller's
ELF (potentially compiled with the extension .axf) at /controller.elf**


## Docker tricks
To view all running Docker containers:
```
docker ps
```

To kill the Docker container with process ID 12345:
```
docker kill 12345
```

To kill all Docker containers (be aware not to kill the containers of others on the server):
```
docker kill $(docker ps -q)
```
You can streamline this by adding `alias dockerka='docker kill $(docker ps -q)'` to your `.bashrc`.

To run a command in the Docker container `test:deployment`:
```
docker run test:deployment echo "this echo command will be run in the container"
```

Docker can chew up disk space, so if you need to reclaim some, first prune all unused images:
```
docker image prune -a
```

If that isn't enough, you can prune all of the containers:
```
docker container prune
```
NOTE: these will remove all of the cached containers, so the next builds may take a longer time
