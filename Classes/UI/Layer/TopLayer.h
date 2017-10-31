#pragma once
#ifndef _TOP_LAYER_H
#define _TOP_LAYER_H

#include <BaseLayer.h>

class CTopLayer
	: public CBaseLayer<CTopLayer>
{
public:
	CTopLayer();
	~CTopLayer();
	virtual bool init();

public:
	void createLoadingCircle();
	void showLoadingCircle();
	void hideLoadingCircle();

private:
	cocos2d::Sprite * m_loadingCircle;
};

#endif


