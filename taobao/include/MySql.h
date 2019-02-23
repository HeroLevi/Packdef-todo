#ifndef _MYSQL_H
#define _MYSQL_H

#include <list>
#include <mysql/mysql.h>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;

class MySql
{
public:
	MySql();
	~MySql();
public:
	bool ConnectMySql(char* host,char* user,char* pass,char* db);
	void DisConnect();
	bool SelectMySql(char* szSql,int nColumn,list<string>& lstStr);

	bool UpdateMySql(char* szSql);
private:
	MYSQL *sock;
	MYSQL_RES *results;
	MYSQL_ROW record;
};

#endif
