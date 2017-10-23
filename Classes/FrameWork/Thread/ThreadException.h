//////////////////////////////////////////////////////////
//// Author:	Zhu 
//// Date:		2017/10/23
//// Describe:	线程异常类
/////////////////////////////////////////////////////////
#pragma once
#ifndef _THREAD_EXCEPTION_H
#define _THREAD_EXCEPTION_H

#include <exception>

// ============================================================================
//  Include Files for the threading libraries
// ============================================================================
#ifdef WIN32                // windows95 and above
#include <windows.h>
#else                       // linux
#include <pthread.h>
#endif

namespace ThreadLib {
	enum Error
	{
		Unspecified,                // 未说明的错误
		InitFailure,                // 未初始化
		CreationFailure             // 创建错误
	};

	class Exception
		: public std::exception
	{
	public:
		// ====================================================================
		// Description: 创建异常
		// ====================================================================
		Exception(Error p_error = Unspecified)
		{
			m_error = p_error;
		}

		// ====================================================================
		// Description: 获取错误类型
		// ====================================================================
		Error GetError() const
		{
			return m_error;
		}

	protected:
		Error m_error;
	};
}

#endif // !_THREAD_EXCEPTION_H
