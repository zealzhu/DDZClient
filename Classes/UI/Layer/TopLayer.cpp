#include "TopLayer.h"

USING_NS_CC;

CTopLayer::CTopLayer()
{
}


CTopLayer::~CTopLayer()
{
}

bool CTopLayer::init()
{
	if(!Layer::init())
		return false;

	createLoadingCircle();

	return true;
}

void CTopLayer::createLoadingCircle()
{
	m_loadingCircle = Sprite::create("loadingImg/loading01.png");
	m_loadingCircle->setVisible(false);

	auto loadingCircleAnimation = Animation::create();
	for (int i = 1; i <= 12; i++) {
		loadingCircleAnimation->addSpriteFrameWithFile(StringUtils::format("loadingImg/loading%02d.png", i));
	}
	auto loadingCircleAnimate = Animate::create(loadingCircleAnimation);
	m_loadingCircle->runAction(RepeatForever::create(loadingCircleAnimate));

	this->addChild(m_loadingCircle);
}

void CTopLayer::showLoadingCircle()
{
	m_loadingCircle->setVisible(true);
}

void CTopLayer::hideLoadingCircle()
{
	m_loadingCircle->setVisible(false);
}
