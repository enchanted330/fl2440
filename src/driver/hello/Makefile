obj-m := hello.o
modules:
	make -C /lib/modules/`uname -r`/build/ M=`pwd` modules
	make clean
clean:
	rm -f *.ko.* *.o *mod.c *.order *.symvers
