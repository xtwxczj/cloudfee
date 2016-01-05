/*
	MYSQL数据库操作类说明

	注意首先要MYSQL数据库开通访问权限，并且linux主机的Viptables允许，才能访问成功
	Mysql权限命令
	grant all privileges on *.* to 'root'@'202.122.33.9' with grant option; 

	编译方法
	 gcc dbtest.c -o dbtest -I /usr/local/mysql/include/mysql/ -l mysqlclient
	 没有安装mysql的系统，需要拷贝两个文件，才能支持编译
	 /usr/lib/libmysqlclient.so.15
	 /usr/local/mysql/include/mysql.h

    安装帮助文件和支持的方法
	yum install -y mysql-devel.x86_64


*/
#ifndef MYSQLC
#define MYSQLC 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include "mysql/mysql.h"
#include <stdexcept>
//#include <cppconn/exception.h>
#include <exception>

using namespace std;
class MY_SQLC {
	//typedef std::map<std::string, int> MapFieldNameIndex;

private:
	//数据库变量
	MYSQL		m_conn;		//连接句柄
	MYSQL_RES 	*m_res;		//查询返回
	MYSQL_ROW   m_row;		//字符串变量，可支持二进制
	MYSQL_FIELD *m_field;	//表字段信息

	long		m_rcount;	//记录数
	long		m_fcount;	//字段数
	
	int m_iFields; //字段个数
	std::map<std::string, int> m_mapFieldNameIndex; //是一个map,  key是字段名,  value是字段索引
	//MYSQL_ROW m_row; //一行,  typedef char **MYSQL_ROW;
	
	//返回数据缓冲区指针,该缓冲区每个字段占256字节,顺序存放,直到结束
	char		*m_data;

	char		m_host[128];
	char		m_sqlcom[4096];
	char		m_username[128];
	char		m_userpwd[128];
	char		m_dbname[128];

	char		m_msgbox[128];


public:
	MY_SQLC();
	~MY_SQLC();
	//初始化
	int			init(char *host,char *username,char *userpwd,char *dbname);
	//查询
	int			query(char *sqlcom);

	int			query_count(char *sqlcom);

	//插入记录
	int			insert(char *sqlcom);
	//获取查询返回结果的字段数量
	int			getfieldcount();
	//获取查询返回结果的记录数
	int			getrecordcount();
	//返回缓冲区中按照下标的某一个序号的值
	int			getvalue(char *value, int index);
	//关闭查询,释放内存
	int			close();

	char		*getmsg();
	
	int query_select(char *sqlcom);
	//char* GetField(const char* szFieldName);
	int FetchRow();

	char* GetField(const char* szFieldName);
	
	char* GetField(unsigned int iFieldIndex);

    int free_res();

	int cout_row();
};
#endif
