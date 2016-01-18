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


void* cloud_vm_scan(void* args)
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
				snprintf(cmd,sizeof(cmd), "select * from %s.vms where uuid = '%s';",database,instance_uuid);
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
				snprintf(cmd,sizeof(cmd),"update %s.vms set vm_ip='%s',vm_mac='%s',create_at='%s', delete_at='%s',check_at=NOW(),av_zone='%s',vcpus=%d,mem_mb=%d,disk_gb=%d,user_name='%s',vcpu_utility=%.2f,cpu_hour=%.2f,host_name='%s', image_name='%s',vm_state='%s' where uuid='%s';"
				,database,vm_ip,vm_mac,created_at,deleted_at,availability_zone,vcpus,memory_mb,disk_gb,vm_username,vcpu_util,vcpu_hour,host_domain,image_name,vm_state,instance_uuid);
				msql2.insert(cmd);
				}


		}
		
	}
	is.close();
	
}

void* cloud_host_scan(void* args)
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
	//获取所有的hypervisor信息std::string str_mac;

	Json::Reader reader;// 解析json用Json::Reader   
	Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array    

	std::ifstream is;  

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


	//获取所有vm
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/servers/detail?all_tenants=1 |python -mjson.tool >%s/vm.json",token.c_str(),novaAddr,tenant_id.c_str(),json_dir.c_str());
	system(cmd);
	
	//retrieve vcpu hour	
	memset(cmd,0,sizeof(cmd));
	snprintf(cmd,sizeof(cmd),"curl -X GET -H  \"X-Auth-Token:%s\" -H \"Content-Type: application/json\"  http://%s:8774/v2/%s/os-simple-tenant-usage?start=%s |python -mjson.tool > %s/server_diagnostics.json",token.c_str(),novaAddr,tenant_id.c_str(),argv[1],json_dir.c_str());
	system(cmd);
	
	

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
	is.close();



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

}
