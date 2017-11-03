#include "Poker.h"
#include <AppFunc.h>

USING_NS_CC;

CPoker::CPoker()
{
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
	switch (m_suit)
	{
	case SUIT_SPADE:
		m_spPoker = Sprite::createWithSpriteFrameName(StringUtils::format("S%d.png", m_pokerNumber));
		break;
	case SUIT_HEART:
		m_spPoker = Sprite::createWithSpriteFrameName(StringUtils::format("H%d.png", m_pokerNumber));
		break;
	case SUIT_DIAMOND:
		m_spPoker = Sprite::createWithSpriteFrameName(StringUtils::format("D%d.png", m_pokerNumber));
		break;
	case SUIT_CLUB:
		m_spPoker = Sprite::createWithSpriteFrameName(StringUtils::format("C%d.png", m_pokerNumber));
		break;
	case SUIT_SMALL_JACKER:
		m_spPoker = Sprite::createWithSpriteFrameName("J14.png");
		break;
	case SUIT_BIG_JACKER:
		m_spPoker = Sprite::createWithSpriteFrameName("J15.png");
		break;
	default:
		break;
	}

	m_spPoker->setScale(0.7f);
	this->addChild(m_spPoker);
	this->setContentSize(m_spPoker->getContentSize());
}
