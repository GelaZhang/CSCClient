/* 
 * filename : default_protocol.h
 * author   : sundayman66@gmail.com
 * date     : 2012-7-11
 * brief    : 解析协议头分发命令
 */

#ifndef DEFAULT_PROTOCOL_H_
#define DEFAULT_PROTOCOL_H_

#include <map>
#include <string>

#include "diplomat.h"
#include "command/command.h"
#include "protocol.h"

namespace Net {


#ifndef PATH_MAX

#define PATH_MAX 255

#endif
/*
 * brief 协议头结构
 */
struct ProtocolHeader {

    static const char kProtocolStart[];
    static const char kProtocolVersion[];
    static const char kProtocolPubkey[];
    static const char kProtocolSize[];
    static const char kProtocolCmdId[];
    static const char kProtocolSeparator[];
    static const char kProtocolSeparator2[];

    char protocol[50];
    char version[32];
    char pubkey[32 + 1];
    char commandId[128 + 1];
    int size;
    ProtocolHeader();
    std::string BuildHeader(int size, const char *ack_id);
};

class DefaultProtocol  : public Protocol{

	static const int kMaxLengthOfHeader;
	static const int kMaxLengthOfMethod;

	class DiplomatMemo: public Utility::RefCounter {

	public:
		DiplomatMemo() {

			_header_got = false;
			_header_length = 0;
		}
		ProtocolHeader _header;
		int _header_length;
		bool _header_got;
	};
	typedef Utility::SmartPtr<DiplomatMemo> DiplomatMemoPtr;

	typedef std::map<std::string, CommandPtr> CmdMap;
public:
	DefaultProtocol(const DiplomatPtr &diplomat);
	virtual int GarrisonDiplomat();
	virtual int WithdrawDiplomat();
	/**
	 * \brief  进行命令分发。
	 * \return 返回成功分发的命令数。
	 */
	virtual int RecvSomething();

	static bool RegisterCommand(const CommandPtr &command);
private:

	/**
	 *  \brief 从数据中提取协议头
	 *  \return 成功返回协议头占用的字节数。协议头无效返回-1，
	 *  		协议头还没完整返回0, 否则返回协议头长度
	 */
	int ParseProtocolHeader(const char* data);

	int GetProtocolHeader(
			char *buffer, int &cursor, int length);

	const char *GetProtocolKeyword(
			const char *data, const char *keyword_start,
			        char *keyword, int keyword_len);

	int ParseCommand(char *content, char *version);

private:

	static CmdMap s_cmds;
	DiplomatMemoPtr _memo;
};

};
#endif /* COMMAND_DISTRIBUTOR_H_ */


