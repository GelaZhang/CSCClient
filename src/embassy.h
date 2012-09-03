/* 
 * filename : embassy.h
 * author   : sundayman66@gmail.com
 * date     : 2012-7-10
 * brief    : 处理网络事件的接口，如上线，下线，接收数据。
 */

#ifndef EMBASSY_H_
#define EMBASSY_H_

namespace Net {

class Embassy {

public:
	virtual ~Embassy(){}
	virtual int GarrisonDiplomat(const DiplomatPtr &diplomat) = 0;
	virtual int WithdrawDiplomat(const DiplomatPtr &diplomat) = 0;
	virtual int RecvSomething(const DiplomatPtr &diplomat) = 0;
};

};
#endif /* EMBASSY_H_ */


