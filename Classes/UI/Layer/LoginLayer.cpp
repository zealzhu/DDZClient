#include "LoginLayer.h"
#include <editor-support/cocostudio/CocoStudio.h>

USING_NS_CC;

LoginLayer::LoginLayer()
{
}

bool LoginLayer::init()
{
	if (!Layer::init())
		return false;

	m_layerGraphNode = CSLoader::createNode("Layer/login.csb");
	this->addChild(m_layerGraphNode);

	return true;
}