#pragma once
#ifndef _DATA_CENTER_H
#define _DATA_CENTER_H

#include <Singleton.h>
#include "IModule.h"
#include <Protocol/Code.h>

class CDataCenter
	: public IModule
{
public:
	virtual bool init();
	virtual void handle(std::shared_ptr<zhu::SelfDescribingMessage> pTmpMsg);

public:
	void dealWithLoginResponse(MessagePtr pMsg);			// ´¦ÀíµÇÂ¼ÏìÓ¦ 

public:
	CDataCenter();
	~CDataCenter();
	
private:
	int m_userId;
	std::string m_strAccount;
};

#endif


