#pragma once
#ifndef _MODULE_MANAGER_H
#define _MODULE_MANAGER_H

#include "IModule.h"
#include <map>
#include <BaseMsg.pb.h>
#include <Singleton.h>
#include "DataCenter.h"

typedef std::map<std::string, IModule*> MODULE_MAP;
typedef std::map<std::string, IModule*>::iterator MODULE_ITER;
typedef std::shared_ptr<google::protobuf::Message> MSG_PTR;

class CModuleManager
	: public CSingleTon<CModuleManager>
{
	friend class CSingleTon<CModuleManager>;
public:
	// 添加模块
	void addModule(std::string strModuleName, IModule * pModule) {
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
	IModule * getModule(const char * strModuleName) {
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
		auto pDataCenter = CDataCenter::getInstance();
		pDataCenter->init();
		addModule("data", pDataCenter);

		return true;
	}

	// 将消息分发给各个模块
	void dispatchMsg(MSG_PTR pMsg) {
		std::shared_ptr<zhu::SelfDescribingMessage> pTmpMsg = std::dynamic_pointer_cast<zhu::SelfDescribingMessage>(pMsg);
		if (NULL == pTmpMsg.get())
		{
			//log("dynamic_cast msg failed");
			return;
		}

		auto it = m_moduleMap.begin();

		while (it != m_moduleMap.end()) {
			it->second->handle(pTmpMsg);
			it++;
		}
	}

private:
	CModuleManager() {}

private:
	MODULE_MAP m_moduleMap;
};

#define ModuleManagerIns CModuleManager::getInstance()
#endif