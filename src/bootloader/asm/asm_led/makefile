# ***********************************************************************
# *        File:  makefile
# *     Version:  1.0.0
# *   Copyright:  2011 (c) Guo Wenxue <guowenxue@gmail.com>
# * Description:  Makefile used to cross compile the ASM and C source code
# *   ChangeLog:  1, Release initial version on "Mon Mar 21 21:09:52 CST 2011"
# *
# ***********************************************************************

BINAME = led
TEXTBASE = 0x33000000
#TEXTBASE =0

CROSS = /opt/buildroot-2012.08/arm920t/usr/bin/arm-linux-
CC      = $(CROSS)gcc
LD      = $(CROSS)ld
AR      = $(CROSS)ar
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
STRIP   = $(CROSS)strip
READELF = $(CROSS)readelf

CFLAGS  = -g -O2 -Wall -nostdinc -nostdlib -fno-builtin
AFLAGS  = $(CFLAGS) -D__ASSEMBLY__

LDFLAGS  = -Ttext $(TEXTBASE)

SRC_C   = $(wildcard *.c)
SRC_S   = $(wildcard *.S)
OBJ_C   = $(patsubst %.c,%.o,$(SRC_C)) 
OBJ_S   = $(patsubst %.S,%.o,$(SRC_S)) 

OBJ_ALL = $(OBJ_C) $(OBJ_S) 

.PHONY : all
all: ${OBJ_ALL}
	${LD} $(LDFLAGS) -o ${BINAME}.elf ${OBJ_ALL}
	${OBJCOPY} -O binary -S ${BINAME}.elf ${BINAME}.bin
	rm -f *.elf *.o

%.o: %.S
	        $(CC) $(AFLAGS) -c -o $@ $<
%.o: %.c
	        $(CC) $(CFLAGS) -c -o $@ $<

install:
	cp ${BINAME}.bin ~/winxp -f --reply=yes

clean:
	rm -f *.elf *.o
	rm -f ${BINAME}.bin
