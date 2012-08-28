/* 
 * filename : CSCClient.cpp
 * author   : sundayman
 * date     : Aug 27, 2012
 * brief    : 
 */


#include "../src/service.h"

using namespace Net;

int main(int argc, char* argv[]) {

	Service svr(NULL, 5053);
	svr.Init();
	svr.Start();
	for (;;) {
		sleep(1);
	}
	svr.Stop();
	svr.Uninit();
	return 0;
}
