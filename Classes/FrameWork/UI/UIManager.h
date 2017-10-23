//////////////////////////////////////////////////////////
//// Author:	Zhu 
//// Date:		2017/10/23
//// Describe:	UI管理器
/////////////////////////////////////////////////////////
#pragma once
#ifndef _UI_MANAGER_H
#define _UI_MANAGER_H

#include "Singleton.h"
#include "cocos2d.h"
#include "UIDataDefine.h"

// layer create function
typedef std::function<cocos2d::Layer *()> FUN_LAYER_CREATE;

// id <=> layer
typedef std::map<ENUM_UI_LAYER, FUN_LAYER_CREATE> MAP_LAYER_CREATE_FUNC;

class CUIManager
	: public CSingleTon<CUIManager>
{
	friend class CSingleTon<CUIManager>;
private:
	CUIManager();
	~CUIManager();

public:
	static void initCocosSetting(const std::string strWindowsName);					// 初始化cocos设置

public:
	virtual bool init();
	void initGraphBase();
	cocos2d::Layer * createLayer(ENUM_UI_LAYER layerId);

private:
	MAP_LAYER_CREATE_FUNC m_mapLayerCreateFunc;
	cocos2d::Scene * m_currentScene;
};

#define UIManagerIns CUIManager::getInstance()
#endif


