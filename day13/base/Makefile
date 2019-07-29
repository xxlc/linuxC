CC:=gcc
SRCS:=$(wildcard *.c)
BINS:=$(SRCS:%.c=%)
LINKERFLAG:=-lpthread
CFLAGS:=-Wall
all:${BINS}
%:%.c
	${CC} $< -o $@ ${LINKERFLAG}
