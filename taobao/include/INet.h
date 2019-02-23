#ifndef _NET_H
#define _NET_H

#include "ThreadPool.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>  
#include <sys/epoll.h>
#include <list>
using namespace std;

#define IPSIZE 16
#define BUFSIZE 1500
#define EPOLLSIZE 500

class INet
{
public:
	INet(){}
	virtual ~INet(){}
public:
	virtual bool InitNetWork() = 0;
	virtual void UnInitNetWork() = 0;
	virtual bool SendData(int sock,char* szbuf,int nlen) = 0;
};

#endif
