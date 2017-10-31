#pragma once
#ifndef _LOGIN_LAYER_H
#define _LOGIN_LAYER_H

#include "BaseLayer.h"

class LoginLayer
	: public CBaseLayer<LoginLayer>
{
public:
	LoginLayer();
	virtual bool init();
	void loginCallback(cocos2d::Ref * target);

private:
	cocos2d::Node * m_layerGraphNode;
};

#endif


