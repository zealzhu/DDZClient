//////////////////////////////////////////////////////////
//// Author:	Zhu 
//// Date:		2017/10/23
//// Describe:	初始化管理器
/////////////////////////////////////////////////////////
#pragma once
#ifndef _APP_INIT_MANAGER_H
#define _APP_INIT_MANAGER_H

#include "stdio.h"

class CAppInitManager
{
private:
	CAppInitManager();
	~CAppInitManager();

public:
	static CAppInitManager & getInstance();

public:
	bool initManager();
	void release();

protected:
	bool							m_bInit;
};

#endif

