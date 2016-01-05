#include "../header/main.h"
#include <time.h>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include <json/json.h> 
#include <map>



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

/*
扫描用户，并进一步计算用户的积分
*/

int user_scan()
{
	//memset(cmd,0,sizeof(cmd));
	//snprintf(cmd,sizeof(cmd),"%s","select keystone.user.name as username , keystone.user.id as user_id ,default_project_id as project_id , keystone.project.name  as project_name from keystone.user inner join keystone.project where keystone.user.default_project_id = keystone.project.id;");
	//msql.free_res();
	//msql.query_select(cmd);
	//int record_count = msql.getrecordcount();
	char* username = NULL;
	char* user_id = NULL;
	char* project_id = NULL;
	double SumScoreRecharge = 0;
	double SumScoreUsed = 0;
	double vcpu_hour = 0;
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
			/****Watch out differences between openstack icehouse and kilo!****/ 
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
			snprintf(cmd,sizeof(cmd),"select %s.userinfo.SumScoreUsed as SumScoreUsed ,%s.userinfo.SumScoreRecharge as SumScoreRecharge from %s.userinfo where user_name = '%s';",database,database,database,username);
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
			snprintf(cmd,sizeof(cmd),"select * from %s.userinfo where id=0 and user_name='%s';",database,username);
			msql1.free_res();
			msql1.query_select(cmd);
			if(!msql1.getrecordcount())
			{
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd),"insert into %s.userinfo set id=0,user_name='%s',group_name='null',SumScoreUsed=%.2f,instances_all=%d,instances_used=%d,instances_active=%d,instances_shutdown=%d,cores_all=%d,cores_used=%d,ram_all=%d,ram_used=%d,check_at=NOW();",database,username,SumScoreUsed,instances_all,instances_used,instances_active,instances_shutdown,cores_all,cores_used,ram_all,ram_used);
				msql2.insert(cmd);
			}
			else
			{
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd),"update %s.userinfo set instances_all=%d,instances_used=%d,instances_active=%d,instances_shutdown=%d,cores_all=%d,cores_used=%d,ram_all=%d,ram_used=%d,check_at=NOW() where id=0 and user_name='%s';",database,instances_all,instances_used,instances_active,instances_shutdown,cores_all,cores_used,ram_all,ram_used,username);
				msql2.insert(cmd);
				
			}
		}
	}
    std::ifstream is2;	
    Json::Reader reader2;// 解析json用Json::Reader   
    Json::Value root2; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array    
    snprintf(filename,sizeof(filename),"%s/server_diagnostics.json",json_dir.c_str());
    is2.open(filename, std::ios::binary );  
    if (reader.parse(is2, root2, 0))  
    {
	    int tenant_size = root2["tenant_usages"].size();  // 得到"files"的数组个数  
	    for(int i = 0; i < tenant_size; ++i)  // 遍历数组  
	    {  
		    std::string pro_id = root2["tenant_usages"][i]["tenant_id"].asString();
		    project_id = new char[pro_id.length()+1];
		    strcpy(project_id,pro_id.c_str());
		    std::string str_cpu_hour = root2["tenant_usages"][i]["total_vcpus_usage"].asString();
		    vcpu_hour = atof(str_cpu_hour.c_str());	

		    memset(cmd,0,sizeof(cmd));
		    snprintf(cmd,sizeof(cmd),"select * from %s.userinfo where id=0 and user_name='%s';",database,project_id);
		    msql1.free_res();
		    msql1.query_select(cmd);
		    if(!msql1.getrecordcount())
		    {
		    }
		    else
		    {
			    memset(cmd,0,sizeof(cmd));
			    snprintf(cmd,sizeof(cmd),"update %s.userinfo set vcpu_hour = %f where id=0 and user_name='%s';",database,vcpu_hour,project_id);    
			    msql2.insert(cmd);
		    }
	    }
    }
    is2.close();	

    return 1;
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




//扫描云平台的信息，将结果呈现给云平台的超级管理员
//先运行project_scan之后，再运行这个cloud_scan()

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





//計算用戶用了多少磁盘


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




