/* 
 * filename : sys_time_controller.h
 * author   : zhangjiaqin@zlg.cn
 * date     : 2012-8-14
 * brief    : 
 */

#ifndef SYS_TIME_CONTROLLER_H_
#define SYS_TIME_CONTROLLER_H_

#include "../../third_party/tinyxml/tinyxml.h"

#include "../order/ack_sender.h"

namespace Net {

class SysTimeController : public Command{

public:

	SysTimeController(const char *name): Command(name) {

	}

	virtual void Excute(const DiplomatPtr &diplomat, TiXmlElement *params,
			const char *version, const char *cmd_id) {

	    syslog(LOG_INFO, "net: set system time");
	    TiXmlElement* timeEle = params->FirstChildElement("time");
	    if ( timeEle )
	    {
	        const char* timeTxt = timeEle->GetText();
	        if ( timeTxt )
	        {

	            if ( SetSysTime(timeTxt) ) {
	        		AckSender ack_sender(_name, true, "");
	        		ack_sender.Excute(diplomat, cmd_id);

	            }  else {
	        		AckSender ack_sender(_name, false, "set system time fail");
	        		ack_sender.Excute(diplomat, cmd_id);
	            }
	            return;
	        }
	    }

		AckSender ack_sender(_name, false, "invalid xml format");
		ack_sender.Excute(diplomat, cmd_id);
	    syslog(LOG_ERR, "net: recv invalid set time method");
		return;
	}
protected:
	bool SetSysTime(const char *timeStr)
	{
		if (!timeStr) {
			return false;
		}

		struct tm _tm = { 0 };
		if (6 == sscanf(timeStr, "%d-%d-%d %d:%d:%d", &_tm.tm_year,
						&_tm.tm_mon, &_tm.tm_mday, &_tm.tm_hour, &_tm.tm_min,
						&_tm.tm_sec)) {

			struct timespec ts;
			ts.tv_sec = mktime(&_tm); //timegm(&_tm);;
			ts.tv_nsec = 0;
			//clock_settime(CLOCK_REALTIME, &ts);
			return true;

		}

		return false;
	}

};

};

#endif /* SYS_TIME_CONTROLLER_H_ */


