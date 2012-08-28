/*
 * service.cpp
 *
 *  Created on: 2012-7-6
 *      Author: zhangjiaqin@zlg.cn
 */

#include "service.h"

#include <string.h>

#include "syslog.h"

#include "protocol_def.h"
#include "command/command.h"
#include "command_distributor.h"
#include "command/sys_time_controller.h"

using namespace Net;

const int Service::kSockReconnect = 5;

Service::Service(const char *ip, unsigned short port) {
	_server_port = port;
	if ( ip ) {
		_ip = ip;
	} else {
		_ip = "127.0.0.1";
	}
}


int Service::Init() {


	Order::InitOrder();
	CommandDistributor::RegisterCommand(
			new SysTimeController(PROTOCOL_METHOD_SET_SYS_TIME));

	_diplomat = new Diplomat(_ip.c_str(), _server_port, &_cmd_distributor, kSockReconnect, 64*1024);




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

	return 0;
}


