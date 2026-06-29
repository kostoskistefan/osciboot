PROJECT = $(CURDIR)

BINARY 		       = bin/osciboot
CONFIGURATION_FILE = osciboot.conf

SCP    = scp
MAKE   = make
DOCKER = docker

DEVELOPMENT_IMAGE      = osciboot-development
DEVELOPMENT_DOCKERFILE = Dockerfile.development
DEVELOPMENT_MAKEFILE   = Makefile.build

UID = $(shell id -u)
GID = $(shell id -g)

SCP_FLAGS  	  = -o KexAlgorithms=+diffie-hellman-group1-sha1 -o HostKeyAlgorithms=+ssh-rsa -o MACs=hmac-sha1
SCP_DIRECTORY = root@172.16.5.94:/home/dso

.PHONY: all setup build clean transfer

all: setup build

setup:
	$(DOCKER) build -t $(DEVELOPMENT_IMAGE) -f $(DEVELOPMENT_DOCKERFILE) .

build:
	$(DOCKER) run --rm -u $(UID):$(GID) -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		$(MAKE) --no-print-directory -f $(DEVELOPMENT_MAKEFILE) -C /work

clean:
	$(DOCKER) run --rm -u $(UID):$(GID) -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		$(MAKE) --no-print-directory -f $(DEVELOPMENT_MAKEFILE) -C /work clean

transfer:
	@echo Checking scp...
	@scp --version >nul 2>&1 || (echo "SCP not found in make environment" && exit 1)
	$(SCP) $(SCP_FLAGS) $(BINARY) $(SCP_DIRECTORY)
	$(SCP) $(SCP_FLAGS) $(CONFIGURATION_FILE) $(SCP_DIRECTORY)
