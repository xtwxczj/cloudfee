/*
	���๦������:
		ʵ�ֲ�ѯ,�����MYSQL����,���ؽ��

	���ؽ����ȡ��ʽ:  ͨ��m_dataָ��������Ӧ���ڴ�,ÿ���ֶθ���MYSQL�ַ�����
	    ���255�ֽڵ��趨,ȡ256�ֽ�,�����м�¼��ÿ���ֶΰ���256�ֽڵĻ�����˳
		����.
		ͨ��getfieldcount()��getrecordcount()��getvalue(int index)������������
		��ȡ���ݻ�����������һ���ֶε��ַ�����ʽ��ֵ

	��������:
		��װyum install -y mysql-devel.x86_64
		����ʱ����  -l mysqlclient ѡ��
		�����ļ�Ŀ¼  /usr/include/mysql/

        ��װ 
		yum install mariadb-devel.x86_64
		yum install -y libcurl-devel.x86_64
		cp /usr/lib64/mysql/libmysqlclient.so* /usr/lib64/
		�޸����ݿ����� 
		���root���ʵ�Ȩ�� 



*/
#include "../header/mysqlc.h"

MY_SQLC::MY_SQLC()
{
	memset(&m_conn,0,sizeof(MYSQL));
	m_res = NULL;
	m_data = NULL;
	memset(&m_row,0,sizeof(MYSQL_ROW));
	memset(m_host,0,128);
	memset(m_sqlcom,0,4096);
	memset(m_username,0,128);
	memset(m_userpwd,0,128);
	memset(m_dbname,0,128);
	memset(m_msgbox,0,128);
}

MY_SQLC::~MY_SQLC()
{


}
/*
	����:MYSQL���ݿ�������ʼ��
	����:
		��ʼ�����������ݿ�
		�����û������ݿ��ļ�����������ַ
		������ݿ�û�йرգ����ش���
	����:
	����:
	˵��:
	
	  �ǵ�һ��Ҫȷ���ϴ�ʹ�ú��Ѿ��ر�
*/
int	MY_SQLC::init(char *host,char *username,char *userpwd,char *dbname){
	MYSQL *rcp = NULL;
	//int rc = 0;
	if(m_conn.server_status>0) 
	{
		sprintf(m_msgbox,"mysql handle is not emtye! close mysql!\n");
		return -1;
    }

	//��ʼ�����ݿ����ӣ��ǵñ���
	if(host != NULL) 
	{
		memset(&m_conn,0,sizeof(MYSQL));
		m_res = NULL;
		memset(&m_row,0,sizeof(MYSQL_ROW));
		memset(m_host,0,128);
		memset(m_sqlcom,0,128);
		memset(m_username,0,128);
		memset(m_userpwd,0,128);
		memset(m_dbname,0,128);
		memset(m_msgbox,0,128);

		//��ʼ������
		strcpy(m_host,host);
		strcpy(m_username,username);
		strcpy(m_userpwd,userpwd);
		strcpy(m_dbname,dbname);
	}
	else  
	{
		strcpy(m_username,username);
		strcpy(m_userpwd,userpwd);
		strcpy(m_dbname,dbname);
	}

	//�������
	if(m_host == NULL) return -1;

	//��ʼ��MYSQL�ṹ
	rcp = mysql_init(&m_conn);
    if(rcp == NULL) {
		sprintf(m_msgbox,"error code:%d %s\n",mysql_errno(&m_conn),mysql_error(&m_conn));
		printf("msg = %s\n",m_msgbox);
		return -1;
    }
	
	//printf("mysql_init:m_host = %s  user = %s  dbname= %s pwd = %s\n",m_host,m_username,m_dbname,userpwd);	
	//�������ݿ�
    rcp = mysql_real_connect(&m_conn,m_host,m_username,m_userpwd,m_dbname,0,NULL,0);
	//rc =(int) mysql_change_user(&g_conn,username,NULL,db);
 	//rc = mysql_select_db(&m_conn,"mysql");
    if(rcp == NULL) 
	{
		sprintf(m_msgbox,"open mysql database failed! error code:%d %s\n",(signed int)mysql_errno(&m_conn),mysql_error(&m_conn));
		//sprintf(m_msgbox,"open mysql database failed!\n");
		//printf("msg = %s\n",m_msgbox);
		return -1;
	}
	return 0;
}






