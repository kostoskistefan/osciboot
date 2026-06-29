PROJECT = $(CURDIR)

BINARY 		       = bin/osciboot
CONFIGURATION_FILE = osciboot.conf

DEVELOPMENT_IMAGE      = osciboot-development
DEVELOPMENT_DOCKERFILE = Dockerfile.development
DEVELOPMENT_MAKEFILE   = Makefile.build

UID = $(shell id -u)
GID = $(shell id -g)

SSH_PASSWORD = rd2423
SSH_FLAGS = \
	-o LogLevel=ERROR \
	-o StrictHostKeyChecking=no \
	-o UserKnownHostsFile=/dev/null \
	-o KexAlgorithms=diffie-hellman-group1-sha1 \
	-o HostKeyAlgorithms=ssh-rsa \
	-o MACs=hmac-sha1
SSH_ADDRESS = root@172.16.5.94
SCP_DIRECTORY = /home/dso

.PHONY: all setup build clean transfer connect

all: setup build

setup:
	@docker build -t $(DEVELOPMENT_IMAGE) -f $(DEVELOPMENT_DOCKERFILE) .

build:
	@docker run --rm -u $(UID):$(GID) -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		make --no-print-directory -f $(DEVELOPMENT_MAKEFILE) -C /work

clean:
	@docker run --rm -u $(UID):$(GID) -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		make --no-print-directory -f $(DEVELOPMENT_MAKEFILE) -C /work clean

transfer:
	@echo 'Transferring $(BINARY)...'
	@docker run --rm -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		sshpass -p $(SSH_PASSWORD) scp $(SSH_FLAGS) $(BINARY) $(SSH_ADDRESS):$(SCP_DIRECTORY)
	@echo 'Transferring $(CONFIGURATION_FILE)...'
	@docker run --rm -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		sshpass -p $(SSH_PASSWORD) scp $(SSH_FLAGS) $(CONFIGURATION_FILE) $(SSH_ADDRESS):$(SCP_DIRECTORY)
	@echo 'Transfer complete'

connect:
	@docker run --rm -it -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		sshpass -p $(SSH_PASSWORD) ssh $(SSH_FLAGS) $(SSH_ADDRESS)
