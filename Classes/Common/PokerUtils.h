#ifndef _POKER_UTILS_H
#define _POKER_UTILS_H

#include <vector>
#include <UI/Node/Poker.h>

using namespace std;

class CPokerUtils {
public:
	// 三带1
	static bool IsThreeWithOne(vector<CPoker *>& vecPoker) {
		if (vecPoker[0]->getValue() == vecPoker[1]->getValue() &&
			vecPoker[0]->getValue() == vecPoker[2]->getValue())
			return true;
		else if (vecPoker[0]->getValue() == vecPoker[2]->getValue() &&
			vecPoker[0]->getValue() == vecPoker[3]->getValue())
			return true;
		else if (vecPoker[1]->getValue() == vecPoker[2]->getValue() &&
			vecPoker[1]->getValue() == vecPoker[3]->getValue())
			return true;
		else
			return false;
	}

	// 三带2
	static bool IsTreeWithTwo(vector<CPoker *>& vecPoker) {
		if (vecPoker[0]->getValue() == vecPoker[1]->getValue() &&
			vecPoker[0]->getValue() == vecPoker[2]->getValue() &&
			vecPoker[3]->getValue() == vecPoker[4]->getValue())
			return true;
		else if (vecPoker[2]->getValue() == vecPoker[3]->getValue() &&
			vecPoker[2]->getValue() == vecPoker[4]->getValue() &&
			vecPoker[0]->getValue() == vecPoker[1]->getValue())
			return true;

		return false;
	}

	// 顺子
	static bool IsSingleStraight(vector<CPoker *>& vecPoker) {
		if (vecPoker.size() > 12 || vecPoker.size() < 5)
			return false;

		// A的值是12，超过这个值就不是顺子
		if (vecPoker[0]->getValue() > 12)
			return false;

		// 循环遍历
		int iLastValue = vecPoker[0]->getValue();
		int iCurrentValue;
		for (int i = 1; i < vecPoker.size(); i++) {
			iCurrentValue = vecPoker[i]->getValue();
			//值不连续就不是顺子
			if ((iCurrentValue + 1) != iLastValue)
				return false;
			iLastValue = iCurrentValue;
		}
		return true;
	}

	// 连对
	static bool IsDoubleStraight(vector<CPoker *>& vecPoker) {
		int iSize = vecPoker.size();
		if (iSize < 6 || iSize % 2 != 0)
			return false;

		// A的值是12，超过这个值就不是连对
		if (vecPoker[0]->getValue() > 12)
			return false;

		// 循环遍历
		int iLastValue = vecPoker[0]->getValue();
		int iCurrentValue;
		for (int i = 1; i < vecPoker.size(); i++) {
			iCurrentValue = vecPoker[i]->getValue();
			// i是奇数要和上一张比是不是相同牌
			if ((i + 1) % 2 == 0 && iCurrentValue != iLastValue)
				return false;

			// i是偶数要比较与上一张牌数值是不是相差1
			if (i % 2 == 0 && (iCurrentValue + 1) != iLastValue)
				return false;

			iLastValue = iCurrentValue;
		}
		return true;
	}

	// 炸弹带单牌
	static bool IsBoomWithSingle(vector<CPoker *>& vecPoker) {
		int	iSize = vecPoker.size();
		if (iSize != 6)
			return false;
		// 中间两张一定相同
		if (vecPoker[2]->getValue() != vecPoker[3]->getValue())
			return false;

		// 三种情况成立
		if ((vecPoker[0]->getValue() == vecPoker[1]->getValue() &&
			vecPoker[0]->getValue() == vecPoker[2]->getValue()) ||
			(vecPoker[1]->getValue() == vecPoker[4]->getValue() &&
				vecPoker[1]->getValue() == vecPoker[2]->getValue()) ||
				(vecPoker[4]->getValue() == vecPoker[5]->getValue() &&
					vecPoker[4]->getValue() == vecPoker[2]->getValue()))
			return true;

		return false;
	}

	// 炸弹带对子
	static bool IsBoomWithDouble(vector<CPoker *>& vecPoker) {
		int iSize = vecPoker.size();
		if (iSize != 8)
			return false;
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 6; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 2; i < 8; i++)
			temp2.push_back(vecPoker[i]);;

		if (IsBoomWithSingle(temp1)) {
			if (vecPoker[7]->getValue() != vecPoker[6]->getValue())
				return false;
			if (vecPoker[0]->getValue() != vecPoker[1]->getValue() &&
				vecPoker[4]->getValue() != vecPoker[5]->getValue())
				return false;
		}
		else if (IsBoomWithSingle(temp2)) {
			if (vecPoker[0]->getValue() != vecPoker[1]->getValue())
				return false;
			if (vecPoker[2]->getValue() != vecPoker[3]->getValue() &&
				vecPoker[6]->getValue() != vecPoker[7]->getValue())
				return false;
		}

