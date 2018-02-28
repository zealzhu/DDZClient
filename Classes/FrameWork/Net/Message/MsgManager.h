////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/26
// Describe：消息管理器 - 单线程就没必要使用生产者消费者模式了
////////////////////////////////////////////////////////////////////////
#ifndef _MSG_MANAGER_H
#define _MSG_MANAGER_H

#include "Protocol/protobuf.hpp"
#include <google/protobuf/message.h>
#include <deque>
#include <ThreadLib.h>
#include <Singleton.h>

typedef std::shared_ptr<proto::Protobuf> MSG_PTR;
typedef std::deque<MSG_PTR> MSG_DEQUE;
typedef std::deque<MSG_PTR>::iterator MSG_ITER;

// 消息队列最大容量
const unsigned int MAX_DEQUE_MESSAGE_SIZE = 90000;

// ========================================================================
// Type:        CMsgLooper
// Purpose:     消息循环
// ========================================================================
class CMsgLooper {
public:

	// 由cocos2dx Scheduler调用实现与游戏帧率相同的回调函数，与游戏同线程
	void update(float dt);

	// 开启一个新线程用来接收数据
	void beginReceiveThread();

	// 结束接收线程
	void endReceiveThread();

	// 接收线程处理函数
	static void handleReceiveThread(void * pData);

public:
	CMsgLooper();
	~CMsgLooper();

private:
	static bool m_flag;								//线程结束标志
	static ThreadLib::ThreadID m_receiveThread;		//接收线程id
};

class CMsgManager 
	: public CSingleTon<CMsgManager>
{
public:
	virtual bool init();

private:
	CMsgLooper m_looper;
};

// ========================================================================
// Type:        CMsgDeque
// Purpose:     消息队列
// ========================================================================
class CMsgDeque
{
public:
	static CMsgDeque & getInstance() {
		static CMsgDeque instance;
		return instance;
	}

	// 判断接收消息队列是否为空
	bool receivedMsgEmpty();

	// 插入一条消息到接收队列中
	void insertReceivedMsg(MSG_PTR pMsg);

	// 从接收队列中获取一条消息
	MSG_PTR getReceivedMsg();

private:
	CMsgDeque() {
		init();
	}

	// 初始化消息队列
	void init();

protected:

	MSG_DEQUE m_receiveDeq;						  // 接收消息队列

	ThreadLib::Mutex	 m_csReceive;			  // 接收临界区

#ifdef WIN32
	HANDLE               m_hRecDeqFullSemaphore;  // 消息接受队列满信号量
	HANDLE               m_hRecDeqEmptySemphore;  // 消息接受队列空信号量
#endif // WIN32
};

#define MsgManagerIns CMsgManager::getInstance()
#endif // !_MSG_MANAGER_H