#include "Poker.h"
#include <AppFunc.h>

USING_NS_CC;

std::string CPoker::getPokerFileName(int number, Suit suit)
{
	std::string fileName;
	char buf[20];

	switch (suit)
	{
	case kSpade:
		sprintf(buf, "S%d.png", number);
		fileName = buf;
		break;
	case kHeart:
		sprintf(buf, "H%d.png", number);
		fileName = buf;
		break;
	case kDiamond:
		sprintf(buf, "D%d.png", number);
		fileName = buf;
		break;
	case kClub:
		sprintf(buf, "C%d.png", number);
		fileName = buf;
		break;
	case kJocker:
		if(number == 2)
			fileName = "J14.png";
		else
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
	m_touchFlag = false;
}

Node * CPoker::create(int id, int number, int value, Suit suit)
{
	CPoker *pRet = new(std::nothrow) CPoker();
	if (pRet && pRet->init(id, number, value, suit))
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

bool CPoker::init(int id, int number, int value, Suit suit)
{
	if(!Node::init())
		return false;

	m_pokerNumber = number;
	m_value = value;
	m_suit = suit;
	m_id = id;

	initPoker();

	return true;
}

void CPoker::initPoker()
{
	m_spPoker = Sprite::createWithSpriteFrameName(getPokerFileName(m_pokerNumber, m_suit));

	m_spPoker->setAnchorPoint(Vec2(0, 0));
	m_spPoker->setScale(0.7f);
	this->addChild(m_spPoker);
	this->setContentSize(m_spPoker->getBoundingBox().size);
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

bool CPoker::getTouchFlag()
{
	return m_touchFlag;
}

void CPoker::setTouchFlag(bool touch)
{
	m_touchFlag = touch;
}

void CPoker::updateColor()
{
	if (m_touchFlag)
		m_spPoker->setColor(Color3B::GRAY);
	else
		m_spPoker->setColor(Color3B::WHITE);
}
