/* 
 * filename : CSCClient.cpp
 * author   : sundayman66@gmail.com
 * date     : Aug 27, 2012
 * brief    : 
 */


#include "../src/service.h"

using namespace Net;

int main(int argc, char* argv[]) {

	Service svr("192.168.27.100", 5053);
	svr.Init();
	svr.Start();
	for (int i = 0; i < 50; i ++) {
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
