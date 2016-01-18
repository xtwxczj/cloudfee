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

	vm_history_scan();
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
