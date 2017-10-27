#include "GameServerSocket.h"
#include "SocketLibTypes.h"
#include "cocos2d.h"
#include "Protocol/Code.h"
#include <cocos2d.h>
#include "Protocol/Code.h"
#include "Message/MsgManager.h"

using namespace SocketLib;

bool CGameServerSocket::connectServer(std::string strAddress, int iPort)
{
	ipaddress ipaddrr = inet_addr(strAddress.c_str());
	try {
		cocos2d::log("Try to connect to %s", strAddress.c_str());
		Connect(ipaddrr, iPort);
	}
	catch (Exception e) {
		cocos2d::log("Connect to %s failed! [%s]", strAddress.c_str(), e.PrintError().c_str());
		return false;
	}

	cocos2d::log("connect to %s successes.", strAddress.c_str());
	return true;
}

void CGameServerSocket::receive()
{
	ZeroMemory(headerBuf, 4);
	int bytecount = 0;
	int total = 0;
	// 读取头部数据包大小
	while (total < 4 && (bytecount = DataSocket::Receive(headerBuf + bytecount, 4 - bytecount)) > 0)
	{
		total += bytecount;
	}
	if (bytecount == -1)
	{
		cocos2d::log("receive bytecount == -1");
		throw Exception(GetError());
	}

	// 解码
	MessagePtr messagePtr = ProtocolCode::decode(this, ProtocolCode::readHdr(headerBuf));

	// 插入消息队列中
	CMsgDeque::getInstance().insertReceivedMsg(messagePtr);
}

int CGameServerSocket::send(const google::protobuf::Message & message)
{
	// 编码
	int iBufSize;
	const char * encode = ProtocolCode::encode(message, iBufSize);
	
	// 发送数据
	int iByteCount;
	if ((iByteCount = DataSocket::Send(encode, iBufSize)) == -1) {
		cocos2d::log("error sending data %d", errno);
	}
	return iByteCount;
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
