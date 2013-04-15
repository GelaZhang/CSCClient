#include "default_factory.h"
#include "default_protocol.h"
#include "protocol_def.h"
#include "command/sys_time_controller.h"
using namespace Net;
ProtocolPtr DefaultFactory::BuildProtocol(const DiplomatPtr &diplomat) {
	return new DefaultProtocol(diplomat);
}
	/**
	*	\brief 启动协议工厂，内部自动调用， 在此做一些初始化操作
	*/
void DefaultFactory::StartEmbassy() {
	Order::InitOrder();
	DefaultProtocol::RegisterCommand(
		new SysTimeController(PROTOCOL_METHOD_SET_SYS_TIME));


}
	/**
	*	\brief 停止协议工厂，内部自动调用，在此做一些清除操作
	*/
void DefaultFactory::StopEmbassy() {

}