int cloud_vm_scan()
{
	//const char filename[100];memset(filename,0,sizeof(filename);) = "/root/vm.json";
	char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/vm.json",json_dir.c_str());
	char* vm_domain = "null";			//虚拟机域名
	char* vm_name = "null";			//虚拟机名称
	char* group_name = "null";
	char* instance_uuid = "null";
	char* tenant_id = "null";
	int vm_batch=0;

	char* cpu_info = "null";		//宿主机cpu信息
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


	char* vm_username = "null";		//虚拟机创建的用户名称
	char* vm_user_id = "null";			//用户id
	char* vm_ip = "null";				//虚拟机IP
	char* vm_mac = "null";			//虚拟机MAC
        char* cell_name = "null"; 
	char* availability_zone = "null";     //
	char* project_id = "null";        //
	char* vm_state = "null";			//虚拟机状态   active=1  or pause or showdown ...
	int	 vcpus = 0;					//虚拟机占用的虚核数量
	int	 memory_mb = 0;				//虚拟机占用的内存数量
	int disk_gb = 0;
	int flavor = 0;
	char* launched_on = "null";		//虚拟机创建在哪个宿主机
	char* created_at = "null";		//虚拟机创建的时间
	char* deleted_at = "null";
		//虚拟机删除的时间
	int flag = 0;					//和shut_at配合 1表示开启，0表示关闭, -1表示刚创建
	char* start_at = "null";
	char* shut_at = "null";
	char* checked_at = "null";

	char* image_name = "null";		//虚拟机使用的镜像的名称
	char* image_id = "null";			//虚拟机镜像的id
//	char flavor_name[64];		//资源匹配的名称
	char* flavor_id = "null";			//资源匹配的类型id

	double vcpu_util = 0;
	double vcpu_hour = 0;
	double cpu = 0;					//CPU time used
	//double disk_device_read_requests = 0;	//Number of read requests
	//double disk_device_read_requests_rate = 0;//Average rate of read requests
	//double disk_device_write_requests = 0;	//Number of write requests
	//double disk_device_write_requests_rate = 0;//Average rate of write requests
	//double disk_device_read_bytes = 0;		//Volume of reads
	//double disk_device_read_bytes_rate = 0;//Average rate of reads
	//double disk_device_write_bytes = 0;		//Volume of writes
	//double disk_device_write_bytes_rate = 0;//Average rate of writes
	//double network_incoming_bytes = 0;//Number of incoming bytes
	//double network_incoming_bytes_rate = 0;//Average rate of incoming bytes
	//double network_outgoing_bytes = 0;//Number of outgoing bytes
	//double network_outgoing_bytes_rate = 0;//Average rate of outgoing bytes
	//double network_incoming_packets = 0;//Number of incoming packets
	//double network_incoming_packets_rate = 0;//Average rate of incoming packets
	//double network_outgoing_packets = 0;//Number of outgoing packets
	//double network_outgoing_packets_rate = 0;//Average rate of outgoing packets
	

	
	//double SumScoreUsed = 0;
	//double SumScoreRecharge = 0;
	//int instances_all = 0;


	std::string str_mac;
	
	Json::Reader reader;// 解析json用Json::Reader   
    Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array    
	
	std::ifstream is;  
    is.open (filename, std::ios::binary );    
	if (reader.parse(is, root, 0))  
    {  
        int server_size = root["servers"].size();  //   
        for(int i = 0; i < server_size; ++i)  // 遍历数组  
        {
			
			std::string av_zone = root["servers"][i]["OS-EXT-AZ:availability_zone"].asString();
			availability_zone = new char[av_zone.length()+1];
			strcpy(availability_zone,av_zone.c_str());
			
			std::string hypervisor_hostname = root["servers"][i]["OS-EXT-SRV-ATTR:hypervisor_hostname"].asString();
			host_domain = new char[hypervisor_hostname.length()+1];
			strcpy(host_domain,hypervisor_hostname.c_str());
			
			std::string str_vm_state = root["servers"][i]["OS-EXT-STS:vm_state"].asString();
			vm_state = new char[str_vm_state.length()+1];
			strcpy(vm_state,str_vm_state.c_str());
			
			if(str_vm_state!="error")
			{
					
			std::string launched_at = root["servers"][i]["OS-SRV-USG:launched_at"].asString();
			created_at = new char[launched_at.length()+1];
			strcpy(created_at,launched_at.c_str());
			
			std::string terminated_at = root["servers"][i]["OS-SRV-USG:terminated_at"].asString();
			deleted_at = new char[terminated_at.length()+1];
			strcpy(deleted_at,terminated_at.c_str());
			
			std::vector<string> vec = root["servers"][i]["addresses"].getMemberNames();
			vector<string>::iterator it = vec.begin();
			//cout<<it<<endl;
			cout<<*it<<endl;
			cout<<root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"]<<endl;

			cout<<root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().length()<<endl;
			
		//	str_mac = root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().c_str();
			vm_mac = new char[root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().length()+1];
			strcpy(vm_mac,root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().c_str());
	
			std::string str_ip = root["servers"][i]["addresses"][*it][0]["addr"].asString();
			vm_ip = new char[str_ip.length()+1];
			strcpy(vm_ip,str_ip.c_str());
		cout<<"haha"<<endl;		
			}
			else
			{
				created_at = "null";
				deleted_at = "null";
				vm_mac = "null";
				vm_ip = "null";
				
			}
		
			std::string str_vm_name = root["servers"][i]["name"].asString();
			vm_name = new char[str_vm_name.length()+1];
			strcpy(vm_name,str_vm_name.c_str());
			
			std::string user_id = root["servers"][i]["user_id"].asString();
			vm_user_id = new char[user_id.length()+1];
			strcpy(vm_user_id,user_id.c_str());
			
			std::string flavor_id = root["servers"][i]["flavor"]["id"].asString();
			std::string image_id = root["servers"][i]["image"]["id"].asString();
			
			std::string str_uuid = root["servers"][i]["id"].asString();
			instance_uuid = new char[str_uuid.length()+1];
			strcpy(instance_uuid,str_uuid.c_str());
			
			std::string str_tenant_id = root["servers"][i]["tenant_id"].asString();
			tenant_id = new char[str_tenant_id.length()+1];
			strcpy(tenant_id,str_tenant_id.c_str());
			
			//memset(cmd,0,sizeof(cmd));
			//snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/servers/%s/diagnostics |python -mjson.tool > %s/server_diagnostics.json",token.c_str(),novaAddr,tenant_id,instance_uuid,json_dir.c_str());
			//system(cmd);
			std::string str_user_name = user_map[user_id];
			vm_username = new char[str_user_name.length()+1];
			strcpy(vm_username,str_user_name.c_str());
			cout<<vm_username<<endl;

			std::string str_vm_vcpu = flavor_vcpu_map[flavor_id];
			vcpus = atoi(str_vm_vcpu.c_str());
			
			std::string str_vm_ram = flavor_ram_map[flavor_id];
			memory_mb = atoi(str_vm_ram.c_str());
			
			std::string str_vm_disk = flavor_disk_map[flavor_id];
			disk_gb = atoi(str_vm_disk.c_str());
			
			std::string str_image_name = image_map[image_id];
			image_name = new char[str_image_name.length()+1];
			strcpy(image_name,str_image_name.c_str());
			
		cout<<"haha"<<endl;	
					
			
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd,sizeof(cmd), "select * from %s.vms where vm_name = '%s' and user_name = '%s' and user_id='%s' and uuid = '%s';", database,vm_name,vm_username,vm_user_id,instance_uuid);
				msql1.free_res();
				cout<<cmd<<endl;
				msql1.query_select(cmd);

				
				if (msql1.getrecordcount() ==0 )
				{
					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd,sizeof(cmd), "insert into %s.vms set id=0,vm_name='%s',vm_ip='%s',vm_mac='%s',uuid='%s',batch=%d,create_at='%s',delete_at='%s',av_zone='%s',image_name='%s',vcpus=%d,mem_mb=%d,disk_gb=%d,user_name='%s',user_id='%s',vcpu_utility=%.2f,cpu_hour=%.2f,host_name='%s',vm_state='%s',check_at=NOW();",
					database,vm_name, vm_ip,vm_mac,instance_uuid,vm_batch,created_at, deleted_at,availability_zone,image_name, vcpus, memory_mb, disk_gb, vm_username, vm_user_id,vcpu_util,vcpu_hour,host_domain,vm_state);
					msql2.insert(cmd);
					
				}
		 
				else
				{
					
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd,sizeof(cmd),"update %s.vms set vm_ip='%s',vm_mac='%s',create_at='%s', delete_at='%s',check_at=NOW(),av_zone='%s',vcpus=%d,mem_mb=%d,disk_gb=%d,user_name='%s',vcpu_utility=%.2f,cpu_hour=%.2f,host_name='%s', image_name='%s',vm_state='%s' where vm_name='%s' and uuid='%s';"
				,database,vm_ip,vm_mac,created_at,deleted_at,availability_zone,vcpus,memory_mb,disk_gb,vm_username,vcpu_util,vcpu_hour,host_domain,image_name,vm_state,vm_name,instance_uuid);
				msql2.insert(cmd);
				}


		}
		
	}
	is.close();
	
	
	
	//宿主机hypervisor监控
	std::string str_hypervisor_hostname;
	std::string str_host_ip;
	std::string str_cpu_info;
	std::string str_free_disk_gb;
	std::string str_disk_gb_used;
	std::string str_free_ram_mb;
	std::string str_local_gb;
	std::string str_memory_mb;
	std::string str_memory_mb_used;
	std::string str_memory_mb_free;
	std::string str_running_vms;
	std::string str_vcpus;
	std::string str_vcpus_used;
	std::string str_vcpus_free;
	
	//获取token
	/*
	string token;
	string tenant_id;
	system("curl -s -X POST http://192.168.81.38:5000/v2.0/tokens -H \"Content-Type: application/json\" -d '{\"auth\": {\"tenantName\": \"'\"admin\"'\", \"passwordCredentials\":{\"username\": \"'\"admin\"'\", \"password\": \"'\"opstk123\"'\"}}}' > /root/token.json");
	filename = "/root/token.json";
	is.open(filename, std::ios::binary );  
	if (reader.parse(is, root, 0))  
    {
		token = root["access"]["token"]["id"].asString();
		tenant_id = root["access"]["token"]["tenant"]["id"].asString();
	}
	is.close();
	*/
	

	//获取所有的hypervisor信息
	std::ifstream is2;	
	Json::Reader reader2;// 解析json用Json::Reader   
	Json::Value root2; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array    
			
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/os-hosts |python -mjson.tool > %s/host.json",token.c_str(),novaAddr,tenant_id,json_dir.c_str());
	system(cmd);
	//filename = "/root/hypervisor.json";
	sprintf(filename,"%s/host.json",json_dir.c_str());
	is.open(filename, std::ios::binary );  
	if (reader.parse(is, root, 0))  
    {
		int host_size = root["hosts"].size(); 
		for(int i = 0; i < host_size; ++i)  // 遍历数组  
		{
			 std::string host_name = root["hosts"][i]["host_name"].asString();
			 std::string service = root["hosts"][i]["service"].asString();
			 if(service!="compute")
			 {
				 continue;
			 }
			 
			 strcpy(host_domain,host_name.c_str());
			 memset(cmd,0,sizeof(cmd));
			 snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/os-hosts/%s |python -mjson.tool > %s/%s_detail.json",token.c_str(),novaAddr,tenant_id,host_name.c_str(),json_dir.c_str(),host_name.c_str());
			 system(cmd);
			// filename = "/root/hypervisor_detail.json";
			// char filename[100];memset(filename,0,sizeof(filename));
			sprintf(filename,"%s/%s_detail.json",json_dir.c_str(),host_name.c_str());
			 is2.open(filename, std::ios::binary );  
			 if (reader.parse(is2, root2, 0))  
			 {
				 int size = root2["host"].size();
				host_vcpus = 0;
				host_disk_gb = 0;
				host_memory_mb = 0;
				host_free_vcpus = 0;
				host_free_ram_mb = 0;
				 for(int i = 0;i<size;++i)
				 {
					 std::string project = root2["host"][i]["resource"]["project"].asString();
					 if(project=="(total)")
					 {
						 str_vcpus = root2["host"][i]["resource"]["cpu"].asString();
						 host_vcpus = atoi(str_vcpus.c_str());
						 
						 str_local_gb = root2["host"][i]["resource"]["disk_gb"].asString();
						host_disk_gb = atoi(str_local_gb.c_str());
						
						str_memory_mb = root2["host"][i]["resource"]["memory_mb"].asString();
						host_memory_mb = atoi(str_memory_mb.c_str());
					 }
					 else if(project=="(used_now)")
					 {
						str_vcpus_used = root2["host"][i]["resource"]["cpu"].asString();
						host_free_vcpus = host_vcpus-atoi(str_vcpus_used.c_str());
						
						str_memory_mb_used = root2["host"][i]["resource"]["memory_mb"].asString();
						host_free_ram_mb = host_memory_mb-atoi(str_memory_mb_used.c_str());
						
						str_disk_gb_used = root2["host"][i]["resource"]["disk_gb"].asString();
						host_free_disk_gb = host_disk_gb-atoi(str_disk_gb_used.c_str()); 
					 }
					 
				 }
				 /*
				str_cpu_info = root2["hypervisor"]["cpu_info"].asString();
				cpu_info = new char[str_cpu_info.length()+1];
				strcpy(cpu_info,str_cpu_info.c_str());
				
				str_hypervisor_hostname = root2["hypervisor"]["hypervisor_hostname"].asString();
				host_domain = new char[str_hypervisor_hostname.length()+1];
				strcpy(host_domain,str_hypervisor_hostname.c_str());
				
				
				str_host_ip = root2["hypervisor"]["host_ip"].asString();
				host_ip = new char[str_host_ip.length()+1];
				strcpy(host_ip,str_host_ip.c_str());
				
				str_free_disk_gb = root2["hypervisor"]["free_disk_gb"].asString();
				
				host_free_disk_gb = atoi(str_free_disk_gb.c_str());
				
				str_free_ram_mb = root2["hypervisor"]["free_ram_mb"].asString();
				host_free_ram_mb = atoi(str_free_ram_mb.c_str());
				
				str_local_gb = root2["hypervisor"]["local_gb"].asString();
				host_disk_gb = atoi(str_local_gb.c_str());
				
				str_memory_mb = root2["hypervisor"]["memory_mb"].asString();
				host_memory_mb = atoi(str_memory_mb.c_str());
				
				str_memory_mb_used = root2["hypervisor"]["memory_mb_used"].asString();
				
				str_running_vms = root2["hypervisor"]["running_vms"].asString();
				running_vms = atoi(str_running_vms.c_str());
				
				str_vcpus = root2["hypervisor"]["vcpus"].asString();
				host_vcpus = atoi(str_vcpus.c_str());
				
				str_vcpus_used = root2["hypervisor"]["vcpus_used"].asString();
				host_free_vcpus = host_vcpus-atoi(str_vcpus_used.c_str());
				*/
				
				
			 }
			is2.close();
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"select count(vm_name) as vm_count from %s.vms where host_name='%s' and vm_state='active';",database,host_domain);
			msql1.free_res();
			msql1.query_select(cmd);
			if(!msql1.FetchRow())
                         {
                                 running_vms = 0;
                         }
                         else
                         {
                                 strcpy(key,"vm_count");
                                 running_vms=atoi(msql1.GetField(key));
                         }
 
			 	memset(cmd, 0, sizeof(cmd));
				snprintf(cmd,sizeof(cmd), "select * from %s.hypervisor where host_name = '%s';", database,host_domain);
				
				msql1.free_res();
				msql1.query_select(cmd);
				//int record_count = msql1.getrecordcount();

				if (msql1.getrecordcount() == 0)
				{
					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd,sizeof(cmd), "insert into %s.hypervisor set id=0,host_name='%s',host_ip='%s',vcpu=%d,mem_mb=%d,disk_gb=%d,create_at='%s',delete_at='%s',av_zone='%s',vcpus_free=%d,mem_mb_free=%d,disk_gb_free=%d,running_vms=%d,check_at=NOW();",
						database,host_domain, host_ip, host_vcpus, host_memory_mb, host_disk_gb, host_created_at, host_deleted_at, availability_zone, host_free_vcpus, host_free_ram_mb, host_free_disk_gb, running_vms);
					msql1.insert(cmd);
				}

				else
				{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd,sizeof(cmd), "update %s.hypervisor set host_ip='%s',vcpu=%d, mem_mb=%d,disk_gb=%d,create_at='%s',delete_at='%s',cells_id=%d,av_zone='%s',vcpus_free=%d,mem_mb_free=%d,disk_gb_free=%d,running_vms=%d,check_at=NOW() where host_name='%s';",
					database,host_ip, host_vcpus, host_memory_mb, host_disk_gb,host_created_at, host_deleted_at, 0,availability_zone , host_free_vcpus, host_free_ram_mb, host_free_disk_gb,running_vms, host_domain);
				msql1.insert(cmd);
				}

		}
	}
	return 1;
}




