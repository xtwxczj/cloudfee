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

int hypervisor_history_scan()
{
	char* host_ip = "null";			//宿主机IP
	int host_vcpus = 0;				//宿主机总的cpu核数
	int host_memory_mb = 0;			//宿主机的内存数量
	int host_disk_gb = 0;			//宿主机的磁盘大小
	char* host_created_at = "null";	//宿主机加入的时间
	char* host_deleted_at = "null";	//宿主机离开的时间
	int host_free_vcpus = 0;	    //宿主机上剩余的内核数
	int host_free_ram_mb = 0;		//宿主机剩余的内存数量
	int host_free_disk_gb = 0;		//宿主机剩余的磁盘大小
	int running_vms = 0;			//宿主机上运行的虚拟机数量
	char* host_domain = "null";		//宿主机域名

	//char* availability_zone = "null";     //


	//	char flavor_name[64];		//资源匹配的名称
	char* flavor_id = "null";			//资源匹配的类型id

	double bonus = 0;
	memset(cmd,0,sizeof(cmd));		
	snprintf(cmd,sizeof(cmd),"select * from %s.hypervisor",database);	
	msql.free_res();
	msql.query_select(cmd);
	int record_count = msql.getrecordcount();
	for(int i=0;i<record_count;i++)
	{
		if(!msql.FetchRow())
			break;
		strcpy(key,"host_ip");
		host_ip = msql.GetField(key);

		strcpy(key,"vcpu");
		host_vcpus = atoi(msql.GetField(key));

		strcpy(key,"mem_mb");
		host_memory_mb = atoi(msql.GetField(key));

		strcpy(key,"disk_gb");
		host_disk_gb = atoi(msql.GetField(key));

		strcpy(key,"create_at");
		host_created_at = msql.GetField(key);

		strcpy(key,"delete_at");
		host_deleted_at = msql.GetField(key);
		string t(host_deleted_at);
		if(t=="0.0000000")
			host_deleted_at = "0000-00-00 00:00:00";

		strcpy(key,"vcpus_free");
		host_free_vcpus = atoi(msql.GetField(key));

		strcpy(key,"mem_mb_free");
		host_free_ram_mb = atoi(msql.GetField(key));

		strcpy(key,"disk_gb_free");
		host_free_disk_gb = atoi(msql.GetField(key));

		strcpy(key,"running_vms");
		running_vms = atoi(msql.GetField(key));

		strcpy(key,"host_name");
		host_domain = msql.GetField(key);

		strcpy(key,"bonus");
		bonus = atof(msql.GetField(key));

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd), "insert into %s.hypervisor_history set host_name='%s',host_ip='%s',vcpu=%d,mem_mb=%d,disk_gb=%d,create_at='%s',delete_at='%s',vcpus_free='%d',mem_mb_free='%d',disk_gb_free='%d',bonus='%f',running_vms='%d',check_at=NOW();",
				database,host_domain, host_ip,host_vcpus, host_memory_mb, host_disk_gb, host_created_at, host_deleted_at, host_free_vcpus, host_free_ram_mb, host_free_disk_gb, bonus,running_vms);
		msql1.insert(cmd);


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
	hypervisor_history_scan();
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
