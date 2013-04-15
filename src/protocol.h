/* 
 * filename : protocol.h
 * author   : sundayman
 * date     : 4 15, 2013
 * brief    : Э��ӿڣ�Ҫʵ���µ�Э�飬��Ҫ�̳иýӿڣ�������Э�鹤������� 
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/**
 * \brief embassy service for protocol analysis layer.
 * \	  if you want to change default action of the embassy ,
 * \		 you can inherit class Embassy and then implement your version
 *	Э����󹤳��ӿڣ�ʵ������ӿھͿ���ʵ���ض���Э�鹤��������Э�����Ķ���
 */

#include "ref_counter.h"
#include "diplomat.h"
namespace Net {

class Protocol : public Utility::RefCounter {
public:
	Protocol(const DiplomatPtr &diplomat) {
		_diplomat = diplomat;
	}
	virtual ~Protocol(){}
	virtual int GarrisonDiplomat() = 0;
	virtual int WithdrawDiplomat() = 0;
	virtual int RecvSomething() = 0;
	DiplomatPtr GetDiplomat() {
		return _diplomat;
	}
protected:
	DiplomatPtr _diplomat;
};
typedef Utility::SmartPtr<Protocol> ProtocolPtr;
};
#endif /* PROTOCOL_H_ */
