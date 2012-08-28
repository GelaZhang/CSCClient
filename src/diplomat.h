/* 
 * filename : diplomat.h
 * author   : zhangjiaqin@zlg.cn
 * date     : 2012-7-9
 * brief    : 
 */

/**
 * \brief diplomat is the socket instance. deal with  socket operation.
 */
#ifndef DIPLOMAT_H_
#define DIPLOMAT_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>

#include "thread.h"

namespace Net {


#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 22
#endif

#ifndef SOCKET
typedef int SOCKET;
#endif

class Embassy;

class Memo : public Utility::RefCounter{

};
typedef Utility::SmartPtr<Memo> MemoPtr;


class Diplomat : public Utility::Thread {

	static const int kTimeWaitSending;
	static const int kTimeWaitRecving;
	static const int kTimeWaitReconnect;

public:
	/**
	 * \param reconnect_frequence 断线重连的频率，单位为秒, 小于等于0表示不自动进行断线重连
	 */
	Diplomat(const char *ip, unsigned short port,
			Embassy *embassy, int reconnect_frequence, int buffer_size);

	virtual ~Diplomat();

	void SetMemo(const MemoPtr &memo) {_memo = memo;}
	MemoPtr &GetMemo() {return _memo;}
public:
	bool ConnectServer();

	/**
	 * \brief 如果设置了自动重连，请先Stop再调用此接口
	 */
	bool DisconnectServer();

	int SendData(const char *send_buf, int len);

	/**
	 * \brief you must drain data out , after you have got .
	 * \param how many data you want to drain, -1表示抽取全部
	 * \return actually length drained
	 * \remark 非线程安全的， 必须在事件响应函数中使用。
	 */
	int Drain(int length);

	/**
	 * \brief check how many data have been got
	 * \remark 非线程安全的， 必须在事件响应函数中使用。
	 */
	int GetPeekAbleLength();

	/**
	 * \brief peek data buffer
	 * \return size of buffer
	 */
	int Peek(char **buffer);

	void Stop();

	/**
	 * \brief 只能在不启动线程接收数据时调用，手动进行数据接收。
	 * \return -1 表示断线， 否则表示接收到的数据。
	 * \remark 只返回次的接收数据，可以调用 Peek, drain进行相应操作
	 */
	int RecvDataManual();
public:
	virtual void run();

private:
	int RecvData();

	void DealWithOffline();

	void Online();

private:
	char _ip[INET_ADDRSTRLEN + 1];
	unsigned short _port;
	SOCKET _sock;
	bool _online;
	int _reconnect_frequence;
	fd_set  _fdSet_r;
	fd_set  _fdSet_w;
	char *_recv_buf;
	int _buffer_size;
	int _cur_length;
	Mutex _mutex;
	Mutex _mutex_send;

	Embassy *_embassy;
	sem_t _working_sem;
	bool _working;

	MemoPtr _memo;
};
typedef Utility::SmartPtr<Diplomat> DiplomatPtr;

};
#endif /* DIPLOMAT_H_ */


