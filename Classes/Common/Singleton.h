//////////////////////////////////////////////////////////
//// Author:	Zhu 
//// Date:		2017/10/23
//// Describe:	单例模板
/////////////////////////////////////////////////////////
#pragma once
#ifndef _SINGLETON_H
#define _SINGLETON_H

// 类定义
template<class T>
class CSingleTon
{
public:
	static inline T * getInstance();		// 获取单例
	static inline void release();			// 释放单例

public:
	virtual bool init() = 0;

protected:
	CSingleTon(void) {};
	~CSingleTon(void) {};

protected:
	static T * m_pInstance;
};

// 实现
template<class T>
T * CSingleTon<T>::m_pInstance = 0;

template<class T>
inline T * CSingleTon<T>::getInstance()
{
	if (!m_pInstance) {
		m_pInstance = new T();
	}
	return m_pInstance;
}

template<class T>
inline void CSingleTon<T>::release()
{
	if (m_pInstance) {
		delete m_pInstance;
		m_pInstance = 0;
	}
}

#endif