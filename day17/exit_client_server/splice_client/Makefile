SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:=-Wall -g
CC:=gcc

client:$(OBJS)
	$(CC) $(OBJS) -o client
clean:
	rm -rf $(OBJS) main
