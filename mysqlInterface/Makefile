SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:=-Wall -g -I /usr/include/mysql/
CC:=gcc

main:$(OBJS)
	$(CC) $(OBJS) -o main -lmysqlclient
clean:
	rm -rf $(OBJS) main
