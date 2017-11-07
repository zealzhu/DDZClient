#ifndef _POKER_H
#define _POKER_H

#include <cocos2d.h>
#include <string>

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
	static std::string getPokerFileName(int number, PokerSuit suit);

public:
	CPoker();
	static cocos2d::Node * create(int number, int value, PokerSuit suit);
	bool init(int number, int value, PokerSuit suit);
	void initPoker();

public:
	void updatePositionY();
	bool getChoose();
	void setChoose(bool isChoose);
	int getValue();
	int getSuit();
	int getNumber();
	void click();
	
	bool getTouchFlag();
	void setTouchFlag(bool touch);
	void updateColor();
	
private:
	cocos2d::Sprite * m_spPoker;		// 扑克精灵
	int m_pokerNumber;					// 牌面数
	int m_value;						// 比值
	PokerSuit m_suit;					// 花色
	bool m_choose;						// 是否选中
	bool m_touchFlag;					// 触摸标记
};


#endif