#ifndef IMODULE_H
#define IMODULE_H

#include <string>

class IModule
{
public:
	virtual void handle() = 0;
	virtual void init() = 0;
	virtual std::string getModuleName() = 0;
};


#endif