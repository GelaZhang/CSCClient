/*
 * Diplomat.cpp
 *
 *  Created on: 2012-7-10
 *      Author: Administrator
 */

#include "diplomat.h"

#include <string.h>
#include <stdlib.h>

#include <time.h>
#include <errno.h>
#include <arpa/inet.h>

#include "syslog.h"

#include "embassy.h"

using namespace Net;

const int kDefaultNetPort = 5053;
const int Diplomat::kTimeWaitSending = 20;
const int Diplomat::kTimeWaitRecving = 20;
const int Diplomat::kTimeWaitReconnect = 5;

Diplomat::Diplomat(const char *ip, unsigned short port, Embassy *embassy,
		int reconnect_frequence, int buffer_size) : Thread("elcd_net") {

	bzero(_ip, sizeof(_ip));
	_port = kDefaultNetPort;
	_sock = -1;
	_online = false;
	_reconnect_frequence = reconnect_frequence;
	FD_ZERO(&_fdSet_r);
	FD_ZERO(&_fdSet_w);
	_cur_length = 0;

	if ( NULL == ip )
		strcpy(_ip, "127.0.0.1");
	strncpy(_ip, ip, sizeof(_ip) - 1);
	_port = port;
	_embassy = embassy;

	sem_init(&_working_sem, 0, 0);
	_working = true;

	if ( buffer_size > 0) {
		_buffer_size = buffer_size;
	} else {
		_buffer_size = 1024;
	}

	_recv_buf = (char*)malloc(_buffer_size + 1);
	bzero(_recv_buf, sizeof(_recv_buf));
}


Diplomat::~Diplomat() {

	FD_ZERO(&_fdSet_r);
	FD_ZERO(&_fdSet_w);
	if(_sock != -1)
	{
		close(_sock);
	}
	sem_destroy(&_working_sem);
	if ( _recv_buf )
		free(_recv_buf);
}


bool Diplomat::ConnectServer()
{
	if(_sock != -1) {

		close(_sock);
		_sock = -1;
	}

	//创建套接字
	if ( (_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))== -1 ) {
		syslog(LOG_ERR, "net: Create socket failed\n");
		return false;
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(sockaddr_in));
	serverAddress.sin_family=AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(_ip);
	serverAddress.sin_port = htons(_port);
	if ( connect(_sock, (sockaddr*)&serverAddress, sizeof(serverAddress))== -1 ) {
		//syslog(LOG_ERR, "net: Build connect failed\n");
		return false;
	}

    Online();
	syslog(LOG_INFO, "net: Connect success\n");

	return true;
}


bool Diplomat::DisconnectServer() {

	if(_sock != -1) {

		close(_sock);
		_sock = -1;
	}
	return true;

}


int Diplomat::SendData(const char *send_buf, int len) {

	if ( NULL == send_buf || len <= 0 ) {
		syslog(LOG_ERR, "net: Input parameters is invalid\n");
		return -1;
	}
	//判断是否连接
	if ( !_online ) {

		syslog(LOG_INFO, "net: not online , can not send anything");
		return -1;
	}

	FD_SET(_sock, &_fdSet_w);
	struct timeval timeout; //阻塞等待时间
	timeout.tv_sec = kTimeWaitSending;
	timeout.tv_usec = 0;
	int ret = select(_sock + 1, NULL, &_fdSet_w, NULL, &timeout);
	//出错
	if ( ret <= 0 ) {
		syslog(LOG_ERR, "net: Network status is busy, fail to send data\n");
		DealWithOffline();
		return -1;
	}

	if ( !FD_ISSET(_sock, &_fdSet_w) ) {

		syslog(LOG_ERR, "net: socket not ready for sending");
		return 0;
	}

	int send_len = 0;

	{
		AutoMutex auto_mutex(_mutex_send);
		send_len = send(_sock, send_buf, len, 0);
	}
	if (send_len < len) {
		if (send_len >= 0) {
			syslog(LOG_ERR, "net: send data less request");
		} else {
			if (EMSGSIZE == errno) {
				syslog(
						LOG_ERR,
						"net: The message is too large to be sent all at once, \
			                                 as the socket requires.");
			} else if (EAGAIN != errno && EWOULDBLOCK != errno) {
				DealWithOffline();
			}
		}
		syslog(LOG_ERR, "net: send data faild");
		return -1;
	}
	return send_len;
}


int Diplomat::Drain(int length) {

	int ret = 0;
	if ( length >= _cur_length || -1 == length) {
		ret = _cur_length;
		_cur_length = 0;
		bzero(_recv_buf, sizeof(_recv_buf));
	} else if ( length > 0 && length < _cur_length ) {
		ret = length;
		memcpy(_recv_buf, _recv_buf + length, _cur_length - length);
		_cur_length -= length;
	}

	return ret;
}


int Diplomat::GetPeekAbleLength() {

	return _cur_length;
}


int Diplomat::Peek(char **buffer) {

	if ( !buffer ) {
		return -1;
	}

	*buffer = _recv_buf;
	return _cur_length;
}


void Diplomat::Stop() {

	_working = false;
	sem_post(&_working_sem);
	getThreadControl().join();
}


int Diplomat::RecvDataManual() {

	return RecvData();;

}


void Diplomat::run() {

	for (;_working;) {

		int ret = RecvData();
		if ( -1 != ret ) {

			if ( ret > 0 && _embassy ) {

				_embassy->RecvSomething(this);
			}
			continue;
		}

		if ( _reconnect_frequence <= 0 || !ConnectServer() ) {

			struct timespec time_sem;
			time_sem.tv_sec = time(NULL) + kTimeWaitReconnect;
            time_sem.tv_nsec = 0;
            sem_timedwait(&_working_sem, &time_sem);
		}

	}
}

int Diplomat::RecvData(){

	if(!_online) {
		return -1;
	}
	FD_SET(_sock, &_fdSet_r);
	//select等待时间
	struct timeval timeout;
	timeout.tv_sec = kTimeWaitRecving;
	timeout.tv_usec = 0;
	int ret = select(_sock + 1, &_fdSet_r, NULL, NULL, &timeout);
	//出错
	if (ret < 0) {

		syslog(LOG_ERR, "net: select failed\n");
		DealWithOffline();
		return -1;
	}
		//等待超时
	if (ret == 0) {
		return 0;
	}

	if (FD_ISSET(_sock, &_fdSet_r)) {

		int recv_len = recv(_sock, _recv_buf + _cur_length, _buffer_size - _cur_length, 0);

		if (recv_len <= 0) {

			if (errno != ETIMEDOUT && errno != EWOULDBLOCK) {

				DealWithOffline();
				syslog(LOG_INFO, "net: receive failed or socket is close\n");
			}
			return -1;
		}

		_cur_length += recv_len;
		return recv_len;
	}
	return -1;
}

void Diplomat::DealWithOffline() {

	syslog(LOG_INFO, "net: network is disconnect\n");

	bool online = true;
	{
		AutoMutex auto_mutex(_mutex);
		online = _online;
		_online = false;
	}

	if ( online && _embassy ) {

		_embassy->WithdrawDiplomat(this);
	}

}

void Diplomat::Online() {

	{
		AutoMutex auto_mutex(_mutex);
		_online = true;
	}

	if ( _embassy ) {
		_embassy->GarrisonDiplomat(this);
	}
}

