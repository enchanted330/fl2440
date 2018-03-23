# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
CFLAGS += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)
# call from kernel build system
scull-objs := main.o pipe.o access.o
obj-m	:= scull.o
else
KERNELDIR ?= /usr/Devices/L350/linux-2.6.22.1
PWD       := $(shell pwd)
CC	  := /opt/buildroot_350/build_arm/staging_dir/bin/arm-linux-uclibc-gcc
HOSTCC	  := gcc
export CC HOSTCC

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
