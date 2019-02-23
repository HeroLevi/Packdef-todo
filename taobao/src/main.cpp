#include "TCPNet.h"
#include "TCPKernel.h"
#include <iostream>
using namespace std;


int main(void)
{
	IKernel* pKernel = new TCPKernel;
	if(!pKernel->Open())
	{
		cout<<"open failed..."<<endl;
		return 0;
	}
	return 0;
}
