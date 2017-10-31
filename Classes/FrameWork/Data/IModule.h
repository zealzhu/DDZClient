#ifndef IMODULE_H
#define IMODULE_H

#include <string>
#include <Message/BaseMsg.pb.h>

class IModule
{
public:
	virtual void handle(std::shared_ptr<zhu::SelfDescribingMessage> pTmpMsg) = 0;
	virtual bool init() = 0;
};


#endif