int vm_history_scan()
{
	//const char filename[100];memset(filename,0,sizeof(filename);) = "/root/vm.json";
	char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/vm.json",json_dir.c_str());
	char* vm_name = "null";			//虚拟机名称
	char* group_name = "null";
	char* instance_uuid = "null";
	int vm_batch=0;

	
	char* host_domain = "null";		//宿主机域名


	char* vm_username = "null";		//虚拟机创建的用户名称
	char* vm_user_id = "null";			//用户id
	char* vm_ip = "null";				//虚拟机IP
	char* vm_mac = "null";			//虚拟机MAC
        char* cell_name = "null"; 
	char* availability_zone = "null";     //
	char* vm_state = "null";			//虚拟机状态   active=1  or pause or showdown ...
	int	 vcpus = 0;					//虚拟机占用的虚核数量
	int	 memory_mb = 0;				//虚拟机占用的内存数量
	int disk_gb = 0;
	char* created_at = "null";		//虚拟机创建的时间
	char* deleted_at = "null";
		//虚拟机删除的时间

	char* image_name = "null";		//虚拟机使用的镜像的名称
//	char flavor_name[64];		//资源匹配的名称
	double bonus;

	double vcpu_util = 0;
	double mem_util = 0;
	double disk_util = 0;
	
	//double disk_device_read_requests = 0;	//Number of read requests
	//double disk_device_read_requests_rate = 0;//Average rate of read requests
	//double disk_device_write_requests = 0;	//Number of write requests
	//double disk_device_write_requests_rate = 0;//Average rate of write requests
	//double disk_device_read_bytes = 0;		//Volume of reads
	//double disk_device_read_bytes_rate = 0;//Average rate of reads
	//double disk_device_write_bytes = 0;		//Volume of writes
	//double disk_device_write_bytes_rate = 0;//Average rate of writes
	//double network_incoming_bytes = 0;//Number of incoming bytes
	//double network_incoming_bytes_rate = 0;//Average rate of incoming bytes
	//double network_outgoing_bytes = 0;//Number of outgoing bytes
	//double network_outgoing_bytes_rate = 0;//Average rate of outgoing bytes
	//double network_incoming_packets = 0;//Number of incoming packets
	//double network_incoming_packets_rate = 0;//Average rate of incoming packets
	//double network_outgoing_packets = 0;//Number of outgoing packets
	//double network_outgoing_packets_rate = 0;//Average rate of outgoing packets
	

	
	double SumScoreUsed = 0;
	double SumScoreRecharge = 0;

	
	Json::Reader reader;// 解析json用Json::Reader   
    Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array    
	
	std::ifstream is;  
    is.open (filename, std::ios::binary );    
	if (reader.parse(is, root, 0))  
    {  
        int server_size = root["servers"].size();  //   
        for(int i = 0; i < server_size; ++i)  // 遍历数组  
        {
			std::string av_zone = root["servers"][i]["OS-EXT-AZ:availability_zone"].asString();
			availability_zone = new char[av_zone.length()+1];
			strcpy(availability_zone,av_zone.c_str());
			
			std::string hypervisor_hostname = root["servers"][i]["OS-EXT-SRV-ATTR:hypervisor_hostname"].asString();
			host_domain = new char[hypervisor_hostname.length()+1];
			strcpy(host_domain,hypervisor_hostname.c_str());
			
			std::string str_vm_state = root["servers"][i]["OS-EXT-STS:vm_state"].asString();
			vm_state = new char[str_vm_state.length()+1];
			strcpy(vm_state,str_vm_state.c_str());

			if(str_vm_state!="error")
			{
					
			std::string launched_at = root["servers"][i]["OS-SRV-USG:launched_at"].asString();
			created_at = new char[launched_at.length()+1];
			strcpy(created_at,launched_at.c_str());
			
			std::string terminated_at = root["servers"][i]["OS-SRV-USG:terminated_at"].asString();
			deleted_at = new char[terminated_at.length()+1];
			strcpy(deleted_at,terminated_at.c_str());
			
			std::vector<string> vec = root["servers"][i]["addresses"].getMemberNames();
			vector<string>::iterator it = vec.begin();
			//cout<<it<<endl;
			cout<<*it<<endl;
			cout<<root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"]<<endl;

			cout<<root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().length()<<endl;
			
		//	str_mac = root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().c_str();
			vm_mac = new char[root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().length()+1];
			strcpy(vm_mac,root["servers"][i]["addresses"][*it][0]["OS-EXT-IPS-MAC:mac_addr"].asString().c_str());
	
			std::string str_ip = root["servers"][i]["addresses"][*it][0]["addr"].asString();
			vm_ip = new char[str_ip.length()+1];
			strcpy(vm_ip,str_ip.c_str());
		cout<<"haha"<<endl;		
			}
			else
			{
				created_at = "null";
				deleted_at = "null";
				vm_mac = "null";
				vm_ip = "null";
				
			}
			std::string str_vm_name = root["servers"][i]["name"].asString();
			vm_name = new char[str_vm_name.length()+1];
			strcpy(vm_name,str_vm_name.c_str());
			
			std::string user_id = root["servers"][i]["user_id"].asString();
			vm_user_id = new char[user_id.length()+1];
			strcpy(vm_user_id,user_id.c_str());
			
			std::string flavor_id = root["servers"][i]["flavor"]["id"].asString();
			std::string image_id = root["servers"][i]["image"]["id"].asString();
			
			std::string str_uuid = root["servers"][i]["id"].asString();
			instance_uuid = new char[str_uuid.length()+1];
			strcpy(instance_uuid,str_uuid.c_str());
			
			std::string str_user_name = user_map[user_id];
			vm_username = new char[str_user_name.length()+1];
			strcpy(vm_username,str_user_name.c_str());
			
		
			std::string str_vm_vcpu = flavor_vcpu_map[flavor_id];
			vcpus = atoi(str_vm_vcpu.c_str());
			
			std::string str_vm_ram = flavor_ram_map[flavor_id];
			memory_mb = atoi(str_vm_ram.c_str());
			
			std::string str_vm_disk = flavor_disk_map[flavor_id];
			disk_gb = atoi(str_vm_disk.c_str());
			
			std::string str_image_name = image_map[image_id];
			image_name = new char[str_image_name.length()+1];
			strcpy(image_name,str_image_name.c_str());
			
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"select vcpu_utility from %s.vms where uuid = '%s';",database,instance_uuid);
			msql1.free_res();
			msql1.query_select(cmd);
			if(!msql1.FetchRow())
			{
				vcpu_util = 0;
			}
			else
			{
				strcpy(key,"vcpu_utility");
				vcpu_util=atof(msql1.GetField(key));
			}
				
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"select mem_utility from %s.vms where uuid = '%s';",database,instance_uuid);
			msql1.free_res();
			msql1.query_select(cmd);
			if(!msql1.FetchRow())
			{
				mem_util = 0;
			}
			else
			{
				strcpy(key,"mem_utility");
				mem_util=atof(msql1.GetField(key));
			}
				
			
			string state(vm_state);
				if(str_user_name=="admin"&&state=="active")
				{
					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd,sizeof(cmd), "select * from %s.group_vm_map where vm_name='%s';",database,vm_name);
					msql1.free_res();
					msql1.query_select(cmd);
					if(!msql1.FetchRow()||!msql1.getrecordcount())
					{
						
					}
					else
					{
						strcpy(key,"group_name");
						group_name = msql1.GetField(key);
						strcpy(key,"batch");
						vm_batch = atoi(msql1.GetField(key));
						strcpy(key,"bonus");
						bonus = atof(msql1.GetField(key));
						
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd,sizeof(cmd), "insert into %s.group_vm_history set vm_name='%s',vm_ip='%s',vm_mac='%s',uuid='%s',batch=%d,create_at='%s',delete_at='%s',cells_id='%s',av_zone='%s',image_name='%s',vcpus=%d,mem_mb=%d,disk_gb=%d,user_name='%s',group_name='%s',vcpu_utility=%.2f,mem_utility=%.2f,disk_utility=%.2f,bonus=%.2f,host_name='%s',vm_state='%s',check_at=NOW();",
						database,vm_name, vm_ip,vm_mac,instance_uuid,vm_batch,created_at, deleted_at,cell_name,availability_zone,image_name, vcpus, memory_mb, disk_gb, vm_username,group_name, vcpu_util,mem_util,disk_util,bonus,host_domain,vm_state);
						msql2.insert(cmd);
					}
				}
				if(str_user_name!="admin")
				{
					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd,sizeof(cmd), "select * from %s.vms where uuid='%s';",database,instance_uuid);
					msql1.free_res();
					msql1.query_select(cmd);
					if(!msql1.FetchRow()||!msql1.getrecordcount())
					{
						
					}
					else
					{
						strcpy(key,"bonus");
						bonus = atof(msql1.GetField(key));
						
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd,sizeof(cmd), "insert into %s.user_vm_history set vm_name='%s',vm_ip='%s',vm_mac='%s',uuid='%s',create_at='%s',delete_at='%s',cells_id='%s',av_zone='%s',image_name='%s',vcpus=%d,mem_mb=%d,disk_gb=%d,user_name='%s',vcpu_utility=%.2f,mem_utility=%.2f,disk_utility=%.2f,bonus=%.2f,host_name='%s',vm_state='%s',check_at=NOW();",
						database,vm_name, vm_ip,vm_mac,instance_uuid,created_at, deleted_at,cell_name,availability_zone,image_name, vcpus, memory_mb, disk_gb, vm_username,vcpu_util,mem_util,disk_util,bonus,host_domain,vm_state);
						msql2.insert(cmd);
					}
				}

		}
		
	}
	is.close();
	return 1;
}

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


