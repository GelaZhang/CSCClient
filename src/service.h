/* 
 * filename : service.h
 * author   : sundayman66@gmail.com
 * date     : 2012-7-6
 * brief    : 
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include <string>
#ifndef WIN32
#include <sys/socket.h>
#endif
#include "ref_counter.h"

#include "embassy.h"
#include "diplomat.h"

namespace Net {
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 22
#endif
class Service {

	static const int kSockReconnect;
public:
	Service(const char *ip, unsigned short port);
	~Service(){}
    /**
	 *  \brief 初始化服务
	 *  \return E_OK 返回成功
	 */
	int Init(Embassy *embassy);

	/**
	 *  \brief 启动socket服务
	 *  \return E_OK 启动成功
	 */
	int Start();

	/**
	 *  \brief 停止socket服务
	 *  \return E_OK 成功停止
	 */
	int Stop();

	/**
	 * \brief 清除资源
	 */
	int Uninit();

private:

	DiplomatPtr _diplomat; //一个diplomat 对应一个command distributor;

	unsigned int _server_port;
	std::string _ip;
	Embassy *_embassy;

private:

};
};
#endif /* SERVICE_H_ */


