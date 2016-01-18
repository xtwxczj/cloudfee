#include<iostream>
#include "../header/main.h"
#include <time.h>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include <json/json.h> 
#include <map>
#include <pthread.h>



static class MY_SQLC msql;
static class MY_SQLC msql1;
static class MY_SQLC msql2;
static char cmd[10240] = {0};
//char host[]="192.168.81.38";
char* var;
char* keystoneAddr;
char* novaAddr;
char* ceilometerAddr;
char* tenantName;
char* cloudUsername;
char* cloudPassword;

char* mysqlHost;
char* mysqlUsername;
char* mysqlPwd;
char* database;
string token;
string tenant_id;


string json_dir;

int rc =0;

char* key = new char[20];

typedef map<string,string> STR_STR_MAP;
STR_STR_MAP user_map;
STR_STR_MAP image_map;
STR_STR_MAP flavor_vcpu_map;
STR_STR_MAP flavor_ram_map;
STR_STR_MAP flavor_disk_map;
STR_STR_MAP tenant_instance_quota;
STR_STR_MAP tenant_core_quota;
STR_STR_MAP tenant_ram_quota;
using namespace std;

int group_scan()
{
	char* group_name = "null";	
	int vm_count = 0;				//给该项目组运行的虚拟机的数量
	int	 vcpus_count = 0;					//虚拟机占用的虚核数量
	int	 mem_count = 0;				//虚拟机占用的内存数量	
	int disk_count = 0;

	memset(cmd,0,sizeof(cmd));		
	snprintf(cmd,sizeof(cmd),"select distinct(group_name) from %s.group_vm_map;",database);	
	msql.free_res();
	msql.query_select(cmd);
	int record_count = msql.getrecordcount();
	for(int i=0;i<record_count;i++)
	{
		if(!msql.FetchRow())
				break;
		strcpy(key,"group_name");
		group_name = msql.GetField(key);
		memset(cmd,0,sizeof(cmd));		
		snprintf(cmd,sizeof(cmd),"select count(vm_name) as vm_count,sum(vcpus) as vcpus_count,sum(mem_mb) as mem_count ,sum(disk_gb) as disk_count from %s.group_vm_map where IsDeleted = 0 and group_name='%s';",database,group_name);	
		msql1.free_res();
		msql1.query_select(cmd);
		if(!msql1.FetchRow())
				continue;
		else
		{
			strcpy(key,"vm_count");
			vm_count = atoi(msql1.GetField(key));
			strcpy(key,"vcpus_count");
			vcpus_count = atoi(msql1.GetField(key));
			strcpy(key,"mem_count");
			mem_count = atoi(msql1.GetField(key));
			strcpy(key,"disk_count");
			disk_count = atoi(msql1.GetField(key));
			
		}
		
		memset(cmd,0,sizeof(cmd));	
		snprintf(cmd,sizeof(cmd),"select * from %s.group where group_name='%s';",database,group_name);
		msql2.free_res();
		msql2.query_select(cmd);
		if(!msql2.getrecordcount())
		{
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"insert into %s.group set group_name='%s',running_vms=%d,cores=%d,ram=%d,checked_at=NOW();",database,group_name,vm_count,vcpus_count,mem_count);
			msql2.insert(cmd);
		}
		else
		{
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"update %s.group set running_vms=%d,cores=%d,ram=%d,checked_at=NOW() where group_name='%s';",database,vm_count,vcpus_count,mem_count,group_name);
			msql2.insert(cmd);
		}
	}

	return 1;
}

int main(int argc,char* argv[])
{
	var = "CLOUDFEE_DB_ADDR";
	mysqlHost = getenv(var);
	//char username[] = "root";
	//char pwd[] = "mysql";
	var = "CLOUDFEE_DB_USER";
	mysqlUsername = getenv(var);
	var = "CLOUDFEE_DB_PWD";
	mysqlPwd = getenv(var);
	//char database[] = "nova";
	var = "CLOUDFEE_DB";
	database = getenv(var);

	var = "KEYSTONE_ADDR";
	keystoneAddr = getenv(var);
	var = "NOVA_ADDR";
	novaAddr = getenv(var);
	var = "CEILOMETER_ADDR";
	ceilometerAddr = getenv(var);

	var = "TENANT_NAME";
	tenantName = getenv(var);
	var = "CLOUD_USERNAME";
	cloudUsername = getenv(var);
	var = "CLOUD_PASSWORD";
	cloudPassword = getenv(var);

	var = "CLOUDFEE_JSON_DIR";
	json_dir = getenv(var);

        int flag;
	char msg[sizeof(cmd)] = {0};

			
	rc = msql.init(mysqlHost,mysqlUsername,mysqlPwd,database);
	if(rc == -1) {printf("error for mysql init\n");printf(msql.getmsg());return 0;}
	
	rc = msql1.init(mysqlHost,mysqlUsername,mysqlPwd,database);
	if(rc == -1) {printf("error for mysql1 init\n");printf(msql1.getmsg());return 0;}
	
	rc = msql2.init(mysqlHost,mysqlUsername,mysqlPwd,database);
	if(rc == -1) {printf("error for mysql1 init\n");printf(msql1.getmsg());return 0;}
	
	Json::Reader reader;// 解析json用Json::Reader   
        Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         
	Json::Value root2;
	
	//获取token
    std::ifstream is;  
	std::ifstream is2;





	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -s -X POST http://%s:5000/v2.0/tokens -H \"Content-Type: application/json\" -d '{\"auth\": {\"tenantName\": \"'\"%s\"'\", \"passwordCredentials\":{\"username\": \"'\"%s\"'\", \"password\": \"'\"%s\"'\"}}}'  | python -mjson.tool > %s/token.json",keystoneAddr,tenantName,cloudUsername,cloudPassword,json_dir.c_str());
	system(cmd);
	//char filename[100];memset(filename,0,sizeof(filename);) = "/root/token.json";
	char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/token.json",json_dir.c_str());
	is.open(filename, std::ios::binary );  
	if (reader.parse(is, root, 0))  
	{
		token = root["access"]["token"]["id"].asString();
		tenant_id = root["access"]["token"]["tenant"]["id"].asString();
	}
	is.close();
	group_scan();
/*
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	cout<<"start sec: "<<tv.tv_sec<<endl;
	cout<<"start usec: "<<tv.tv_usec<<endl;
	
	pthread_t vm_scan_tid[2];
	int ret = pthread_create(&vm_scan_tid[0],NULL,cloud_vm_scan,NULL);
	if(ret!=0)
	{
		cout<<"pthread_create error:error_code="<<ret<<endl;

	}
	int ret2 = pthread_create(&vm_scan_tid[1],NULL,cloud_host_scan,NULL);
	if(ret2!=0)
	{
		cout<<"pthread_create error:error_code="<<ret2<<endl;

	}
	gettimeofday(&tv,&tz);
	cout<<"end sec: "<<tv.tv_sec<<endl;
	cout<<"end usec: "<<tv.tv_usec<<endl;
	
	
	pthread_exit(NULL);
	//cloud_vm_scan();	
	exit(EXIT_SUCCESS);
*/
}
