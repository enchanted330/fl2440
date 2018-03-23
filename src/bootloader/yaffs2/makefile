
# ***********************************************************************
# *        File:  makefile
# *     Version:  1.0.0
# *   Copyright:  2011 (c) Guo Wenxue <guowenxue@gmail.com>
# * Description:  Makefile used to cross compile the ASM source code
# *   ChangeLog:  1, Release initial version on "Sun Mar 20 18:41:04 CST 2011"
# *
# ***********************************************************************
#

APP_NAME=bootstrap
INST_PATH=/tftp
SRC_TOP=$(CURDIR)

OBJS_MAIN := bootstrap.o start.o 
OBJS_BSP += ${SRC_TOP}/bsp/*.o
OBJS_COMMON += ${SRC_TOP}/common/*.o
OBJS_YAFFS2 += ${SRC_TOP}/yaffs2/*.o

CFLAGS+=-I${SRC_TOP}/common
CFLAGS+=-I${SRC_TOP}/bsp
CFLAGS+=-I${SRC_TOP}/yaffs2 

# Set the stack top base address here
TEXT_BASE=0x31000000
STACK_BASE=0x31010000
MALLOC_SIZE=0x100000
CFLAGS+=-DTEXT_BASE=$(TEXT_BASE) -DSTACK_BASE=${STACK_BASE} -DCONFIG_SYS_MALLOC_LEN=${MALLOC_SIZE}

CFLAGS+=-DDEBUG

gccincdir := $(shell $(CC) -print-file-name=include)
CFLAGS += -ffreestanding -nostdinc -isystem $(gccincdir) -pipe 
CFLAGS+= -fno-strict-aliasing  -fno-common -ffixed-r8 -msoft-float 
CFLAGS+=-march=armv4t -Wall -Wno-unused -Wstrict-prototypes -fno-stack-protector
CFLAGS += -fno-builtin  
LDFLAGS=-Bstatic -T$(APP_NAME).lds -Ttext $(TEXT_BASE)
AFLAGS := $(CFLAGS) -D__ASSEMBLY__
export CFLAGS
export AFLAGS

#CFLAGS+=-Os -D__KERNEL__ -DCONFIG_ARM -D__ARM__ 
PLATFORM_LIBS += -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc
#PLATFORM_LIBS += -L/opt/buildroot-2011.11/arm920t/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/lib/ -lc 

CROSS_COMPILE=/opt/buildroot-2012.08/arm920t/usr/bin/arm-linux-
AS  = $(CROSS_COMPILE)as
LD  = $(CROSS_COMPILE)ld
CC  = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
AR  = $(CROSS_COMPILE)ar
NM  = $(CROSS_COMPILE)nm
LDR = $(CROSS_COMPILE)ldr
STRIP   = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB  = $(CROSS_COMPILE)RANLIB

export  CROSS_COMPILE AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP CFLAGS

ALL: clean bsp_lib common_lib yaffs2_lib ${OBJS_MAIN} 
	echo ${OBJS_BSP}
	cd $(SRC_TOP) 
	${LD} $(LDFLAGS) \
	--start-group ${OBJS_BSP} ${OBJS_COMMON} ${OBJS_YAFFS2} $(OBJS_MAIN) \
	--end-group $(PLATFORM_LIBS) \
	-Map $(APP_NAME).map  -o $(APP_NAME).elf
	${OBJCOPY} -O binary $(APP_NAME).elf $(APP_NAME).bin
	@chmod 777 $(APP_NAME).bin
	@rm -f *.elf *.o
	@make install

bsp_lib:
	@make -C bsp

common_lib:
	@make -C common

yaffs2_lib:
	@make -C yaffs2

%.o: %.S
	 $(CC) $(AFLAGS) -c -o $@ $<

%.o: %.c
	 $(CC) $(CFLAGS) -c -o $@ $<

install: $(APP_NAME).bin
	cp $(APP_NAME).bin $(INST_PATH) -f 

clean:
	@find $(OBJTREE) -type f \
			\( -name 'core' -o -name '*.bak' -o -name '*~' -o -name .depend \
			-o -name '*.o'  -o -name '*.a' -o -name '*.elf' \) -print \
			| xargs rm -f
	@rm -f $(APP_NAME).bin
	@rm -f $(APP_NAME).map
	@make clean -C yaffs2

distclean clear: clean
	@rm -f cscope.* tags


