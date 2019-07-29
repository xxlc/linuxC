SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:=-Wall -g
CC:=gcc

test:$(OBJS)
	$(CC) $(OBJS) -o test
clean:
	rm -rf $(OBJS) test