		return true;
	}

	// 两飞机不带
	static bool IsTwoPlane(vector<CPoker *>& vecPoker) {
		int iSize = vecPoker.size();

		//头尾三张都相同是
		if (vecPoker[0]->getValue() == vecPoker[1]->getValue() &&
			vecPoker[0]->getValue() == vecPoker[2]->getValue() &&
			vecPoker[3]->getValue() == vecPoker[4]->getValue() &&
			vecPoker[3]->getValue() == vecPoker[5]->getValue())
			return true;

		return false;
	}

	// 三飞机不带 9
	static bool IsThreePlane(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 6; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 3; i < 9; i++)
			temp2.push_back(vecPoker[i]);;
		// 头尾都是双飞机
		if (IsTwoPlane(temp1) && IsTwoPlane(temp2))
			return true;
		return false;
	}

	// 四飞机不带 12
	static bool IsFourPlane(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 9; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 3; i < 12; i++)
			temp2.push_back(vecPoker[i]);;
		// 头尾都是三飞机
		if (IsThreePlane(temp1) && IsThreePlane(temp2))
			return true;
		return false;
	}

	// 五飞机不带 15
	static bool IsFivePlane(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 12; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 3; i < 15; i++)
			temp2.push_back(vecPoker[i]);;
		// 头尾都是四飞机
		if (IsFourPlane(temp1) && IsFourPlane(temp2))
			return true;
		return false;
	}

	// 两飞机带单 8
	static bool IsTwoPlaneWithSingle(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 6; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 3; i < 8; i++)
			temp2.push_back(vecPoker[i]);;
		if (IsTwoPlane(temp1) &&
			vecPoker[6]->getValue() != vecPoker[7]->getValue())
			return true;
		else if (IsTwoPlane(temp2) &&
			vecPoker[0]->getValue() != vecPoker[1]->getValue())
			return true;

		return false;
	}

	// 两飞机带对 10
	static bool IsTwoPlaneWithDouble(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 6; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 4; i < 9; i++)
			temp2.push_back(vecPoker[i]);;
		if (IsTwoPlane(temp1) &&
			vecPoker[6]->getValue() == vecPoker[7]->getValue() &&
			vecPoker[8]->getValue() == vecPoker[9]->getValue())
			return true;
		else if (IsTwoPlane(temp2) &&
			vecPoker[0]->getValue() == vecPoker[1]->getValue() &&
			vecPoker[2]->getValue() == vecPoker[3]->getValue())
			return true;

		return false;
	}

	// 三飞机带单 12
	static bool IsThreePlaneWithSingle(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 9; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 3; i < 12; i++)
			temp2.push_back(vecPoker[i]);;
		if (IsThreePlane(temp1) &&
			vecPoker[9]->getValue() != vecPoker[10]->getValue() &&
			vecPoker[9]->getValue() != vecPoker[11]->getValue() &&
			vecPoker[10]->getValue() != vecPoker[11]->getValue())
			return true;
		else if (IsThreePlane(temp2) &&
			vecPoker[0]->getValue() != vecPoker[1]->getValue() &&
			vecPoker[0]->getValue() != vecPoker[2]->getValue() &&
			vecPoker[1]->getValue() != vecPoker[2]->getValue())
			return true;

		return false;
	}

	// 三飞机带对 15
	static bool IsThreePlaneWithDouble(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 9; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 6; i < 15; i++)
			temp2.push_back(vecPoker[i]);;
		if (IsThreePlane(temp1)) {
			// 判断其余牌是不是对子
			int iLastValue = vecPoker[9]->getValue();
			int iCurrentValue;
			for (int i = 10; i < 15; i++) {
				iCurrentValue = vecPoker[i]->getValue();
				// i是偶数要和上一张比是不是相同牌
				if (i % 2 == 0 && iCurrentValue != iLastValue)
					return false;

				iLastValue = iCurrentValue;
			}
			return true;
		}
		else if (IsThreePlane(temp2)) {
			// 判断其余牌是不是对子
			int iLastValue = vecPoker[0]->getValue();
			int iCurrentValue;
			for (int i = 1; i < 6; i++) {
				iCurrentValue = vecPoker[i]->getValue();
				// i是偶数要和上一张比是不是相同牌
				if ((i + 1) % 2 == 0 && iCurrentValue != iLastValue)
					return false;

				iLastValue = iCurrentValue;
			}
			return true;
		}

		return true;
	}

	// 四飞机带单 16
	static bool IsFourPlaneWithSingle(vector<CPoker *>& vecPoker) {
		vector<CPoker *> temp1;
		vector<CPoker *> temp2;
		for (int i = 0; i < 12; i++)
			temp1.push_back(vecPoker[i]);;
		for (int i = 4; i < 16; i++)
			temp2.push_back(vecPoker[i]);;
		if (IsFourPlane(temp1)) {
			// 判断剩余牌是不是单牌
			for (int i = 12; i < 16; i++) {
				for (int j = i + 1; j < 16; j++) {
					if (vecPoker[i]->getValue() == vecPoker[j]->getValue())
						return false;
				}
			}
			return true;
		}
		else if (IsFourPlane(temp2)) {
			// 判断剩余牌是不是单牌
			for (int i = 0; i < 4; i++) {
				for (int j = i + 1; j < 4; j++) {
					if (vecPoker[i]->getValue() == vecPoker[j]->getValue())
						return false;
				}
			}
			return true;
		}

		return false;
	}
};

#endif