int group_history_scan()
{
	char* vm_name = "null";			//虚拟机名称
	char* vm_ip = "null";				//虚拟机IP	
	char* group_name = "null";	
	int batch=0;
	char* create_at = "null";		//虚拟机创建的时间
	char* delete_at = "null";
	char* image_name = "null";		//虚拟机使用的镜像的名称
	int vm_count = 0;				//给该项目组运行的虚拟机的数量
	int	 vcpus_count = 0;					//虚拟机占用的虚核数量
	int	 mem_count = 0;				//虚拟机占用的内存数量	
	int disk_count = 0;
	char* host_name="null";
	int IsDeleted=0;
	
	
	
	//const char filename[100];memset(filename,0,sizeof(filename);) = "/root/user.json";
	char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/user.json",json_dir.c_str());
	
	char* vm_domain = "null";			//虚拟机域名
	char* instance_uuid = "null";
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
	char* vm_username = "null";		//虚拟机创建的用户名称
	char* vm_mac = "null";			//虚拟机MAC
    char* cell_name = "null"; 
	char* availability_zone = "null";     //
	char* project_id = "null";        //
	char* vm_state = "null";			//虚拟机状态   active=1  or pause or showdown ...
	int flavor = 0;
	char* launched_on = "null";		//虚拟机创建在哪个宿主机
	int flag = 0;					//和shut_at配合 1表示开启，0表示关闭, -1表示刚创建
	char* start_at = "null";
	char* shut_at = "null";
	char* checked_at = "null";
	char* image_id = "null";			//虚拟机镜像的id
//	char flavor_name[64];		//资源匹配的名称
	char* flavor_id = "null";			//资源匹配的类型id
	double vcpu_util = 0;
	double mem_util = 0;
	double disk = 0;
	double SumScoreUsed = 0;
	double SumScoreRecharge = 0;
	double ScoreRepaid = 0;
	double SumScore = 0;
	int instances_all = 0;


	
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
		if(!msql1.FetchRow()||!msql1.getrecordcount())
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
		if(!msql2.FetchRow()||!msql2.getrecordcount())
				continue;
		else
		{
			strcpy(key,"SumScoreUsed");
			SumScoreUsed = atof(msql2.GetField(key));
			strcpy(key,"SumScoreRecharge");
			SumScoreRecharge = atof(msql2.GetField(key));
			strcpy(key,"ScoreRepaid");
			ScoreRepaid = atof(msql2.GetField(key));
			strcpy(key,"SumScore");
			SumScore = atof(msql2.GetField(key));
			
		}
		
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"insert into %s.group_history_info set group_name='%s',running_vms=%d,cores=%d,ram=%d,SumScoreUsed=%.2f,SumScoreRecharge=%.2f,ScoreRepaid=%.2f,SumScore=%.2f,checked_at=NOW();",database,group_name,vm_count,vcpus_count,mem_count,SumScoreUsed,SumScoreRecharge,ScoreRepaid,SumScore);
		msql2.insert(cmd);
	}
	
	
	
	
}

