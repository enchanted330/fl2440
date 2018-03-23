
VPATH=.
SRCS=$(wildcard ${VPATH}/*.c)
OBJS=$(patsubst %.c,%.o, ${SRCS})

test: entry ${OBJS}
	echo $@ 
	echo $?
	${CC} $? -o $@ ${CFLAGS}

entry:
	@echo ${SRCS}
	@echo ${OBJS}

%.o: %.c
	${CC} -c $< ${CFLAGS}
