PROJECT := $(CURDIR)

MAKE   := make
DOCKER := docker

DEVELOPMENT_IMAGE      := osciboot-development
DEVELOPMENT_DOCKERFILE := Dockerfile.development
DEVELOPMENT_MAKEFILE   := Makefile.build

UID := $(shell id -u)
GID := $(shell id -g)

all: setup build

setup:
	@$(DOCKER) image inspect $(DEVELOPMENT_IMAGE) > /dev/null 2>&1 || \
		$(DOCKER) -t $(DEVELOPMENT_IMAGE) -f $(DEVELOPMENT_DOCKERFILE) .

build:
	$(DOCKER) run --rm -u $(UID):$(GID) -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		$(MAKE) --no-print-directory -f $(DEVELOPMENT_MAKEFILE) -C /work

clean:
	$(DOCKER) run --rm -u $(UID):$(GID) -v $(PROJECT):/work $(DEVELOPMENT_IMAGE) \
		$(MAKE) --no-print-directory -f $(DEVELOPMENT_MAKEFILE) -C /work clean
