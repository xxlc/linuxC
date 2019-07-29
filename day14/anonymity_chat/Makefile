CC:=gcc
SRCS:=$(wildcard *.c)
BINS:=$(SRCS:%.c=%)
LINKERFLAG:=-lpthread
CFLAGS:=-Wall -g
all:${BINS}
%:%.c
	${CC} $< -o $@ ${CFLAGS} ${LINKERFLAG}
