#include "NetManager.h"
#include "SocketLibTypes.h"
#include <cocos2d.h>

using namespace SocketLib;

bool CNetManager::connectGameServer()
{
	return m_gameServerSocket.connectServer("127.0.0.1");
}

bool CNetManager::init()
{
	return true;
}

CNetManager::CNetManager()
{
	
}


CNetManager::~CNetManager()
{
}