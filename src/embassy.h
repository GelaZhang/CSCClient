/* 
 * filename : embassy.h
 * author   : sundayman66@gmail.com
 * date     : 2012-7-10
 * brief    : 处理网络事件的接口，如上线，下线，接收数据。
 */

#ifndef EMBASSY_H_
#define EMBASSY_H_
#include "protocol.h"
namespace Net {

class Embassy {

public:
	virtual ~Embassy(){}	
	/**
	*	\brief 产生协议对象,一个连接对应一个协议对象
	*/
	virtual ProtocolPtr BuildProtocol(const DiplomatPtr &diplomat) = 0;
	/**
	*	\brief 启动协议工厂，内部自动调用， 在此做一些初始化操作
	*/
	virtual void StartEmbassy() = 0;
	/**
	*	\brief 停止协议工厂，内部自动调用，在此做一些清除操作
	*/
	virtual void StopEmbassy() = 0;
};

};
#endif /* EMBASSY_H_ */


