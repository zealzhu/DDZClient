////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/26
// Describe：消息管理器 - 单线程就没必要使用生产者消费者模式了
////////////////////////////////////////////////////////////////////////

#include <google/protobuf/message.h>
#include <deque>
#include <ThreadLibMutex.h>

typedef std::shared_ptr<google::protobuf::Message> MSG_PTR;
typedef std::deque<MSG_PTR> MSG_DEQUE;
typedef std::deque<MSG_PTR>::iterator MSG_ITER;

// 消息队列最大容量
const unsigned int MAX_DEQUE_MESSAGE_SIZE = 90000;

class CMsgManager
{
public:
	// 获取实例
	static CMsgManager & getInstance() {
		static CMsgManager instance;
		return instance;
	}

	// 判断接收消息队列是否为空
	bool receivedMsgEmpty();

	// 插入一条消息到接收队列中
	void insertReceivedMsg(MSG_PTR pMsg);

	// 从接收队列中获取一条消息
	MSG_PTR getReceivedMsg();

private:
	CMsgManager() {
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

