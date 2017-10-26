#pragma once
#ifndef _GAME_SERVER_SOCKET_H
#define _GAME_SERVER_SOCKET_H

#include "SocketLibSocket.h"
#include <string>

class CGameServerSocket 
	: public SocketLib::DataSocket
{
public:
	// 连接到服务器
	bool connectServer(std::string strAddress, int iPort = 9876);

	// 只是将m_closed设置为ture
	inline void close() {
		m_closed = true;
	}

	// 是否连接关闭
	inline bool closed() {
		return m_closed;
	}

	// 关闭连接
	inline void closeSocket() {
		DataSocket::Close();
	}

	// 接收数据
	virtual void receive();

	// 发送数据
	virtual void send();
	
protected:
	// 初始化
	void initialize();

public:
	CGameServerSocket();
	~CGameServerSocket();

protected:
	// 链接是否关闭
	bool m_closed;
};

#endif