int MY_SQLC::query(char *sqlcom)
{
	int rc = 0;
	int i=0,j=0,k=0;
        memset(m_sqlcom,0,4096);
	strcpy(m_sqlcom,sqlcom);

	//��ѯ
	/*
	mysql_real_query��mysql_query���˸�����: �ַ���query�ĳ���, �����ʺ��ж��������ݵ�query, ��mysql_query���ַ���query���ܰ���������,��Ϊ����\0Ϊ��β
	*/
	rc = mysql_real_query(&m_conn,m_sqlcom,strlen(m_sqlcom));//�ʺ϶����ƴ���
	//rc = mysql_query(&m_conn,m_sqlcom);//�ʺ��ַ���
	if(rc != 0) {
		sprintf(m_msgbox,"SQL:query error code:%d %s\n",mysql_errno(&m_conn),mysql_error(&m_conn));
		return -1;
	}
	//��ȡ��ѯ���
#if 0
	/*
		mysql_use_result����������¼���ݵ������ڴ棬
		��¼�������ֶ���Ϣ�޷�ֱ�Ӵ����ݽṹ�л�ȡ
		��¼��Ҫͨ��mysql_fetch_row���������ݿ���
		�����ȡ
	*/
	m_res = mysql_use_result(&m_conn); 
	//����ѯ���ת�����ַ�������
	m_row = mysql_fetch_row(m_res);
	printf("row_count = %ld\n",(long int)mysql_num_rows(m_res));
	printf("MySQL Tables in mysql database:\n"); 
	while ((m_row = mysql_fetch_row(m_res)) != NULL) 
		printf("%03d: %20s %20s %20s %20s\n",mysql_row_tell(m_res),m_row[0],m_row[1],m_row[2],m_row[3]); 
#endif

	/*
		mysql_store_result������ȫ��ȡ��¼���ݵ������ڴ��У�
		��¼�������ֶ���Ϣ����ֱ�Ӵ����ݽṹ�л�ȡ
	*/
	m_res = mysql_store_result(&m_conn); 
	//��ȡ�ֶ�����
	m_fcount = mysql_num_fields(m_res);//printf("field_count = %ld\n",m_fcount);
	//��ȡ��¼����
	m_rcount = mysql_num_rows(m_res);//printf("row_count = %lu\n",m_rcount);

	//�������ݱ��ڴ�,mysqlÿ���ַ����������Ϊ255�ֽ�,��������ÿ���ַ�������Ϊ256�ֽ�
	m_data = (char *)malloc(m_rcount * m_fcount * 256*2);	
	if(m_data == NULL) 
	{
		sprintf(m_msgbox,"SQL:query succeed but memory malloc failed!");
		return -1;
	}
#if 0
	for(rc =0;rc<m_fcount;rc++)
	{
		m_field = mysql_fetch_field(m_res);
		printf("%20s ",m_field->name);
	}
	printf("\n");
#endif

	for(i =0;i<m_rcount;i++)
	{
		m_row = mysql_fetch_row(m_res);
		for(j =0;j<m_fcount;j++){ 
                     if(m_row[j]==NULL)
                            sprintf(&m_data[256*k++],"%s","null"); 
                     else 
                            sprintf(&m_data[256*k++],"%s",m_row[j]);
                  	}
    }

	

	//mysql_autocommit(m_conn,1);//ǿ������ִ�����������
	//�������ܴ�Ĳ�ѯ����Ҫ���
	mysql_free_result(m_res);

	
	return 0;
}

int	 MY_SQLC::query_count(char *sqlcom)
{
	int rc = 0;
        memset(m_sqlcom,0,4096);
	strcpy(m_sqlcom, sqlcom);




	rc = mysql_real_query(&m_conn, m_sqlcom, strlen(m_sqlcom));//������?
	if (rc != 0) {
		sprintf(m_msgbox, "SQL:query error code:%d %s\n", mysql_errno(&m_conn), mysql_error(&m_conn));
		return -1;
	}
	//m_res = mysql_use_result(&m_conn);
	m_res = mysql_store_result(&m_conn); 
	int num =(int)mysql_num_rows(m_res); 
        mysql_free_result(m_res);
	return num;
}

