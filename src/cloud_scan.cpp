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

int cloud_scan()
{
	int SumUser = 0;
	int SumGroup = 0;
	int SumVm = 0;
	int SumVmActive = 0;
	int SumVmStopped = 0;
	int SumVcpu = 0;
	int SumMemMb = 0;
	int SumDiskGb = 0;
	int SumVcpuUsed = 0;
	int SumMemUsed = 0;
	double SumVcpuHour = 0;
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select count(user_name) as user_count from %s.userinfo where id=0;",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumUser = 0;
	}
	else
	{
		strcpy(key,"user_count");
		SumUser = atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select count(group_name) as group_count from %s.group;",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumGroup = 0;
	}
	else
	{
		strcpy(key,"group_count");
		SumGroup = atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select count(vm_name) as vm_count from %s.vms where vm_state = 'active' or vm_state = 'stopped';",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumVm = 0;
	}
	else
	{
		strcpy(key,"vm_count");
		SumVm = atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select count(vm_name) as vm_count from %s.vms where vm_state = 'active';",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumVmActive = 0;
	}
	else
	{
		strcpy(key,"vm_count");
		SumVmActive = atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select count(vm_name) as vm_count from %s.vms where vm_state = 'stopped';",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumVmStopped = 0;
	}
	else
	{
		strcpy(key,"vm_count");
		SumVmStopped = atoi(msql.GetField(key));
	}
		
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select sum(vcpu) as vcpu_count from %s.hypervisor;",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumVcpu = 0;
	}
	else
	{
		strcpy(key,"vcpu_count");
		SumVcpu = atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select sum(mem_mb) as mem_count from %s.hypervisor;",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumMemMb = 0;
	}
	else
	{
		strcpy(key,"mem_count");
		SumMemMb = atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select sum(disk_gb) as disk_count from %s.hypervisor;",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumDiskGb = 0;
	}
	else
	{
		strcpy(key,"disk_count");
		SumDiskGb = atoi(msql.GetField(key));
	}
	
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select sum(vcpus_free) as vcpufree_count from %s.hypervisor;",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumVcpuUsed = 0;
	}
	else
	{
		strcpy(key,"vcpufree_count");
		SumVcpuUsed = SumVcpu-atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select sum(mem_mb_free) as memfree_count from %s.hypervisor;",database);
	msql.free_res();
	msql.query_select(cmd);
	if(!msql.FetchRow()||!msql.getrecordcount())
	{
		SumMemUsed = 0;
	}
	else
	{
		strcpy(key,"memfree_count_count");
		SumMemUsed = SumMemMb-atoi(msql.GetField(key));
	}
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"replace into %s.cloud_info set id=0,sum_user=%d,sum_group=%d,sum_vm=%d,sum_vm_active=%d,sum_vm_stopped=%d,sum_vcpu=%d,sum_mem_mb=%d,sum_disk_gb=%d,sum_vcpu_used=%d,sum_mem_used=%d,check_at=NOW();",database,SumUser,SumGroup,SumVm,SumVmActive,SumVmStopped,SumVcpu,SumMemMb,SumDiskGb,SumVcpuUsed,SumMemUsed);
	msql.insert(cmd);
	

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
	cloud_scan();
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
