#include "PokerUtils.h"

bool CPokerUtils::IsThreeWithOne(vector<CPoker *>& vecPoker) {
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

bool CPokerUtils::IsTreeWithTwo(vector<CPoker *>& vecPoker) {
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

bool CPokerUtils::IsSingleStraight(vector<CPoker *>& vecPoker) {
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

bool CPokerUtils::IsDoubleStraight(vector<CPoker *>& vecPoker) {
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

bool CPokerUtils::IsBoomWithSingle(vector<CPoker *>& vecPoker) {
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