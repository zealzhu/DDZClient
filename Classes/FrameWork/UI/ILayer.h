#pragma once
#ifndef _ILAYER_H
#define _ILAYER_H

class ILayer
{
public:
	virtual bool isChildExit(int id)           = 0;
	virtual bool isChildExit(void * target)    = 0;
	virtual bool isChildExit(std::string name) = 0;
};

#endif // _ILAYER_H
