SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:=-Wall -g
CC:=gcc

main_client:$(OBJS)
	$(CC) $(OBJS) -o main_client
clean:
	rm -rf $(OBJS) main_client
