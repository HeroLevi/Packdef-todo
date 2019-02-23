#include "MySql.h"

MySql::MySql()
{
	sock = new MYSQL;
	mysql_init(sock);
	mysql_set_character_set(sock,"gb2312");
}

MySql::~MySql()
{
	if(sock)
	{
		delete sock;
		sock = NULL;
	}
}

void MySql::DisConnect()
{
	mysql_close(sock);
}

bool MySql::ConnectMySql(char *host,char *user,char *pass,char *db)
{

	if (!mysql_real_connect(sock, host, user, pass, db, 0, NULL, CLIENT_MULTI_STATEMENTS))
	{
		//连接错误
		return false;
	}
	return true;
}

bool MySql::SelectMySql(char* szSql,int nColumn,list<string>& lstStr)
{
	if(mysql_query(sock,szSql))
		return false;
	results = mysql_store_result(sock);
	
	if(NULL == results)
		return false;
	while (record = mysql_fetch_row(results))
	{
		for(int i = 0;i < nColumn;i++)
		{
			lstStr.push_back(record[i]);
		}
	}
	cout<<"result:"<<results<<endl;
	return true;
}

bool MySql::UpdateMySql(char* szSql)
{
	if(!szSql)
		return false;

	if(!mysql_query(sock,szSql))
	{
		return false;
	}
	return true;
}



