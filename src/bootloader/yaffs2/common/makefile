#*********************************************************************************
#      Copyright:  (C) 2012 CoherentPlus Sdn. Bhd.
#                  All rights reserved.
#
#       Filename:  Makefile
#    Description:  This is the common subdir Makefile which to compile all the C
#                  source code to object files and then generate the shared or 
#                  static library named lib$(FOLDER_NAME).a orlib $(FOLDER_NAME).so,
#                  which depends on the variable $LINK_MODE.
#                      
#        Version:  1.0.0(10/08/2011~)
#                  Author:  Guo Wenxue <guowenxue@gmail.com>
#      ChangeLog:  1, Release initial version on "10/08/2011 01:29:33 AM"
#                       
#********************************************************************************/

PWD=$(shell pwd)
LOCAL_COMPILE=YES
LINK_MODE=STATIC

#If wanna compile in the subdir, not called by top makefile, uncomment it
ifneq (${TOP_COMPILE}, YES) 
LOCAL_COMPILE=YES
endif

LIBNAME=$(shell basename ${PWD})
STALIB=lib${LIBNAME}.a
DYNLIB=lib${LIBNAME}.so

VPATH= .
SRCS = $(wildcard ${VPATH}/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

#======================================================
#  ---> Doesn't call by top makefile, compile by local
#======================================================
ifeq (${LOCAL_COMPILE}, YES)
ARCH?=arm920t
#ARCH?=i386
CFLAGS+=-fPIC
TMP=$(shell echo $(ARCH) | tr "[A-Z]" "[a-z]")
ifneq (,$(filter i386,$(TMP)))
    CROSS_COMPILE=
else
    CROSS_COMPILE=/opt/buildroot-2011.11/$(ARCH)/usr/bin/arm-linux-
endif

PRJDIR?=$(shell pwd)
CFLAGS+=-I${PRJDIR}
CC = ${CROSS_COMPILE}gcc
AR = ${CROSS_COMPILE}ar

endif #End local compile

ifeq ("${LINK_MODE}", "STATIC")
	LIBS = ${STALIB} 
else 
	LIBS=${DYNLIB}
endif

all: entry ${LIBS} install

entry: 
	@echo " ";
	@echo " =========================================================";
	@echo " **     Compile subdir ${LIBNAME} for ${ARCH}             ";
	@echo " =========================================================";

#$(LD) -g --relocatable $(OBJS) -o lib${LIBNAME}.o
${STALIB}:	$(OBJS) 
	$(AR) -rcu $@ $(OBJS)

${DYNLIB}:   $(OBJS) 
	$(CC) -fPIC -shared -o $@ $(OBJS)

%.o : %.c
	$(CC) -c $< $(CFLAGS)

tag: 
	@ctags --c-kinds=+defglmnstuvx --langmap=c:.c.h.ho.hem.het.hec.hev.him.hit.hic.hiv -R .  
	@cscope -Rbq

install:
	@if [ ! -z "${LIBS_PATH}" ] ; then \
		mkdir -p ${LIBS_PATH} ; \
		cp ${LIBS} ${LIBS_PATH}; \
	fi;


clean:
	@rm -f *.o
	@rm -rf *.gdb *.a *.so

distclean: clean
	@rm -f  tags cscope*

.PHONY: clean entry
