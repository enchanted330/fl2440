TEST_APP=event_button

KERNEL_VER = linux-3.0
LINUX_SRC ?= ../../linux/kernel/$(KERNEL_VER)

CROSS_COMPILE=/opt/buildroot-2012.08/arm920t/usr/bin/arm-linux-

PWD := $(shell pwd)

obj-m += kbd_device.o
obj-m += kbd_driver.o

modules:
	@make -C $(LINUX_SRC) M=$(PWD) modules
	@make clear
	@chmod a+x *.ko && cp *.ko /tftp
	@make testapp

clear:
	@rm -f *.o *.cmd *.mod.c
	@rm -rf  *~ core .depend  .tmp_versions Module.symvers modules.order -f
	@rm -f .*ko.cmd .*.o.cmd .*.o.d

clean: clear
	@rm -f  *.ko ${TEST_APP}

testapp:
	${CROSS_COMPILE}gcc ${TEST_APP}.c -o ${TEST_APP}
