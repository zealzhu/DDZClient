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

public:
	void createDialog();
	void showDialog(std::string strTitle, std::string strMessage, std::function<void(Ref*)> callBack = NULL);	//不能显示中文蛋疼
	void hideDialog();

public:
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);

private:
	cocos2d::Sprite * m_loadingCircle;
	cocos2d::Node * m_dialog;
};

#endif


