#include "Poker.h"
#include <AppFunc.h>

USING_NS_CC;

std::string CPoker::getPokerFileName(int number, PokerSuit suit)
{
	std::string fileName;
	char buf[20];

	switch (suit)
	{
	case SUIT_SPADE:
		sprintf(buf, "S%d.png", number);
		fileName = buf;
		break;
	case SUIT_HEART:
		sprintf(buf, "H%d.png", number);
		fileName = buf;
		break;
	case SUIT_DIAMOND:
		sprintf(buf, "D%d.png", number);
		fileName = buf;
		break;
	case SUIT_CLUB:
		sprintf(buf, "C%d.png", number);
		fileName = buf;
		break;
	case SUIT_SMALL_JACKER:
		fileName = "J14.png";
		break;
	case SUIT_BIG_JACKER:
		fileName = "J15.png";
		break;
	default:
		break;
	}

	return fileName;
}

CPoker::CPoker()
{
	m_choose = false;
}

Node * CPoker::create(int number, int value, PokerSuit suit)
{
	CPoker *pRet = new(std::nothrow) CPoker();
	if (pRet && pRet->init(number, value, suit))
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

bool CPoker::init(int number, int value, PokerSuit suit)
{
	if(!Node::init())
		return false;

	m_pokerNumber = number;
	m_value = value;
	m_suit = suit;

	initPoker();

	return true;
}

void CPoker::initPoker()
{
	m_spPoker = Sprite::createWithSpriteFrameName(getPokerFileName(m_pokerNumber, m_suit));

	m_spPoker->setAnchorPoint(Vec2(0, 0));
	//m_spPoker->setScale(0.7f);
	this->addChild(m_spPoker);
	this->setContentSize(m_spPoker->getContentSize());
}

void CPoker::onEnter()
{
	Node::onEnter();
	/*auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(CPoker::onTouchBegin, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(CPoker::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(CPoker::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(CPoker::onTouchCancelled, this);
	GEventDispatch->addEventListenerWithSceneGraphPriority(touchListener, this);*/
}

void CPoker::onExit()
{
	//GEventDispatch->removeEventListenersForTarget(this);
	Node::onExit();
}

bool CPoker::onTouchBegin(cocos2d::Touch * touch, cocos2d::Event * event)
{
	auto touchLocation = touch->getLocation();
	// ×ª»»×ø±ê
	auto parentLocation = this->getParent()->convertToNodeSpace(touchLocation);
	if (this->getBoundingBox().containsPoint(parentLocation))
		return true;
	else
		return false;
}

void CPoker::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{

}

void CPoker::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	auto touchLocation = touch->getLocation();
	if (this->getBoundingBox().containsPoint(touchLocation)) {	
		
	}
}

void CPoker::onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event * event)
{
}

void CPoker::updatePositionY()
{
	if (m_choose)
		this->setPositionY(this->getPositionY() + 30.0f);
	else
		this->setPositionY(0);
}

bool CPoker::getChoose()
{
	return m_choose;
}

void CPoker::setChoose(bool isChoose)
{
	m_choose = isChoose;
}

int CPoker::getValue()
{
	return m_value;
}

int CPoker::getSuit()
{
	return m_suit;
}

int CPoker::getNumber()
{
	return m_pokerNumber;
}

void CPoker::click()
{
	m_choose = !m_choose;
	updatePositionY();
}
