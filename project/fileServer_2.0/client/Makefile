OBJ_DIR = ./bin/
SRC_DIR = ./
INC_DIR = ./include/
SRC = $(wildcard $(SRC_DIR)*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
OBJ_WITHOUT_DIR = $(notdir $(OBJ))
OBJ_WITH_DIR = $(addprefix $(OBJ_DIR),$(OBJ_WITHOUT_DIR))
CFLAGS:=-D DEBUG -Wall -g -I$(INC_DIR) -I/usr/include/mysql/
CC:=gcc
EXE = client.exe

$(EXE):$(OBJ_WITH_DIR)
	$(CC) $(OBJ_WITH_DIR) -o $(EXE) -lpthread -lmysqlclient
$(OBJ_DIR)%.o:$(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf $(OBJ_DIR)*.o $(EXE)
