#include "default_factory.h"
#include "default_protocol.h"
#include "protocol_def.h"
#include "command/sys_time_controller.h"
using namespace Net;
ProtocolPtr DefaultFactory::BuildProtocol(const DiplomatPtr &diplomat) {
	return new DefaultProtocol(diplomat);
}
	/**
	*	\brief ����Э�鹤�����ڲ��Զ����ã� �ڴ���һЩ��ʼ������
	*/
void DefaultFactory::StartEmbassy() {
	Order::InitOrder();
	DefaultProtocol::RegisterCommand(
		new SysTimeController(PROTOCOL_METHOD_SET_SYS_TIME));


}
	/**
	*	\brief ֹͣЭ�鹤�����ڲ��Զ����ã��ڴ���һЩ�������
	*/
void DefaultFactory::StopEmbassy() {

}