#ifndef _POKER_H
#define _POKER_H

#include <cocos2d.h>
#include <string>
#include <card.hpp>

class CPoker
	: public cocos2d::Node
{
public:
	static std::string getPokerFileName( int number, Suit suit);

public:
	CPoker();
	static cocos2d::Node * create(int id, int number, int value, Suit suit);
	bool init(int id, int number, int value, Suit suit);
	void initPoker();

public:
	void updatePositionY();
	bool getChoose();
	void setChoose(bool isChoose);
	int getValue();
	int getSuit();
	int getNumber();
	void click();
	int getCid() { return m_id; }
	
	bool getTouchFlag();
	void setTouchFlag(bool touch);
	void updateColor();
	
private:
	cocos2d::Sprite * m_spPoker;		// 扑克精灵
	int m_pokerNumber;					// 牌面数
	int m_value;						// 比值
	Suit m_suit;					// 花色
	bool m_choose;						// 是否选中
	bool m_touchFlag;					// 触摸标记
	int m_id;
};


#endif