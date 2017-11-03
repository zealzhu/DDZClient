#pragma once
#ifndef _INPUT_DIALOG_LAYER_H
#define _INPUT_DIALOG_LAYER_H

#include <cocos2d.h>

class CInputDialogLayer
	: public cocos2d::Layer
{
public:
	CREATE_FUNC(CInputDialogLayer);
	virtual bool init();

private:
	cocos2d::Node * m_layerGraphNode;
};

#endif



