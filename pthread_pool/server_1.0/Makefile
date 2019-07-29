SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:=-Wall -g
CC:=gcc

main:$(OBJS)
	$(CC) $(OBJS) -o main -lpthread
clean:
	rm -rf $(OBJS) main
