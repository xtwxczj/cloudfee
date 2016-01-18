CC =g++
CFLAGS=-Wall
COMPILE=$(CC) $(CFLAGS) -c
BUILD=$(CC) $(CFLAGS)
OBJ_DIR:=$(shell pwd)/obj
SRC_DIR:=$(shell pwd)/src
H_DIR:=$(shell pwd)/header
BIN_DIR:=$(shell pwd)/bin

all:$(BIN_DIR)/netdb $(BIN_DIR)/cloud_vm_scan $(BIN_DIR)/single_user_scan $(BIN_DIR)/hypervisor_history_scan $(BIN_DIR)/group_scan $(BIN_DIR)/group_history_scan $(BIN_DIR)/cloud_scan $(BIN_DIR)/single_cloud_vm_scan $(BIN_DIR)/single_score

$(BIN_DIR)/netdb:$(OBJ_DIR)/main.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/netdb $(OBJ_DIR)/main.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(BIN_DIR)/cloud_vm_scan:$(OBJ_DIR)/cloud_vm_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/cloud_vm_scan $(OBJ_DIR)/cloud_vm_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/cloud_vm_scan.o:$(SRC_DIR)/cloud_vm_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/cloud_vm_scan.o -g $(SRC_DIR)/cloud_vm_scan.cpp  -I /usr/local/mongo/include/  
$(BIN_DIR)/single_user_scan:$(OBJ_DIR)/single_user_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/single_user_scan $(OBJ_DIR)/single_user_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/single_user_scan.o:$(SRC_DIR)/single_user_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/single_user_scan.o -g $(SRC_DIR)/single_user_scan.cpp  -I /usr/local/mongo/include/  
$(BIN_DIR)/tenant_quota:$(OBJ_DIR)/tenant_quota.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/tenant_quota $(OBJ_DIR)/tenant_quota.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/tenant_quota.o:$(SRC_DIR)/tenant_quota.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/tenant_quota.o -g $(SRC_DIR)/tenant_quota.cpp  -I /usr/local/mongo/include/  

$(BIN_DIR)/user_history_scan:$(OBJ_DIR)/user_history_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/user_history_scan $(OBJ_DIR)/user_history_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/user_history_scan.o:$(SRC_DIR)/user_history_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/user_history_scan.o -g $(SRC_DIR)/user_history_scan.cpp  -I /usr/local/mongo/include/  

$(BIN_DIR)/hypervisor_history_scan:$(OBJ_DIR)/hypervisor_history_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/hypervisor_history_scan $(OBJ_DIR)/hypervisor_history_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/hypervisor_history_scan.o:$(SRC_DIR)/hypervisor_history_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/hypervisor_history_scan.o -g $(SRC_DIR)/hypervisor_history_scan.cpp  -I /usr/local/mongo/include/  

$(BIN_DIR)/group_scan:$(OBJ_DIR)/group_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/group_scan $(OBJ_DIR)/group_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/group_scan.o:$(SRC_DIR)/group_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/group_scan.o -g $(SRC_DIR)/group_scan.cpp  -I /usr/local/mongo/include/  

$(BIN_DIR)/group_history_scan:$(OBJ_DIR)/group_history_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/group_history_scan $(OBJ_DIR)/group_history_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/group_history_scan.o:$(SRC_DIR)/group_history_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/group_history_scan.o -g $(SRC_DIR)/group_history_scan.cpp  -I /usr/local/mongo/include/  

$(BIN_DIR)/cloud_scan:$(OBJ_DIR)/cloud_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/cloud_scan $(OBJ_DIR)/cloud_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/cloud_scan.o:$(SRC_DIR)/cloud_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/cloud_scan.o -g $(SRC_DIR)/cloud_scan.cpp  -I /usr/local/mongo/include/  

$(BIN_DIR)/single_cloud_vm_scan:$(OBJ_DIR)/single_cloud_vm_scan.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/single_cloud_vm_scan $(OBJ_DIR)/single_cloud_vm_scan.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/single_cloud_vm_scan.o:$(SRC_DIR)/single_cloud_vm_scan.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/single_cloud_vm_scan.o -g $(SRC_DIR)/single_cloud_vm_scan.cpp  -I /usr/local/mongo/include/  

$(BIN_DIR)/single_score:$(OBJ_DIR)/single_score.o $(OBJ_DIR)/mysqlc.o  
	$(BUILD) -o $(BIN_DIR)/single_score $(OBJ_DIR)/single_score.o $(OBJ_DIR)/mysqlc.o -I /usr/local/mongo/include/  -l mysqlclient -l curl -ljsoncpp
$(OBJ_DIR)/single_score.o:$(SRC_DIR)/single_score.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/single_score.o -g $(SRC_DIR)/single_score.cpp  -I /usr/local/mongo/include/  


$(OBJ_DIR)/main.o:$(SRC_DIR)/main.cpp $(H_DIR)/main.h
	$(COMPILE) -o $(OBJ_DIR)/main.o -g $(SRC_DIR)/main.cpp  -I /usr/local/mongo/include/  
$(OBJ_DIR)/mysqlc.o:$(SRC_DIR)/mysqlc.cpp $(H_DIR)/mysqlc.h
	$(COMPILE) -o $(OBJ_DIR)/mysqlc.o -g $(SRC_DIR)/mysqlc.cpp -I /usr/local/mysql/include 
clean:
	-rm  -f $(OBJ_DIR)/*.o $(BIN_DIR)/netdb 



