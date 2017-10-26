#include "MsgManager.h"

bool CMsgManager::receivedMsgEmpty()
{
	bool bFlag = false;

	// 对临界资源进行访问是互斥的
	m_csReceive.Lock();

	if (m_receiveDeq.empty())
		bFlag = true;

	m_csReceive.Unlock();

	return bFlag;
}

void CMsgManager::insertReceivedMsg(MSG_PTR pMsg)
{
	// 消息满则等待
	WaitForSingleObject(m_hRecDeqEmptySemphore, INFINITE);

	m_csReceive.Lock();
	m_receiveDeq.emplace_back(pMsg);
	m_csReceive.Unlock();

	// 满信号量+1
	ReleaseSemaphore(m_hRecDeqFullSemaphore, 1, NULL);
}

MSG_PTR CMsgManager::getReceivedMsg()
{
	// 没有消息则等待
	WaitForSingleObject(m_hRecDeqFullSemaphore, INFINITE);
	MSG_PTR msg = NULL;

	m_csReceive.Lock();
	// 取出消息队列第一条消息
	if (!m_receiveDeq.empty()) {
		MSG_ITER it = m_receiveDeq.begin();
		msg = *it;
		m_receiveDeq.pop_front();
	}
	m_csReceive.Unlock();

	// 空信号量+1
	ReleaseSemaphore(m_hRecDeqEmptySemphore, 1, NULL);

	return msg;
}

void CMsgManager::init()
{
	// 创建接收队列信号量
	m_hRecDeqEmptySemphore = CreateSemaphore(NULL, MAX_DEQUE_MESSAGE_SIZE, MAX_DEQUE_MESSAGE_SIZE, NULL);
	m_hRecDeqFullSemaphore = CreateSemaphore(NULL, 0, MAX_DEQUE_MESSAGE_SIZE, NULL);

	if (NULL == m_hRecDeqEmptySemphore || NULL == m_hRecDeqFullSemaphore)
	{
		//cocos2d::log("semaphore create failed!");
		assert(0);
	}
}