#include<iostream>
#include "../header/main.h"
#include <time.h>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include <json/json.h> 
#include <map>
#include <pthread.h>
#define THREAD_NUM 5 


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

	typedef struct arg{ 
		string token;
		char* novaAddr;
		string tenant_id;
		string str_tenant_id;
		string json_dir;
		Json::Value root;
		int num;
		int i;
		int max;
	}ARG;
pthread_mutex_t sum_mutex;		

void* quota_scan(void* pM)
{
	//由tenant_id去找quota_limit
	ARG* test = (ARG*)pM;
	string token = test->token;
	char* novaAddr = test->novaAddr;
	string tenant_id = test->tenant_id;
	//string str_tenant_id = test->str_tenant_id;
	string json_dir = test->json_dir;
	Json::Value root = test->root;
	int num = test->num;
	int begin = num*(test->i);
	pthread_mutex_unlock(&sum_mutex);
	int max = test->max;
	for(int i = begin;i<begin+num;i++)
	{
		if(i>=max)
			break;
		string str_tenant_id = root["tenants"][i]["id"].asString();
		string str_tenant_name = root["tenants"][i]["name"].asString();
		cout<<"i = "<<i<<" , str_tenant_id = "<<str_tenant_id<<endl;
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/os-quota-sets/%s |python -mjson.tool >%s/tenant_quota/%s_quota.json",token.c_str(),novaAddr,tenant_id.c_str(),str_tenant_id.c_str(),json_dir.c_str(),str_tenant_id.c_str());
		system(cmd);
	}
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


	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	cout<<"start sec: "<<tv.tv_sec<<endl;
	cout<<"start usec: "<<tv.tv_usec<<endl;
	

	//对tenant_id和tenant_name生成map 对tenant_id和quota_limit生成map
	string str_tenant_id;
	string str_tenant_name;
	string str_cores;
	string str_instances;
	string str_ram;
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:35357/v2.0/tenants |python -mjson.tool>%s/tenant.json",token.c_str(),keystoneAddr,json_dir.c_str());
	system(cmd);
	//filename = "/root/tenant.json";
	sprintf(filename,"%s/tenant.json",json_dir.c_str());
	is.open(filename,std::ios::binary);
	if(reader.parse(is,root,0))
	{
		int tenant_size = root["tenants"].size();
		int turn = tenant_size/THREAD_NUM;
		pthread_t tids[THREAD_NUM];
		//pthread_t tids[tenant_size];
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
		pthread_mutex_init(&sum_mutex,NULL);

		ARG* t;
		t = new ARG;

		t->token = token;
		t->novaAddr = novaAddr;
		t->tenant_id = tenant_id;
		t->root = root;
		t->json_dir = json_dir;
		t->num = turn+1;
		t->max = tenant_size;	
		for(int i = 0;i < THREAD_NUM;i++)
		{
			pthread_mutex_lock(&sum_mutex);
			t->i = i;
			int ret = pthread_create(&tids[i],&attr,quota_scan,(void* )t);
			if(ret!=0)
			{
				cout<<"pthread_create error:error_code="<<ret<<endl;

			}	
		}
	//	is.close();
	//	free(t);
	}
	/*
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

*/
	gettimeofday(&tv,&tz);
	cout<<"end sec: "<<tv.tv_sec<<endl;
	cout<<"end usec: "<<tv.tv_usec<<endl;
	
	
	pthread_exit(NULL);
	//cloud_vm_scan();	
	exit(EXIT_SUCCESS);

}
