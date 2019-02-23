#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H

#include "IKernel.h"
#include "TCPNet.h"
#include "PackDef.h"
#include "ThreadPool.h"
#include "MySql.h"
#include "Memorypool.h"
class TCPKernel:public IKernel
{
public:
	TCPKernel();
	virtual ~TCPKernel();
public:
	bool Open();
	void Close();
	void DealData(int sock,char* szbuf);
public:
	
public:
	INet* m_pTcpNet;
	ThreadPool* m_pool;
	MySql m_sql;
//	unsigned int block_len[10];
//	unsigned int block_count[10];
};

#endif
