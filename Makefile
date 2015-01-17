all: kernel

kernel:
	$(MAKE) -C kernel

modules:
	$(MAKE) -C modules

.PHONY: all kernel modules

