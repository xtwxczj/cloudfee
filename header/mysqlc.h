/*
	MYSQL���ݿ������˵��

	ע������ҪMYSQL���ݿ⿪ͨ����Ȩ�ޣ�����linux������Viptables�������ܷ��ʳɹ�
	MysqlȨ������
	grant all privileges on *.* to 'root'@'202.122.33.9' with grant option; 

	���뷽��
	 gcc dbtest.c -o dbtest -I /usr/local/mysql/include/mysql/ -l mysqlclient
	 û�а�װmysql��ϵͳ����Ҫ���������ļ�������֧�ֱ���
	 /usr/lib/libmysqlclient.so.15
	 /usr/local/mysql/include/mysql.h

    ��װ�����ļ���֧�ֵķ���
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
	//���ݿ����
	MYSQL		m_conn;		//���Ӿ��
	MYSQL_RES 	*m_res;		//��ѯ����
	MYSQL_ROW   m_row;		//�ַ�����������֧�ֶ�����
	MYSQL_FIELD *m_field;	//���ֶ���Ϣ

	long		m_rcount;	//��¼��
	long		m_fcount;	//�ֶ���
	
	int m_iFields; //�ֶθ���
	std::map<std::string, int> m_mapFieldNameIndex; //��һ��map,  key���ֶ���,  value���ֶ�����
	//MYSQL_ROW m_row; //һ��,  typedef char **MYSQL_ROW;
	
	//�������ݻ�����ָ��,�û�����ÿ���ֶ�ռ256�ֽ�,˳����,ֱ������
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
	//��ʼ��
	int			init(char *host,char *username,char *userpwd,char *dbname);
	//��ѯ
	int			query(char *sqlcom);

	int			query_count(char *sqlcom);

	//�����¼
	int			insert(char *sqlcom);
	//��ȡ��ѯ���ؽ�����ֶ�����
	int			getfieldcount();
	//��ȡ��ѯ���ؽ���ļ�¼��
	int			getrecordcount();
	//���ػ������а����±��ĳһ����ŵ�ֵ
	int			getvalue(char *value, int index);
	//�رղ�ѯ,�ͷ��ڴ�
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
