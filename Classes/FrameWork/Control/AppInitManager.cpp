#include "AppInitManager.h"
#include "UIManager.h"
#include "NetManager.h"


CAppInitManager::CAppInitManager()
{
	m_bInit = false;
}


CAppInitManager::~CAppInitManager()
{
	release();
}

CAppInitManager & CAppInitManager::getInstance()
{
	static CAppInitManager instance;
	return instance;
}

bool CAppInitManager::initManager()
{
	if (!m_bInit) {
		m_bInit = true;
		if (!UIManagerIns->init()) return false;
		if (!CNetManager::getInstance()->connectGameServer()) return false;
	}
	return m_bInit;
}

void CAppInitManager::release()
{
	UIManagerIns->release();
}