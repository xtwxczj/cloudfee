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
char* loginusername;

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

//计算用户的积分以及计算实验组的积分
int score()
{
	double SumScoreRecharge = 0;
	double SumScoreUsed = 0;
	
	
	char* vm_name = NULL;
	char* user_name = NULL;
	char* user_id = NULL;
	char* host_name = NULL;
	char* group_name = NULL;
	char* uuid = NULL;
	int vcpus = 0;
	int mem_mb = 0;
	int disk_gb = 0;
	char* create_at = NULL;
	char* delete_at = NULL;
	int hour = 0;
	int minute = 0;
	double value = 1;
	double time = 0;
	double score = 0;
	double temp = 0;
	double bonus = 0;
	double sum_bonus = 0;
	double host_sum_bonus = 0;
	double group_sum_bonus = 0;
	double group_sum_bonus_add = 0;
	

	
	//计算每个虚拟机消耗的积分
	memset(cmd,0,sizeof(cmd));
//	snprintf(cmd,sizeof(cmd),"select %s.vms.*,hour(timediff(%s.vms.delete_at,%s.vms.create_at)) as hour,minute(timediff(%s.vms.delete_at,%s.vms.create_at)) as minute,hour(timediff(NOW(),%s.vms.create_at)) as hour_t,minute(timediff(NOW(),%s.vms.create_at)) as minute_t ,%s.hypervisor.value from %s.vms inner join %s.hypervisor on %s.vms.host_name = %s.hypervisor.host_name where IsCharged = 0;",database,database,database,database,database,database,database,database,database,database,database,database);

	snprintf(cmd,sizeof(cmd),"select %s.vms.*,floor((UNIX_TIMESTAMP(%s.vms.delete_at)-UNIX_TIMESTAMP(%s.vms.create_at))/60/60) as hour,floor((UNIX_TIMESTAMP(%s.vms.delete_at)-UNIX_TIMESTAMP(%s.vms.create_at))/60)-floor((UNIX_TIMESTAMP(%s.vms.delete_at)-UNIX_TIMESTAMP(%s.vms.create_at))/60/60)*60 as minute,floor((UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(%s.vms.create_at))/60/60) as hour_t,floor((UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(%s.vms.create_at))/60)-floor((UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(%s.vms.create_at))/60/60)*60 as minute_t ,%s.hypervisor.value from %s.vms inner join %s.hypervisor on %s.vms.host_name = %s.hypervisor.host_name where IsCharged = 0;",database,database,database,database,database,database,database,database,database,database,database,database,database,database,database);
	msql.free_res();
	msql.query_select(cmd);
	int record_count = msql.getrecordcount();
	for(int i=0;i<record_count;i++)
	{
		if(!msql.FetchRow())
		{
			vcpus = 0;
			mem_mb = 0;
			disk_gb = 0;
			create_at = NULL;
			delete_at = NULL;
			hour = 0;
			minute = 0;
			uuid = NULL;
		}
		else
		{
			strcpy(key,"vm_name");
			vm_name = msql.GetField(key);
			strcpy(key,"vcpus");
			vcpus = atoi(msql.GetField(key));
			strcpy(key,"mem_mb");
			mem_mb = atoi(msql.GetField(key));
			strcpy(key,"disk_gb");
			disk_gb = atoi(msql.GetField(key));
			strcpy(key,"create_at");
			create_at = msql.GetField(key);
			strcpy(key,"delete_at");
			delete_at = msql.GetField(key);
			strcpy(key,"uuid");
			uuid = msql.GetField(key);
			string deletetime(delete_at);
			if(deletetime!="0000-00-00 00:00:00")//已经删除
			{
				strcpy(key,"hour");
				hour = atoi(msql.GetField(key));
				strcpy(key,"minute");
				minute = atoi(msql.GetField(key));
				
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd),"update %s.vms set IsCharged = 1 where vm_name = '%s' and uuid = '%s';",database,vm_name,uuid);
				msql1.insert(cmd);
			}
			else
			{
				strcpy(key,"hour_t");
				hour = atoi(msql.GetField(key))-8;
				strcpy(key,"minute_t");
				minute = atoi(msql.GetField(key));
			}
			
			strcpy(key,"value");
			value = atof(msql.GetField(key));
			
			time = (double)hour*60+minute;
			
			temp = (vcpus+mem_mb/1024.00/10.00)/60.00;//每分钟消耗的积分
			score = temp*time*value;
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"update %s.vms set bonus = %.2f ,check_at = NOW() where vm_name = '%s' and uuid = '%s';",database,score,vm_name,uuid);
			msql1.insert(cmd);
			
		}
	}
	
	
	//计算普通用户消耗的积分
	//const char filename[100];memset(filename,0,sizeof(filename);) = "/root/user.json";
	char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/user.json",json_dir.c_str());
	Json::Reader reader;// 解析json用Json::Reader   
    Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         

    std::ifstream is;  
    is.open (filename, std::ios::binary );
	if (reader.parse(is, root, 0))  
    {  
        int user_size = root["users"].size();  // 得到"files"的数组个数  
        for(int i = 0; i < user_size; ++i)  // 遍历数组  
        {  
			/****使用ldap时****/  
			/**
			std::string name = root["users"][i]["username"].asString();
			username = new char[name.length()+1];
			strcpy(username,name.c_str());
			**/
			
			 /****不使用ldap时****/  
			 std::string name = root["users"][i]["username"].asString();
			 std::string id = root["users"][i]["id"].asString();
			 std::string pro_id = root["users"][i]["tenantId"].asString();
			 user_name = new char[name.length()+1];
			 strcpy(user_name,name.c_str());
			 if(strcmp(user_name,loginusername)!=0)
				 continue;
			 user_id = new char[id.length()+1];
			 strcpy(user_id,id.c_str());
			 
			 memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"select sum(bonus) as sum_bonus from %s.vms where user_id = '%s';",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			if(!msql1.FetchRow())
			{
				sum_bonus = 0;
			}
			else
			{
				strcpy(key,"sum_bonus");
				sum_bonus = atof(msql1.GetField(key));	
			}
		
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"select * from %s.userinfo where user_name = '%s';",database,user_name);
			msql1.free_res();
			msql1.query_select(cmd);
			if(!msql1.getrecordcount())
			{
				continue;
			}
			else
			{
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd),"update %s.userinfo set SumScoreUsed = %.2f , check_at = NOW() where user_name = '%s' and id=0;",database,sum_bonus,user_name);
				msql1.insert(cmd);
			}
		}
	}		
	
	
	
	
	
	
	
	
	//计算实验组消耗的积分,也就是给实验组虚拟机做计算用的所有积分
	//同时算出group_vm_map中所有计算用虚拟机的积分
	//如果有vm_name重名问题导致积分计算出错，下列代码将修复这个bug
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select vm_name from %s.group_vm_map;",database);
	msql.free_res();
	msql.query_select(cmd);
	record_count = msql.getrecordcount();
	for(int i=0;i<record_count;i++)
	{
		if(!msql.FetchRow())
		{
			vm_name = NULL;
			break;
		}
		else
		{
			strcpy(key,"vm_name");
			vm_name = msql.GetField(key);	
		}
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"select * from %s.vms where vm_name='%s' and user_name='admin';",database,vm_name);
		msql1.free_res();
		msql1.query_select(cmd);
		if(msql1.getrecordcount()>1)//有重名情况
		{
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"select b.* from (select %s.vms.*, abs(TIMESTAMPDIFF(second, create_at,(select create_at from %s.group_vm_map where vm_name='%s'))) as diff from %s.vms) as b order by b.diff asc limit 1;",database,database,vm_name,database);
			msql2.free_res();
			msql2.query_select(cmd);
			if(!msql2.FetchRow())
			{
				//group_name = NULL;
				break;
			}
			else
			{
				strcpy(key,"bonus");
				bonus = atof(msql2.GetField(key));
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd),"update %s.group_vm_map set bonus = %f where vm_name='%s';",database,bonus,vm_name);
				msql2.insert(cmd);
			}
		}
		else //没有重名情况
		{
			
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"update %s.group_vm_map set %s.group_vm_map.bonus=(select %s.vms.bonus from %s.vms where %s.vms.vm_name='%s' and %s.vms.user_name='admin') where %s.group_vm_map.vm_name='%s';",database,database,database,database,database,vm_name,database,database,vm_name);
			msql2.insert(cmd);
		}
	}
	
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select distinct(group_name) from %s.group_vm_map;",database);
	msql.free_res();
	msql.query_select(cmd);
	record_count = msql.getrecordcount();
	for(int i=0;i<record_count;i++)
	{
		if(!msql.FetchRow())
		{
			group_name = NULL;
			break;
		}
		else
		{
			strcpy(key,"group_name");
			group_name = msql.GetField(key);
			
		}
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"select sum(bonus) as group_sum_bonus from %s.group_vm_map where group_name = '%s';",database,group_name);
		msql1.free_res();
		msql1.query_select(cmd);
		if(!msql1.FetchRow())
		{
			group_sum_bonus = 0;
		}
		else
		{
			strcpy(key,"group_sum_bonus");
			group_sum_bonus = atof(msql1.GetField(key));	
		}
		
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"update %s.group set %s.group.SumScoreUsed = %.2f ,%s.group.checked_at = NOW() where group_name = '%s';",database,database,group_sum_bonus,database,group_name);
		msql1.insert(cmd);
	}
	
	
	
	//计算每台宿主机增加的积分,该积分包含了用户的虚拟机给实验组宿主机增加的积分
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select distinct(host_name) from %s.hypervisor;",database);
	msql.free_res();
	msql.query_select(cmd);
	record_count = msql.getrecordcount();
	for(int i=0;i<record_count;i++)
	{
		if(!msql.FetchRow())
		{
			break;
		}
		else
		{
			strcpy(key,"host_name");
			host_name = msql.GetField(key);
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"select sum(bonus) as host_sum_bonus from %s.vms where host_name = '%s';",database,host_name);
			msql1.free_res();
			msql1.query_select(cmd);
			if(!msql1.FetchRow())
			{
				host_sum_bonus = 0;
			}
			else
			{
				strcpy(key,"host_sum_bonus");
				host_sum_bonus = atof(msql1.GetField(key));
			}
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"update %s.hypervisor set bonus = %.2f,check_at = NOW() where host_name = '%s';",database,host_sum_bonus,host_name);
			msql2.insert(cmd);
		}
	}
	
	//计算各个实验组增加的积分,也就是实验组物理机增加的积分
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select distinct(group_name) from %s.group_vm_map;",database);
	msql.free_res();
	msql.query_select(cmd);
	record_count = msql.getrecordcount();
	for(int i=0;i<record_count;i++)
	{
		if(!msql.FetchRow())
		{
			group_name = NULL;
			break;
		}
		else
		{
			strcpy(key,"group_name");
			group_name = msql.GetField(key);
			
		}
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"select sum(bonus) as group_sum_bonus_add from %s.hypervisor where _group = '%s';",database,group_name);
		msql1.free_res();
		msql1.query_select(cmd);
		if(!msql1.FetchRow())
		{
			group_sum_bonus_add = 0;
		}
		else
		{
			strcpy(key,"group_sum_bonus_add");
			string null = "NULL";
			if(strcmp(msql1.GetField(key),null.c_str()))
				group_sum_bonus_add = 0;
			group_sum_bonus_add = atof(msql1.GetField(key));	
		}
		
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"update %s.group set %s.group.SumScoreRecharge = %.2f, %s.group.checked_at = NOW() where group_name = '%s';",database,database,group_sum_bonus_add,database,group_name);
		msql1.insert(cmd);
	}
	
	//计算实验组剩余的积分
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"update %s.group set SumScore=ScoreRepaid+SumScoreRecharge-SumScoreUsed;",database);
	msql.insert(cmd);
	
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
	var = "LOGIN_USERNAME";
	loginusername = getenv(var);


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
	score();
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
