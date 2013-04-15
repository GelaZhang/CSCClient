/* 
 * filename : CSCClient.cpp
 * author   : sundayman66@gmail.com
 * date     : Aug 27, 2012
 * brief    : 
 */


#include "../src/service.h"
#include "protocol/default_factory.h"
using namespace Net;
#pragma comment( linker, "/subsystem:\"Windows\" /entry:\"mainCRTStartup\"" )
int main(int argc, char* argv[]) {


	DefaultFactory embassy;
	Service svr("192.168.27.100", 5055);
	svr.Init(&embassy);
	svr.Start();
	for (;;) {
#ifndef WIN32
		sleep(1);
#else
        Sleep(1000);
#endif
	}
	svr.Stop();
	svr.Uninit();
	return 0;
}
