#pragma once
#ifndef _MODULE_MANAGER_H
#define _MODULE_MANAGER_H

#include "IModule.h"
#include <map>
#include <BaseMsg.pb.h>
#include <Singleton.h>

typedef std::map<std::string, std::shared_ptr<IModule>> MODULE_MAP;
typedef std::map<std::string, std::shared_ptr<IModule>>::iterator MODULE_ITER;
typedef std::shared_ptr<google::protobuf::Message> MSG_PTR;
typedef std::shared_ptr<IModule> MODULE_PTR;

class CModuleManager
	: public CSingleTon<CModuleManager>
{
	friend class CSingleTon<CModuleManager>;
public:
	// 添加模块
	void addModule(std::string strModuleName, MODULE_PTR pModule) {
		m_moduleMap[strModuleName] = pModule;
	}

	// 移除模块
	void removeModule(MODULE_ITER it) {
		m_moduleMap.erase(it);
	}

	// 移除所有模块
	void removeAllModule() {
		while (!m_moduleMap.empty())
		{
			MODULE_ITER it = m_moduleMap.begin();
			removeModule(it);
		}
	}

	// 获取指定模块
	MODULE_PTR getModule(const char * strModuleName) {
		return m_moduleMap[strModuleName];
	}

	// 将模块名解析出来 模块名格式 "xxx.xx.xx" 最后一个就是模块名
	std::string getModuleName(std::string strTypeName) {
		int index = strTypeName.find_last_of(".");
		std::string moduleName = strTypeName;
		if (index != std::string::npos)
		{
			moduleName = strTypeName.substr(0, index);
		}

		return moduleName;
	}

	// 初始化模块
	virtual bool init() {
		return true;
	}

	// 将消息分发给各个模块
	void dispatchMsg() {

	}

private:
	CModuleManager() {}

private:
	MODULE_MAP m_moduleMap;
};

#endif