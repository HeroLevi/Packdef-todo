#include "TCPKernel.h"


TCPKernel::TCPKernel()
{
	
	m_pTcpNet = new TCPNet(this);
	m_pool = new ThreadPool;

}

TCPKernel::~TCPKernel()
{
	if(m_pTcpNet)
	{
		delete m_pTcpNet;
		m_pTcpNet = NULL;
	}
}

bool TCPKernel::Open()
{
	//1.初始化内存池
	unsigned int block_len[10] = {8,16,32,64,128,256,512,1024,2048,4096};
	unsigned int block_count[10] = {10,10,10,10,10,10,10,10,10,10};
	
	if(buffer_pool_init(10,block_len,block_count))
		return false;
	//2.初始化线程池
	if(!m_pool->InitThreadPool(500,10,100))
		return false;
	//3.初始化网络
	if(!(m_pTcpNet->InitNetWork()))
		return false;
	//4.初始化数据库
	if(!m_sql.ConnectMySql((char*)"localhost",(char*)"root",(char*)"colin123",(char*)"test"))
		return false;
	return true;
}

void TCPKernel::Close()
{
	m_pTcpNet->UnInitNetWork();
	m_pool->DestroyThreadPool();
}

void TCPKernel::DealData(int sock,char* szbuf)
{
	char szsql[300] = {0};
	list<string> lststr;
//	sprintf(szsql,"select * from tmp where name = '456'");
	sprintf(szsql,"insert into tmp values('%s')",szbuf);
	if(!m_sql.UpdateMySql(szsql))
	{
		cout<<"Update failed"<<endl;
	}

}
