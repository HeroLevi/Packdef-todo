#ifndef _TCPNET_H
#define _TCPNET_H

#include <stdlib.h>
#include <assert.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

#include "INet.h"
#include "TCPKernel.h"
#include "MySql.h"

#define _DEFPORT 8001
#define _DEFIP "127.0.0.1"
#define _DEFSIZE 1024
#define MAX_EVENTS 1024

extern ThreadPool* m_pool;

class TCPNet:public INet
{
public:
	TCPNet(IKernel* pKernel);
	virtual ~TCPNet();
public:
	bool InitNetWork();
	void UnInitNetWork();
	bool SendData(int sock,char* szbuf,int nlen);
public:
	static void addfd(int epollfd,int fd);
	static void removefd(int epollfd,int fd);
	static void setnonblocking(int fd);
public:
	//线程接收ACCEPT RECV
//	static void* ThreadAccept(void*);
//	static void* ThreadRecv(void*);
	
	static void* Jobs(void*);
public:
	static IKernel *m_pKernel;
	int s_socket;
	int epollfd;
	static bool m_flag;
};



#endif