int vm_util_scan()
{
	int vm_count=0;
	char* uuid=NULL;
	double cpu_util=0;
	char* cpu_util_array=NULL;
	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"select count(uuid) as vm_count from %s.vms where vm_state='active';",database);
	msql.free_res();
	msql.query_select(cmd);
	int record_count = msql.getrecordcount();
	if(!msql.FetchRow())
			return 0;
	strcpy(key,"vm_count");
	vm_count = atoi(msql.GetField(key));
    Json::Reader reader;// 解析json用Json::Reader   
    Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         
	
	
	//获取token
	/*
	string token;
     
	system("curl -s -X POST http://192.168.81.38:5000/v2.0/tokens -H \"Content-Type: application/json\" -d '{\"auth\": {\"tenantName\": \"'\"admin\"'\", \"passwordCredentials\":{\"username\": \"'\"admin\"'\", \"password\": \"'\"opstk123\"'\"}}}' > /root/token.json");
	is.open(filename, std::ios::binary );  
	if (reader.parse(is, root, 0))  
    {
		token = root["access"]["token"]["id"].asString();
	}
	*/
	
	std::ifstream is; 
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\" -d '{ \"limit\": %d }' http://%s:8777/v2/meters/cpu_util |python -mjson.tool >%s/cpu_util.json",token.c_str(),vm_count,ceilometerAddr,json_dir.c_str());
	system(cmd);
	//char filename[100];memset(filename,0,sizeof(filename);) = "/root/cpu_util.json";
	char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/cpu_util.json",json_dir.c_str());
	is.open(filename, std::ios::binary );
	if (reader.parse(is, root, 0))  
    {  
        for(int i = 0; i < vm_count; ++i)  // 遍历数组  
        {
			std::string resource_id = root[i]["resource_id"].asString();
			std::string str_cpu_util = root[i]["counter_volume"].asString();
			
			uuid = new char[resource_id.length()+1];
			strcpy(uuid,resource_id.c_str());
			cpu_util_array = new char[str_cpu_util.length()+1];
			strcpy(cpu_util_array,str_cpu_util.c_str());
			cpu_util = atof(cpu_util_array);
			
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"update %s.vms set vcpu_utility=%f where uuid='%s';",database,cpu_util,uuid);
			msql.insert(cmd);
		}
	}

	
}
	
  
#define ACTION  1

