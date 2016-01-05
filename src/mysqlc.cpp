/*
	该类功能如下:
		实现查询,插入等MYSQL操作,返回结果

	返回结果获取方式:  通过m_data指针申请相应的内存,每个字段根据MYSQL字符便浪
	    最大255字节的设定,取256字节,即所有记录的每个字段按照256字节的缓冲区顺
		序存放.
		通过getfieldcount()、getrecordcount()、getvalue(int index)三个函数可以
		读取数据缓冲区的任意一个字段的字符串形式的值

	编译需求:
		安装yum install -y mysql-devel.x86_64
		编译时增加  -l mysqlclient 选项
		包含文件目录  /usr/include/mysql/

        安装 
		yum install mariadb-devel.x86_64
		yum install -y libcurl-devel.x86_64
		cp /usr/lib64/mysql/libmysqlclient.so* /usr/lib64/
		修改数据库密码 
		添加root访问的权限 



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
	函数:MYSQL数据库访问类初始化
	功能:
		初始化并连接数据库
		更新用户或数据库文件名，不换地址
		如果数据库没有关闭，返回错误
	参数:
	返回:
	说明:
	
	  记得一定要确定上次使用后已经关闭
*/
int	MY_SQLC::init(char *host,char *username,char *userpwd,char *dbname){
	MYSQL *rcp = NULL;
	//int rc = 0;
	if(m_conn.server_status>0) 
	{
		sprintf(m_msgbox,"mysql handle is not emtye! close mysql!\n");
		return -1;
    }

	//初始化数据库连接，记得必须
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

		//初始化变量
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

	//参数检查
	if(m_host == NULL) return -1;

	//初始化MYSQL结构
	rcp = mysql_init(&m_conn);
    if(rcp == NULL) {
		sprintf(m_msgbox,"error code:%d %s\n",mysql_errno(&m_conn),mysql_error(&m_conn));
		printf("msg = %s\n",m_msgbox);
		return -1;
    }
	
	//printf("mysql_init:m_host = %s  user = %s  dbname= %s pwd = %s\n",m_host,m_username,m_dbname,userpwd);	
	//连接数据库
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

	//查询
	/*
	mysql_real_query比mysql_query多了个参数: 字符串query的长度, 所以适合有二进制数据的query, 而mysql_query的字符串query不能包含二进制,因为它以\0为结尾
	*/
	rc = mysql_real_query(&m_conn,m_sqlcom,strlen(m_sqlcom));//适合二进制代码
	//rc = mysql_query(&m_conn,m_sqlcom);//适合字符串
	if(rc != 0) {
		sprintf(m_msgbox,"SQL:query error code:%d %s\n",mysql_errno(&m_conn),mysql_error(&m_conn));
		return -1;
	}
	//获取查询结果
#if 0
	/*
		mysql_use_result函数不将记录内容到本地内存，
		记录数量，字段信息无法直接从数据结构中获取
		记录需要通过mysql_fetch_row函数从数据库中
		逐个读取
	*/
	m_res = mysql_use_result(&m_conn); 
	//将查询结果转换成字符串数组
	m_row = mysql_fetch_row(m_res);
	printf("row_count = %ld\n",(long int)mysql_num_rows(m_res));
	printf("MySQL Tables in mysql database:\n"); 
	while ((m_row = mysql_fetch_row(m_res)) != NULL) 
		printf("%03d: %20s %20s %20s %20s\n",mysql_row_tell(m_res),m_row[0],m_row[1],m_row[2],m_row[3]); 
#endif

	/*
		mysql_store_result函数完全读取记录内容到本地内存中，
		记录数量，字段信息可以直接从数据结构中获取
	*/
	m_res = mysql_store_result(&m_conn); 
	//获取字段数量
	m_fcount = mysql_num_fields(m_res);//printf("field_count = %ld\n",m_fcount);
	//获取记录数量
	m_rcount = mysql_num_rows(m_res);//printf("row_count = %lu\n",m_rcount);

	//申请数据表内存,mysql每个字符串变量最大为255字节,因此申请的每个字符串变量为256字节
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

	

	//mysql_autocommit(m_conn,1);//强制立即执行上面的命令
	//数据量很大的查询才需要这个
	mysql_free_result(m_res);

	
	return 0;
}

int	 MY_SQLC::query_count(char *sqlcom)
{
	int rc = 0;
        memset(m_sqlcom,0,4096);
	strcpy(m_sqlcom, sqlcom);




	rc = mysql_real_query(&m_conn, m_sqlcom, strlen(m_sqlcom));//靠靠靠?
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
	rc = mysql_real_query(&m_conn, m_sqlcom, strlen(m_sqlcom));//靠靠靠?
	if (rc != 0) {
		sprintf(m_msgbox, "SQL:query error code:%d %s\n", mysql_errno(&m_conn), mysql_error(&m_conn));
		return -1;
	}
	//m_res = mysql_use_result(&m_conn);
	m_res = mysql_store_result(&m_conn); 
	
	//取字段的个数
    m_iFields = mysql_num_fields(m_res);
    m_mapFieldNameIndex.clear();
    //取各个字段的属性信息
    MYSQL_FIELD *fields;
    fields = mysql_fetch_fields(m_res);
    //把字段名字和索引保存到一个map中
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
    //如果结果集为空,则直接返回空; 调用FetchRow之前, 必须先调用 SelectQuery(...)
    if (m_res == NULL)
        return 0;
    //从结果集中取出一行
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
    //防止索引超出范围
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
	rc = mysql_real_query(&m_conn,m_sqlcom,strlen(m_sqlcom));//适合二进制代码

	//查询
	/*
	mysql_real_query比mysql_query多了个参数: 字符串query的长度, 所以适合有二进制数据的query, 而mysql_query的字符串query不能包含二进制,因为它以\0为结尾
	*/
	//try
	//{
		//rc = mysql_real_query(&m_conn,m_sqlcom,strlen(m_sqlcom));//适合二进制代码
		//if(rc != 0){
			//throw rc;
		//}
	//}
	//catch(...)
	//{
		//cout<<"catch Exception!:sqlcom="<<sqlcom<<endl;
		// 处理 ExceptionName 异常的代码
	//}
	
	//rc = mysql_query(&m_conn,m_sqlcom);//适合字符串
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
