/*
 * service.cpp
 *
 *  Created on: 2012-7-6
 *      Author: sundayman66@gmail.com
 */
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#endif

#include "service.h"

#include <string.h>

#include "syslog.h"


using namespace Net;

#ifdef WIN32
bool InitWinSocket()
{
    WSADATA wsd;
    if ( 0 != WSAStartup(MAKEWORD(2,2), &wsd) )
    {
        return false;
    }
    if ( 2 == LOBYTE(wsd.wVersion) && 2 == HIBYTE(wsd.wVersion) )
    {
        return true;
    }
    return false;
}

void UnInitWinSocket()
{
    if ( 0 != WSACleanup() )
    {
        int errCode = WSAGetLastError();
        //¼ÓÈëlog
    }
}

#endif

const int Service::kSockReconnect = 5;

Service::Service(const char *ip, unsigned short port) {
	_server_port = port;
	if ( ip ) {
		_ip = ip;
	} else {
		_ip = "127.0.0.1";
	}
}


int Service::Init(Embassy *embassy) {

#ifdef WIN32
    InitWinSocket();
#endif

	_diplomat = new Diplomat(_ip.c_str(), _server_port, embassy, kSockReconnect, 64*1024);




	return 0;
}


int Service::Start() {

	if (_diplomat ) {

		_diplomat->start(160 * 1024);
	}

	return 0;
}

int Service::Stop() {

	if (_diplomat) {
		_diplomat->Stop();
	}


	return 0;
}

int Service::Uninit() {

#ifdef WIN32
    UnInitWinSocket();
#endif

	return 0;
}