/*
	主函数
*/
int main(int argc, char **argv)
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
	//filename = "/root/user.json";
	sprintf(filename,"%s/user.json",json_dir.c_str());
	is.open(filename, std::ios::binary );  
	if (reader.parse(is, root, 0))  
    {
		int user_size = root["users"].size();  // 得到数组个数
        for(int i = 0; i < user_size; ++i)  // 遍历数组  
        {
			str_username = root["users"][i]["username"].asString();
			str_user_id = root["users"][i]["id"].asString();
			user_map[str_user_id] = str_username;
			
		}
	}
	is.close();
	
	
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
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/os-quota-sets/%s |python -mjson.tool >%s/tenant_quota/%s_quota.json",token.c_str(),novaAddr,tenant_id.c_str(),str_tenant_id.c_str(),json_dir.c_str(),str_tenant_id.c_str());
			system(cmd);
			//filename = "/root/tenant_quota.json";
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
	
	
	//对flavor_id和flavor生成map
	string flavor_id;
	string flavor_vcpu;
	string flavor_ram;
	string flavor_disk;
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/flavors/detail |python -mjson.tool>%s/flavor.json",token.c_str(),novaAddr,tenant_id.c_str(),json_dir.c_str());
	system(cmd);
	//filename = "/root/flavor.json";
	//char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/flavor.json",json_dir.c_str());
	is.open(filename, std::ios::binary ); 
	if (reader.parse(is, root, 0))  
    {
		int flavor_size = root["flavors"].size(); 
		for(int i = 0; i < flavor_size; ++i)  // 遍历数组  
		{
			 flavor_id = root["flavors"][i]["id"].asString();
			 flavor_vcpu = root["flavors"][i]["vcpus"].asString();
			 flavor_ram = root["flavors"][i]["ram"].asString();
			 flavor_disk = root["flavors"][i]["disk"].asString();
			 
			 flavor_vcpu_map[flavor_id] = flavor_vcpu;
			 flavor_ram_map[flavor_id] = flavor_ram;
			 flavor_disk_map[flavor_id] = flavor_disk;
		}
	}
	is.close();
	
	//对image_id和image_name 生成map
	string image_id;
	string image_name;
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/images |python -mjson.tool >%s/image.json",token.c_str(),novaAddr,tenant_id.c_str(),json_dir.c_str());
	system(cmd);
	//filename = "/root/image.json";
	//char filename[100];memset(filename,0,sizeof(filename));
	sprintf(filename,"%s/image.json",json_dir.c_str());
	
	is.open(filename, std::ios::binary ); 
	if (reader.parse(is, root, 0))  
    {
		int image_size = root["images"].size(); 
		for(int i = 0; i < image_size; ++i)  // 遍历数组  
		{
			 image_id = root["images"][i]["id"].asString();
			 image_name = root["images"][i]["name"].asString();
			 image_map[image_id] = image_name;
		}
	}
		
	//获取所有vm
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/servers/detail?all_tenants=1 |python -mjson.tool >%s/vm.json",token.c_str(),novaAddr,tenant_id.c_str(),json_dir.c_str());
	system(cmd);
	
	
	//	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/os-simple-tenant-usage?start=%s |python -mjson.tool > %s/server_diagnostics.json",token.c_str(),novaAddr,tenant_id.c_str(),argv[1],json_dir.c_str());
	system(cmd);
	
		
	
		
	rc = msql.init(mysqlHost,mysqlUsername,mysqlPwd,database);
	if(rc == -1) {printf("error for mysql init\n");printf(msql.getmsg());return 0;}
	
	rc = msql1.init(mysqlHost,mysqlUsername,mysqlPwd,database);
	if(rc == -1) {printf("error for mysql1 init\n");printf(msql1.getmsg());return 0;}
	
	rc = msql2.init(mysqlHost,mysqlUsername,mysqlPwd,database);
	if(rc == -1) {printf("error for mysql1 init\n");printf(msql1.getmsg());return 0;}
	//while(1)
	//{

		clock_t start_time=clock();
		cloud_vm_scan();
		vm_util_scan();
		vm_history_scan();
		score();
		user_scan();
		user_history_scan();
		group_scan();
		group_history_scan();
		hypervisor_history_scan();
		//char* a_str = "zhuyue";
		//char* b_str = "instances";
		cloud_scan();
		

		//user_usage_scan();

		//project_scan();
		//const char filename[100];memset(filename,0,sizeof(filename);) = "/root/user.json";
		//ReadJsonFromFile(filename);
		clock_t end_time=clock();
		
		cout<< "Running time is: "<<end_time-start_time<<"ms"<<endl;//输出运行时间
		
	//}
	msql.close();
   
	delete(key);
	//getstrtime(msg);
	printf("----%s\n",msg);
	return 0;
}
