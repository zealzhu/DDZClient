#include "GameServerSocket.h"
#include "SocketLibTypes.h"
#include "cocos2d.h"

using namespace SocketLib;

bool CGameServerSocket::connectServer(std::string strAddress, int iPort)
{
	ipaddress ipaddrr = inet_addr(strAddress.c_str());
	try {
		cocos2d::log("tru tp connect to %s", strAddress.c_str());
		Connect(ipaddrr, iPort);
	}
	catch (Exception e) {
		cocos2d::log("connect to %s failed! [%s]", strAddress.c_str(), e.PrintError().c_str());
		return false;
	}

	cocos2d::log("connect to %s successed.", strAddress.c_str());
	return true;
}

void CGameServerSocket::receive()
{
}

void CGameServerSocket::send()
{
}

void CGameServerSocket::initialize()
{
	DataSocket::m_connected = false;
}

CGameServerSocket::CGameServerSocket()
{
	initialize();
}


CGameServerSocket::~CGameServerSocket()
{
}
