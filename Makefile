CC =g++
CFLAGS=-Wall
COMPILE=$(CC) $(CFLAGS) -c
BUILD=$(CC) $(CFLAGS)
OBJ_DIR:=$(shell pwd)/obj
SRC_DIR:=$(shell pwd)/src
H_DIR:=$(shell pwd)/header
BIN_DIR:=$(shell pwd)/bin

all:$(BIN_DIR)/netdb
$(BIN_DIR)/netdb:$(OBJ_DIR)/main.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/netdb $(OBJ_DIR)/main.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/main.o:$(SRC_DIR)/main.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/main.o -g $(SRC_DIR)/main.cpp  -I /usr/local/mongo/include/  
$(OBJ_DIR)/mysqlc.o:$(SRC_DIR)/mysqlc.cpp $(H_DIR)/mysqlc.h
	$(COMPILE) -o $(OBJ_DIR)/mysqlc.o -g $(SRC_DIR)/mysqlc.cpp -I /usr/local/mysql/include 
clean:
	-rm  -f $(OBJ_DIR)/*.o $(BIN_DIR)/netdb 