int MY_SQLC::query_select(char *sqlcom)
{
	int rc = 0;
    try{
        memset(m_sqlcom,0,4096);

	strcpy(m_sqlcom, sqlcom);
	rc = mysql_real_query(&m_conn, m_sqlcom, strlen(m_sqlcom));//������?
	if (rc != 0) {
		sprintf(m_msgbox, "SQL:query error code:%d %s\n", mysql_errno(&m_conn), mysql_error(&m_conn));
		return -1;
	}
	//m_res = mysql_use_result(&m_conn);
	m_res = mysql_store_result(&m_conn); 
	
	//ȡ�ֶεĸ���
    m_iFields = mysql_num_fields(m_res);
    m_mapFieldNameIndex.clear();
    //ȡ�����ֶε�������Ϣ
    MYSQL_FIELD *fields;
    fields = mysql_fetch_fields(m_res);
    //���ֶ����ֺ��������浽һ��map��
    for (unsigned int i = 0; i < m_iFields; i++) 
        m_mapFieldNameIndex[fields[i].name] = i;


    } catch (...){
	printf("# ERR:(MYSQL SQLState:%s)",sqlcom);


//	printf("# ERR:%s\n",e.what());
//	printf("(MYSQL error code:%s,SQLState:%s\n",e.getErrorCode(),e.getSQLState());
}



	return 0;
}

int MY_SQLC::free_res()
{
	if(m_res!=NULL)
		mysql_free_result(m_res); 
	return 0;
}

int MY_SQLC::FetchRow() {
    //��������Ϊ��,��ֱ�ӷ��ؿ�; ����FetchRow֮ǰ, �����ȵ��� SelectQuery(...)
    if (m_res == NULL)
        return 0;
    //�ӽ������ȡ��һ��
	else
	{
		 m_row = mysql_fetch_row(m_res);
		return 1;
	}
   
}

char* MY_SQLC::GetField(const char* szFieldName) {
    return GetField(m_mapFieldNameIndex[szFieldName]);
}
 
char* MY_SQLC::GetField(unsigned int iFieldIndex) {
	char* zero = "0.0000000";
    //��ֹ����������Χ
    if (iFieldIndex >= m_iFields)
	{
		return NULL;
	}
	else if(m_row[iFieldIndex]==0x00)
	{
		return zero;
	}
	else
		return m_row[iFieldIndex];
}
										

int MY_SQLC::getfieldcount()
{
	return mysql_num_fields(m_res);
}

int MY_SQLC::getrecordcount()
{
	return mysql_num_rows(m_res);
}

int MY_SQLC::getvalue(char *value, int index)
{
	sprintf(value,"%s",&m_data[index*256]);
	return strlen(value);
}

int MY_SQLC::close()
{

	free(m_data);
	m_data = NULL;
    // close connection
    //mysql_free_result(m_res); 
	m_res = NULL;
    mysql_close(&m_conn); 

	return 0;
}

int	MY_SQLC::insert(char *sqlcom)
{
	int rc = 0;
	int i=0,j=0,k=0;

	strcpy(m_sqlcom,sqlcom);
	rc = mysql_real_query(&m_conn,m_sqlcom,strlen(m_sqlcom));//�ʺ϶����ƴ���

	//��ѯ
	/*
	mysql_real_query��mysql_query���˸�����: �ַ���query�ĳ���, �����ʺ��ж��������ݵ�query, ��mysql_query���ַ���query���ܰ���������,��Ϊ����\0Ϊ��β
	*/
	//try
	//{
		//rc = mysql_real_query(&m_conn,m_sqlcom,strlen(m_sqlcom));//�ʺ϶����ƴ���
		//if(rc != 0){
			//throw rc;
		//}
	//}
	//catch(...)
	//{
		//cout<<"catch Exception!:sqlcom="<<sqlcom<<endl;
		// ���� ExceptionName �쳣�Ĵ���
	//}
	
	//rc = mysql_query(&m_conn,m_sqlcom);//�ʺ��ַ���
	//printf("%s\n",sqlcom);
	if(rc != 0) {
		sprintf(m_msgbox,"SQL:query error code:%d %s\n",mysql_errno(&m_conn),mysql_error(&m_conn));
		printf("error!!!!!!!!!:%s",sqlcom);
		return -1;
	}
	return 0;
}

char *MY_SQLC::getmsg()
{
	return m_msgbox;
}

int MY_SQLC::cout_row()
{
	printf("this is %s",m_row);
	return 0;
}
