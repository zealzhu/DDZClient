#include "HallLayer.h"

USING_NS_CC;

CHallLayer::CHallLayer()
{
	this->setTag(ENUM_HALL_LAYER);
}

bool CHallLayer::init()
{
	auto bgNode = Sprite::create("HelloWorld.png");
	this->addChild(bgNode);

	return true;
}
