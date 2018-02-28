#ifndef _CARD_H
#define _CARD_H

#include <stdint.h>
#include <vector>

struct Card;

enum Suit {
    kSpade = 0,// 黑桃
    kHeart = 1,// 红心
    kDiamond = 2,// 方块
    kClub = 3,// 梅花
    kJocker = 4,
};

enum PlayType {
    kPlayError = -1,
    kPlayDoubleJacker = 0,//天王炸：即双王（大王和小王），最大的牌
    kPlayBomb = 1,//炸弹：四张同数值牌
    kPlaySingle = 2,//单牌：单个牌
    kPlayDoubleSame = 3,//对牌：数值相同的两张牌
    kPlayThreeSame = 4,//三张牌：数值相同的三张牌
    kPlayThreeSameWithOne = 5,//三带一单
    kPlayThreeSameWithTwo = 6,//三带一对
    kPlaySingleStraight = 7,//单顺：五张或更多的连续单牌。不包括 2 点和双王
    kPlayDoubleStraight = 8,//双顺：三对或更多的连续对牌。不包括 2 点和双王
    kPlayBombWithSingle = 9,//四带二单
    kPlayBombWithDouble = 10,//四带二双
    kNoPlay = 11,
    kPlayDoubleThreeSame = 12,//两对飞机不带
    kPlayDoubleThreeSameWithSingle = 13,//两对飞机带单牌
    kPlayDoubleThreeSameWithDouble = 14,//两对飞机带双牌
    kPlayThreeThreeSame = 15,//三对飞机不带
    kPlayThreeThreeSameWithSingle = 16,//三对飞机带单牌
    kPlayThreeThreeSameWithDouble = 17,//三对飞机带双牌
    kPlayFourThreeSame = 18,//四对飞机不带
    kPlayFourThreeSameWithSingle = 19,//四对飞机带单牌
    kPlayFourThreeSameWithDouble = 20,
    kPlayFiveThreeSame = 21,//五对飞机不带
    kPlayFiveThreeSameWithSingle = 22,
};

enum CombRet
{
    kCombFail = 0,
    kCombMustPlay = 1,
    kCombChooseError = 2,
};

struct CombType
{
public:
    CombType() : play_index(0), type(kNoPlay) {}

    int8_t play_index;
    PlayType type;
    std::vector< Card > cards;
};

struct Card
{
public:
    Card() {}
    Card(int i) {
        id = i;
        suit = (Suit)(i / 13);
        point = i % 13 + 1;
        InitGrade();
    }

    void InitGrade()
    {
        if(suit == kJocker) {
            if(point == 1) {
                grade = 15;
            }
            else {
                grade = 14;
            }
        }
        else {
            if(point == 1) {
                grade = 12;
            }
            else if(point == 2) {
                grade = 13;
            }
            else {
                grade = point - 2;
            }
        }
    }

    int8_t id;
    int8_t point;
    int8_t grade;
    Suit suit;
};

#endif // _CARD_H
