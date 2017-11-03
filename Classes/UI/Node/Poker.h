#ifndef _POKER_H
#define _POKER_H

#include <cocos2d.h>

enum PokerSuit
{
	SUIT_SPADE,				// 黑桃
	SUIT_HEART,				// 红心
	SUIT_DIAMOND,			// 方块
	SUIT_CLUB,				// 梅花
	SUIT_SMALL_JACKER,		// 小王
	SUIT_BIG_JACKER,		// 大王 
};

class CPoker
	: public cocos2d::Node
{
public:
	CPoker();
	static cocos2d::Node * create(int number, int value, PokerSuit suit);
	bool init(int number, int value, PokerSuit suit);
	void initPoker();

private:
	cocos2d::Sprite * m_spPoker;		// 扑克精灵
	int m_pokerNumber;					// 牌面数
	int m_value;						// 比值
	PokerSuit m_suit;					// 花色
};


#endif