#pragma once
#ifndef _BASE_LAYER_H
#define _BASE_LAYER_H

#include <cocos2d.h>
#include "ILayer.h"
#include "UIDataDefine.h"

template<class T>
class CBaseLayer
	: public cocos2d::Layer, public ILayer
{
public:
	static T * create();
	virtual bool isChildExit(int id);
	virtual bool isChildExit(void * target);
	virtual bool isChildExit(std::string name);
};

template<class T>
T * CBaseLayer<T>::create()
{
	T *pRet = new(std::nothrow) T();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

template<class T>
bool CBaseLayer<T>::isChildExit(int id)
{
	return this->getChildByTag(id) == NULL;
}

template<class T>
bool CBaseLayer<T>::isChildExit(void * target)
{
	for (auto child : this->getChildren())
	{
		if (target == child)
			return true;
	}
	return false;
}

template<class T>
bool CBaseLayer<T>::isChildExit(std::string name)
{
	return this->getChildByName(name) == NULL;
}
#endif 
