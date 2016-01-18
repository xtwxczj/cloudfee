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

char* quota_scan(char* project_id,char* key)
{
	string str_tenant_id = string(project_id);
	char* temp = NULL;
	if(string(key)=="instances")
	{
		string instances_quota = tenant_instance_quota[str_tenant_id];
		temp = new char[instances_quota.length()+1];
		strcpy(temp,instances_quota.c_str());
		return temp;
	}
	else if(string(key)=="cores")
	{
		string cores_quota = tenant_core_quota[str_tenant_id];
		temp = new char[cores_quota.length()+1];
		strcpy(temp,cores_quota.c_str());
		return temp;
	}
	else if(string(key)=="ram")
	{
		string ram_quota = tenant_ram_quota[str_tenant_id];
		temp = new char[ram_quota.length()+1];
		strcpy(temp,ram_quota.c_str());
		return temp;
	}

}



int user_history_scan()
{
	char* username = NULL;
	char* user_id = NULL;
	char* project_id = NULL;
	double SumScoreRecharge = 0;
	double SumScoreUsed = 0;
	int instances_all =0;
	int instances_used =0;
	int instances_active =0;
	int instances_shutdown =0;
	int instances_error =0;
	int instances_paused =0;
	int instances_building =0;
	int instances_deleted =0;
	int cores_all =0;
	int cores_used =0;
	int ram_all = 0;
	int ram_used = 0;
	
	//char filename[100];memset(filename,0,sizeof(filename);) = "/root/user.json";
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
			username = new char[name.length()+1];
			strcpy(username,name.c_str());
			user_id = new char[id.length()+1];
			strcpy(user_id,id.c_str());
			project_id = new char[pro_id.length()+1];
			strcpy(project_id,pro_id.c_str());
			
			if((name=="nova")||(name=="glance")||(name=="quantum")||(name=="cinder")||(name=="neutron")||(name=="ceilometer")||(name=="glance"))
				continue;
			
			memset(cmd,0,sizeof(cmd));
			//snprintf(cmd,sizeof(cmd),"select sum(ScoreUsed) as SumScoreUsed , sum(ScoreRecharge) as SumScoreRecharge from %s.user_score where user_name = '%s';",username);
			snprintf(cmd,sizeof(cmd),"select %s.userinfo.SumScoreUsed as SumScoreUsed ,%s.userinfo.SumScoreRecharge as SumScoreRecharge from %s.userinfo where user_name = '%s' and id=0;",database,database,database,username);
			msql1.free_res();
			msql1.query_select(cmd);
		
			if(!msql1.FetchRow()||!msql1.getrecordcount())
			{
				SumScoreUsed = 0;
				SumScoreRecharge =0;
			}
			else{
				strcpy(key,"SumScoreUsed");
				SumScoreUsed = atof(msql1.GetField(key));
				strcpy(key,"SumScoreRecharge");
				SumScoreRecharge = atof(msql1.GetField(key));
			}
			
			/****使用ldap时****/ 
			/**
			//这里project_id存的是username
			strcpy(key,"instances";
			instances_all = atoi(quota_scan(username,key));
			strcpy(key,"cores";
			cores_all = atoi(quota_scan(username,key));
			strcpy(key,"ram";
			ram_all = atoi(quota_scan(username,key));
			**/
			
			/****不使用ldap时****/
			strcpy(key,"instances");
			instances_all = atoi(quota_scan(username,key));
			strcpy(key,"cores");
			cores_all = atoi(quota_scan(username,key));
			strcpy(key,"ram");
			ram_all = atoi(quota_scan(username,key));
			
			
			
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select count(display_name) as used_count from instances where user_id = '%s' and (vm_state = 'active' or vm_state = 'stopped');",username);
			/****不使用ldap时****/
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as used_count from %s.vms where user_id = '%s' and (vm_state = 'active' or vm_state = 'stopped');",database,user_id);
			
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"used_count");
			if(!msql1.FetchRow())
			{
				instances_used = 0;
			}
			else
			{
				instances_used = atoi(msql1.GetField(key));
			}
			
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select count(display_name) as active_count from instances where user_id = '%s' and vm_state = 'active';",username);
			/****不使用ldap时****/
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as active_count from %s.vms where user_id = '%s' and vm_state = 'active';",database,user_id);
			
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"active_count");
			if(!msql1.FetchRow())
			{
				instances_active = 0;
			}
			else
			{
				instances_active = atoi(msql1.GetField(key));
			}
		
		
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select count(display_name) as shutdown_count from instances where user_id = '%s' and  vm_state = 'stopped';",username);
			/****不使用ldap时****/ 
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as shutdown_count from %s.vms where user_id = '%s' and  vm_state = 'stopped';",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"shutdown_count");
		
			if(!msql1.FetchRow())
			{
				instances_shutdown = 0;
			}
			else
			{
				instances_shutdown = atoi(msql1.GetField(key));
			}
			
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select count(display_name) as shutdown_count from instances where user_id = '%s' and  vm_state = 'error';",username);
			/****不使用ldap时****/ 
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as error_count from %s.vms where user_id = '%s' and  vm_state = 'error';",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"error_count");
		
			if(!msql1.FetchRow())
			{
				instances_error = 0;
			}
			else
			{
				instances_error = atoi(msql1.GetField(key));
			}
			
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select count(display_name) as shutdown_count from instances where user_id = '%s' and  vm_state = 'deleted';",username);
			/****不使用ldap时****/ 
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as deleted_count from %s.vms where user_id = '%s' and  vm_state = 'deleted';",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"deleted_count");
		
			if(!msql1.FetchRow())
			{
				instances_deleted = 0;
			}
			else
			{
				instances_deleted = atoi(msql1.GetField(key));
			}
			
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select count(display_name) as shutdown_count from instances where user_id = '%s' and  vm_state = 'building';",username);
			/****不使用ldap时****/ 
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as building_count from %s.vms where user_id = '%s' and  vm_state = 'building';",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"building_count");
		
			if(!msql1.FetchRow())
			{
				instances_building = 0;
			}
			else
			{
				instances_building = atoi(msql1.GetField(key));
			}
			
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select count(display_name) as shutdown_count from instances where user_id = '%s' and  vm_state = 'paused';",username);
			/****不使用ldap时****/ 
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as paused_count from %s.vms where user_id = '%s' and  vm_state = 'paused';",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"paused_count");
		
			if(!msql1.FetchRow())
			{
				instances_paused = 0;
			}
			else
			{
				instances_paused = atoi(msql1.GetField(key));
			}
			
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select sum(vcpus) as sum_vcpus from instances where user_id = '%s' and (vm_state = 'active' or vm_state = 'stopped');",username);
			/****不使用ldap时****/
			snprintf(cmd,sizeof(cmd),"select sum(vcpus) as sum_vcpus from %s.vms where user_id = '%s' and (vm_state = 'active' or vm_state = 'stopped');",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"sum_vcpus");
			if(!msql1.FetchRow())
			{
				cores_used = 0;
			}
			else
			{
				cores_used = atoi(msql1.GetField(key));
			}
		
		
			memset(cmd,0,sizeof(cmd));
			/****使用ldap时****/ 
			//snprintf(cmd,sizeof(cmd),"select sum(memory_mb) as sum_memory from instances where user_id = '%s' and (vm_state = 'active' or vm_state = 'stopped');",username);
			/****不使用ldap时****/ 
			snprintf(cmd,sizeof(cmd),"select sum(mem_mb) as sum_memory from %s.vms where user_id = '%s' and (vm_state = 'active' or vm_state = 'stopped');",database,user_id);
			msql1.free_res();
			msql1.query_select(cmd);
			strcpy(key,"sum_memory");
			if(!msql1.FetchRow())
			{
				ram_used = 0;
			}
			else
			{
				ram_used = atoi(msql1.GetField(key));
			}
			
	
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"insert into %s.user_history_info set id = 0,user_name='%s',group_name = (select group_name from %s.userinfo where id =0 and user_name = '%s'),ScoreUsed=%.2f,instances_all=%d,instances_used=%d,instances_active=%d,instances_shutdown=%d,cores_all=%d,cores_used=%d,ram_all=%d,ram_used=%d,check_at=NOW();",database,username,database,username,SumScoreUsed,instances_all,instances_used,instances_active,instances_shutdown,cores_all,cores_used,ram_all,ram_used);
			msql2.insert(cmd);

		}
	
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

	string str_username;
	string str_user_id;
	//对user_id和user_name生成map
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -s -H \"X-Auth-Token:%s\" http://%s:35357/v2.0/users | python -mjson.tool > %s/user.json",token.c_str(),keystoneAddr,json_dir.c_str());
	system(cmd);


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
		for(int i = 0;i < tenant_size;i++)
		{
			str_tenant_id = root["tenants"][i]["id"].asString();
			str_tenant_name = root["tenants"][i]["name"].asString();
			//由tenant_id去找quota_limit
	/*		memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/os-quota-sets/%s |python -mjson.tool >%s/tenant_quota/%s_quota.json",token.c_str(),novaAddr,tenant_id.c_str(),str_tenant_id.c_str(),json_dir.c_str(),str_tenant_id.c_str());
			system(cmd);
	*/	
			sprintf(filename,"%s/tenant_quota/%s_quota.json",json_dir.c_str(),str_tenant_id.c_str());
			is2.open(filename,std::ios::binary);
			if(reader.parse(is2,root2,0))
			{
				str_cores = root2["quota_set"]["cores"].asString().c_str();
				str_instances = root2["quota_set"]["instances"].asString();
				str_ram = root2["quota_set"]["ram"].asString();
				tenant_instance_quota[str_tenant_id] = str_instances;
				tenant_core_quota[str_tenant_id] = str_cores;
				tenant_ram_quota[str_tenant_id] = str_ram;
			}
			is2.close();

		}


	}
	is.close();

	user_history_scan();


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
