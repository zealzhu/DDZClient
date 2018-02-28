#include "GameServerSocket.h"
#include "SocketLibTypes.h"
#include "cocos2d.h"
#include "Protocol/protocol.hpp"
#include "Protocol/protobuf.hpp"
#include <cocos2d.h>
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
	ZeroMemory(headerBuf, sizeof(uint16_t));
	int bytecount = 0;
	int total = 0;
	// 读取头部数据包大小
	while (total < sizeof(uint16_t) && (bytecount = DataSocket::Receive(headerBuf + bytecount, sizeof(uint16_t) - bytecount)) > 0)
	{
		total += bytecount;
	}
	if (bytecount == -1)
	{
		cocos2d::log("receive bytecount == -1");
		throw Exception(GetError());
	}

	// read head
	proto::ByteBuffer buffer;
	buffer.Append(headerBuf, sizeof(uint16_t));
	uint16_t size = buffer.Read< uint16_t >();
	
	char * pBuf = new char[size + sizeof(uint16_t)];
	ZeroMemory(pBuf, size + sizeof(uint16_t));
	memcpy(pBuf, headerBuf, sizeof(uint16_t));
	unsigned uiTotal = 0;
	unsigned uiByteCount = 0;
	while (uiTotal < size && (uiByteCount = DataSocket::Receive(pBuf + sizeof(uint16_t) + uiByteCount, size - uiByteCount)) > 0)
	{
		if (uiByteCount == -1)
		{
			throw SocketLib::Exception(SocketLib::GetError());
		}

		uiTotal += uiByteCount;
	}
	buffer.Append(pBuf, size + sizeof(uint16_t));
	// 消息解包
	MSG_PTR protobuf(new proto::Protobuf);
	proto::Decode(*protobuf, buffer, false); // 不加密

	// 插入消息队列中
	CMsgDeque::getInstance().insertReceivedMsg(protobuf);
}

int CGameServerSocket::send(int32_t id, const google::protobuf::Message & message)
{
	proto::Protobuf protobuf;
	proto::ByteBuffer buffer;
	protobuf.SetId(id);
	protobuf << message;
	proto::Encode(protobuf, buffer, false); // 不加密

	// 发送数据
	int iByteCount;
	if ((iByteCount = DataSocket::Send((const char *)buffer.Contents(), buffer.Size())) == -1) {
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
