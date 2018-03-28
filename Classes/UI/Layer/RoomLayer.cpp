#include "RoomLayer.h"
#include <editor-support/cocostudio/CocoStudio.h>
#include <NetManager.h>
#include <UIManager.h>

#include "AppFunc.h"
#include "UI/Node/Poker.h"
#include "NotifacationString.h"
#include "PokerUtils.h"

USING_NS_CC;

CRoomLayer::CRoomLayer()
{
	m_landlordServerPosition = 0;
	m_noPlayFlag = 0;
	m_start = false;
}

bool CRoomLayer::init()
{
	if(!Layer::init())
		return false;

	m_layerGraphNode = CSLoader::createNode("Layer/Room.csb");
	this->addChild(m_layerGraphNode);

	m_currentCardPanel = (ui::Layout *)m_layerGraphNode->getChildByName("pannel_hand_cards");

	// 退出房间按钮
	auto exitButton = (ui::Button *)m_layerGraphNode->getChildByName("btn_exit_room");
	exitButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onExitRoomCallback, this));

	// 准备按钮
	m_readyButton = (ui::Button *)m_layerGraphNode->getChildByName("btn_ready");
	m_readyButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onReadyClickListener, this));

	// 房间号
	auto roomId = (ui::Text *)m_layerGraphNode->getChildByName("txt_room_id");
	roomId->setString(StringUtils::format("Room Id : %d", CDataCenter::getInstance()->getCurrentRoomId()));

	// 初始化玩家信息
	initPlayer();

	// 初始化抢地主层
	initCallLandlordLayer();

	// 初始化出牌层
	initPlayCardLayer();

	// 初始化每个玩家的消息
	initMsg();

	// 初始化手牌区
	initHandCardPanel();

	return true;
}

void CRoomLayer::initPlayer()
{
	m_players.push_back(m_layerGraphNode->getChildByName("panel_player_0"));
	m_players.push_back(m_layerGraphNode->getChildByName("panel_player_1"));
	m_players.push_back(m_layerGraphNode->getChildByName("panel_player_2"));

	auto name = (ui::Text *)m_players[0]->getChildByName("txt_name");
	name->setString(CDataCenter::getInstance()->getUserAccount());
}

void CRoomLayer::initMsg()
{
	char nodeName[30];
	for (int i = 1; i <= 3; i++) {
		sprintf(nodeName, "sp_player_%d_msg", i);
		auto msgNode = m_layerGraphNode->getChildByName(nodeName);
		msgNode->setVisible(false);
		m_playerMsgs.push_back((Sprite*)msgNode);
	}
}

void CRoomLayer::initCallLandlordLayer()
{
	m_callLandlordLayer = (Layer *)m_layerGraphNode->getChildByName("file_node_call_landlord");

	// Call Button
	m_callLandlordButton = (ui::Button *)m_callLandlordLayer->getChildByName("btn_call");
	m_callLandlordButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onCallLandlordClickListener, this));
	m_callLandlordButton->setVisible(false);

	// Not Call Button
	m_notCallLandlordButton = (ui::Button *)m_callLandlordLayer->getChildByName("btn_cancle");
	m_notCallLandlordButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onNotCallLandlordClickListener, this));
	m_notCallLandlordButton->setVisible(false);
}

void CRoomLayer::initPlayCardLayer()
{
	m_playCardLayer = (Layer *)m_layerGraphNode->getChildByName("file_node_play_card");

	// Play Button
	m_playButton = (ui::Button *)m_playCardLayer->getChildByName("btn_play");
	m_playButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onPlayClickListener, this));
	m_playButton->setVisible(false);

	// Not Play Button
	m_notPlayButton = (ui::Button *)m_playCardLayer->getChildByName("btn_not_play");
	m_notPlayButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onNotPlayClickListener, this));
	m_notPlayButton->setVisible(false);

	// Reelect Button
	m_reelectButton = (ui::Button *)m_playCardLayer->getChildByName("btn_reelect");
	m_reelectButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onReelectClickListener, this));
	m_reelectButton->setVisible(false);

	char nodeName[30];
	for (int i = 1; i <= 3; i++) {
		sprintf(nodeName, "panel_play_card_%d", i);
		auto panel = m_playCardLayer->getChildByName(nodeName);
		panel->setVisible(true);
		m_playCardPanel.push_back(panel);
	}
}

void CRoomLayer::initHandCardPanel()
{
	auto pokerPanel = (ui::Layout *)m_layerGraphNode->getChildByName("pannel_hand_cards");
	pokerPanel->addTouchEventListener(CC_CALLBACK_2(CRoomLayer::handCardPanelTouchListener, this));
	pokerPanel->setSwallowTouches(false);
}

void CRoomLayer::onEnter()
{
	Layer::onEnter();
	GEventDispatch->addCustomEventListener(strPlayerEnterInRoom, CC_CALLBACK_1(CRoomLayer::onPlayerEnterInRoom, this));
	GEventDispatch->addCustomEventListener(strPlayerReady, CC_CALLBACK_1(CRoomLayer::onPlayerReady, this));
	GEventDispatch->addCustomEventListener(strAllPlayerReady, CC_CALLBACK_1(CRoomLayer::onGameStart, this));
	GEventDispatch->addCustomEventListener(strFirstDispatchPoker, CC_CALLBACK_1(CRoomLayer::onInitPoker, this)); 
	GEventDispatch->addCustomEventListener(strCallLandlordResult, CC_CALLBACK_1(CRoomLayer::onCallLandlordSuccess, this));
	GEventDispatch->addCustomEventListener(strShowLandlordPoker, CC_CALLBACK_1(CRoomLayer::onShowLandlordPoker, this));
	GEventDispatch->addCustomEventListener(strPlayPokerSuccess, CC_CALLBACK_1(CRoomLayer::onPlaySuccess, this));
	GEventDispatch->addCustomEventListener(strShowOtherPlayerPoker, CC_CALLBACK_1(CRoomLayer::onShowOtherPlayerPoker, this));
	GEventDispatch->addCustomEventListener(strGameOver, CC_CALLBACK_1(CRoomLayer::onGameOver, this));
	GEventDispatch->addCustomEventListener(strPlayerOut, CC_CALLBACK_1(CRoomLayer::onPlayerOut, this));
	GEventDispatch->addCustomEventListener(strNoOneCallLandlord, CC_CALLBACK_1(CRoomLayer::onNoOneCallLandlord, this));
	GEventDispatch->addCustomEventListener(strGetSeatInfo, CC_CALLBACK_1(CRoomLayer::onGetSeatInfo, this));
	GEventDispatch->addCustomEventListener(strReadySuccess, CC_CALLBACK_1(CRoomLayer::onReadySuccess, this));
	GEventDispatch->addCustomEventListener(strReconnect, CC_CALLBACK_1(CRoomLayer::onReconnect, this));

	auto dataCenter = CDataCenter::getInstance();
	m_user_seat_map[dataCenter->getCurrentUserId()].index = dataCenter->getCurrentSeatPosition();
	m_user_seat_map[dataCenter->getCurrentUserId()].playerInfo._userId = dataCenter->getCurrentUserId();
	m_user_seat_map[dataCenter->getCurrentUserId()].playerInfo._userName = dataCenter->getUserAccount();
	m_user_seat_map[dataCenter->getCurrentUserId()].ready = true;

	int clientPos = computeClientPosition(m_user_seat_map[dataCenter->getCurrentUserId()].index);
	auto player = m_players[clientPos];
	auto name = (ui::Text *)player->getChildByName("txt_name");
	auto spHead = player->getChildByName("sp_head");
	auto spReady = player->getChildByName("sp_ready");

	name->setString(m_user_seat_map[dataCenter->getCurrentUserId()].playerInfo._userName);
	spHead->setVisible(true);
	spReady->setVisible(true);
	m_readyButton->setVisible(false);
}

void CRoomLayer::onExit()
{
	GEventDispatch->removeCustomEventListeners(strPlayerEnterInRoom);
	GEventDispatch->removeCustomEventListeners(strPlayerReady);
	GEventDispatch->removeCustomEventListeners(strAllPlayerReady);
	GEventDispatch->removeCustomEventListeners(strFirstDispatchPoker);
	GEventDispatch->removeCustomEventListeners(strCallLandlordResult);
	GEventDispatch->removeCustomEventListeners(strShowLandlordPoker);
	GEventDispatch->removeCustomEventListeners(strPlayPokerSuccess);
	GEventDispatch->removeCustomEventListeners(strShowOtherPlayerPoker);
	GEventDispatch->removeCustomEventListeners(strGameOver);
	GEventDispatch->removeCustomEventListeners(strPlayerOut);
	GEventDispatch->removeCustomEventListeners(strNoOneCallLandlord);
	GEventDispatch->removeCustomEventListeners(strGetSeatInfo);
	GEventDispatch->removeCustomEventListeners(strReadySuccess);

	Layer::onExit();
}

void CRoomLayer::onExitRoomCallback(cocos2d::Ref * target)
{
	room::LeaveRoomReq msg;
	msg.set_uid(CDataCenter::getInstance()->getCurrentUserId());
	msg.set_rid(CDataCenter::getInstance()->getCurrentRoomId());
	NetManagerIns->getGameServerSocket().send(kC2SLeaveRoom, msg);

	UIManagerIns->getTopLayer()->showLoadingCircle();
}

void CRoomLayer::onCallLandlordClickListener(cocos2d::Ref * target)
{
	auto dataCenter = CDataCenter::getInstance();
	room::LandlordReq msg;
	msg.set_uid(dataCenter->getCurrentUserId());
	msg.set_rid(dataCenter->getCurrentRoomId());
	msg.set_call(true);

	NetManagerIns->getGameServerSocket().send(kC2SLandlord, msg);
}

void CRoomLayer::onNotCallLandlordClickListener(cocos2d::Ref * target)
{
	auto dataCenter = CDataCenter::getInstance();
	room::LandlordReq msg;
	msg.set_uid(dataCenter->getCurrentUserId());
	msg.set_rid(dataCenter->getCurrentRoomId());
	msg.set_call(false);

	NetManagerIns->getGameServerSocket().send(kC2SLandlord, msg);
}

void CRoomLayer::onPlayClickListener(cocos2d::Ref * target)
{
	auto dataCenter = CDataCenter::getInstance();
	room::PlayReq req;
	req.set_uid(dataCenter->getCurrentUserId());
	req.set_rid(dataCenter->getCurrentRoomId());

	for (auto it = m_currentPokers.begin(); it != m_currentPokers.end(); it++) {
		auto poker = (CPoker *)*it;
		if (poker->getChoose()) {
			req.add_cid(poker->getCid());
		}
	}
	if (req.cid().size() > 0) {
		NetManagerIns->getGameServerSocket().send(kC2SPlay, req);
	}
	else {
		UIManagerIns->getTopLayer()->showDialog("error", "please choose card");
	}
	
	//std::vector<CPoker *> vecPoker;
	//bool bCorrect = true;
	//// 一张牌
	//if (vecPoker.size() == 1) {
	//	pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE);
	//}
	//// 两张
	//else if (vecPoker.size() == 2) {
	//	// 对子
	//	if (vecPoker[0]->getValue() == vecPoker[1]->getValue())
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_SAME);
	//	// 王炸
	//	else if ((vecPoker[0]->getSuit() == zhu::table::POKER_SUIT::BIG_JACKER &&
	//		vecPoker[1]->getSuit() == zhu::table::POKER_SUIT::SMALL_JACKER) ||
	//		(vecPoker[0]->getSuit() == zhu::table::POKER_SUIT::SMALL_JACKER &&
	//			vecPoker[1]->getSuit() == zhu::table::POKER_SUIT::BIG_JACKER))
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_JOCKER);
	//	else
	//		bCorrect = false;
	//}
	//// 三张牌
	//else if (vecPoker.size() == 3) {
	//	if (vecPoker[0]->getValue() == vecPoker[1]->getValue() &&
	//		vecPoker[0]->getValue() == vecPoker[2]->getValue())
	//		pMsg.set_type(zhu::table::PLAY_TYPE::THREE_SAME);
	//	else
	//		bCorrect = false;
	//}
	//// 判断四张牌
	//else if (vecPoker.size() == 4) {
	//	// 炸弹
	//	if (vecPoker[0]->getValue() == vecPoker[1]->getValue() &&
	//		vecPoker[0]->getValue() == vecPoker[2]->getValue() &&
	//		vecPoker[0]->getValue() == vecPoker[3]->getValue())
	//		pMsg.set_type(zhu::table::PLAY_TYPE::BOMB);
	//	// 三带一
	//	else if (CPokerUtils::IsThreeWithOne(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::THREE_SAME_WITH_ONE);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断五张牌
	//else if (vecPoker.size() == 5) {
	//	// 三带二
	//	if (CPokerUtils::IsTreeWithTwo(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::THREE_SAME_WITH_TWO);
	//	}
	//	// 顺子
	//	else if (CPokerUtils::IsSingleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE_STRAIGHT);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断6张牌
	//else if (vecPoker.size() == 6) {
	//	// 四带2单
	//	if (CPokerUtils::IsBoomWithSingle(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::BOMB_WIHT_SINGLE);
	//	}
	//	// 顺子
	//	else if (CPokerUtils::IsSingleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE_STRAIGHT);
	//	}
	//	// 连对
	//	else if (CPokerUtils::IsDoubleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_STRAIGHT);
	//	}
	//	// 两个飞机不带
	//	else if (CPokerUtils::IsTwoPlane(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_THREE_SAME);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断八张牌
	//else if (vecPoker.size() == 8) {
	//	// 飞机带单牌
	//	if (CPokerUtils::IsTwoPlaneWithSingle(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_THREE_SAME_WITH_SINGLE);
	//	}
	//	// 顺子
	//	else if (CPokerUtils::IsSingleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE_STRAIGHT);
	//	}
	//	// 四带两对
	//	else if (CPokerUtils::IsBoomWithDouble(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::BOMB_WIHT_DOUBLE);
	//	}
	//	// 连对
	//	else if (CPokerUtils::IsDoubleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_STRAIGHT);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断9张牌
	//else if (vecPoker.size() == 9) {
	//	// 三个飞机不带
	//	if (CPokerUtils::IsThreePlane(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::THREE_THREE_SAME);
	//	}
	//	// 顺子
	//	else if (CPokerUtils::IsSingleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE_STRAIGHT);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断10张牌
	//else if (vecPoker.size() == 10) {
	//	// 两个飞机带两对
	//	if (CPokerUtils::IsTwoPlaneWithDouble(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_THREE_SAME_WITH_DOUBLE);
	//	}
	//	// 顺子
	//	else if (CPokerUtils::IsSingleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE_STRAIGHT);
	//	}
	//	// 连对
	//	else if (CPokerUtils::IsDoubleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_STRAIGHT);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断十二张牌
	//else if (vecPoker.size() == 12) {
	//	// 三飞机带单
	//	if (CPokerUtils::IsThreePlaneWithSingle(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::THREE_THREE_SAME_WITH_SINGLE);
	//	}
	//	// 顺子
	//	else if (CPokerUtils::IsSingleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE_STRAIGHT);
	//	}
	//	// 连对
	//	else if (CPokerUtils::IsDoubleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_STRAIGHT);
	//	}
	//	// 四飞机不带
	//	else if (CPokerUtils::IsFourPlane(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::FOUR_THREE_SAME);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断15张牌
	//else if (vecPoker.size() == 15) {
	//	// 三飞机带对
	//	if (CPokerUtils::IsThreePlaneWithDouble(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::THREE_THREE_SAME_WITH_DOUBLE);
	//	}
	//	// 五飞机不带
	//	else if (CPokerUtils::IsFivePlane(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::FIVE_THREE_SAME);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 判断16张牌
	//else if (vecPoker.size() == 16) {
	//	// 四飞机带单
	//	if (CPokerUtils::IsFourPlaneWithSingle(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::FOUR_THREE_SAME_WITH_SINGLE);
	//	}
	//	// 连对
	//	else if (CPokerUtils::IsDoubleStraight(vecPoker)) {
	//		pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_STRAIGHT);
	//	}
	//	else
	//		bCorrect = false;
	//}
	//// 顺子
	//else if (CPokerUtils::IsSingleStraight(vecPoker)) {
	//	pMsg.set_type(zhu::table::PLAY_TYPE::SINGLE_STRAIGHT);
	//}
	//// 连对
	//else if (CPokerUtils::IsDoubleStraight(vecPoker)) {
	//	pMsg.set_type(zhu::table::PLAY_TYPE::DOUBLE_STRAIGHT);
	//}
	//else
	//	bCorrect = false;

	//// 选择牌型不正确
	//if (!bCorrect) {
	//	UIManagerIns->getTopLayer()->showDialog("error", "choose card error. please reelect");
	//	
	//	reelectPoker();
	//	return;
	//}

	//// 发送牌
	//for (auto p : vecPoker) {
	//	auto pPoker = pMsg.add_pokers();
	//	pPoker->set_number(p->getNumber());
	//	pPoker->set_suit((zhu::table::POKER_SUIT)p->getSuit());
	//	pPoker->set_value(p->getValue());
	//}

	//NetManagerIns->getGameServerSocket().send(pMsg);
}

void CRoomLayer::onNotPlayClickListener(cocos2d::Ref * target)
{
	auto dataCenter = CDataCenter::getInstance();
	room::PlayReq req;
	req.set_uid(dataCenter->getCurrentUserId());
	req.set_rid(dataCenter->getCurrentRoomId());
	NetManagerIns->getGameServerSocket().send(kC2SPlay, req);
}

void CRoomLayer::onReelectClickListener(cocos2d::Ref * target)
{
	reelectPoker();
}

void CRoomLayer::onGetSeatInfo(cocos2d::EventCustom * event)
{
	int32_t uid = (int32_t)*((char *)event->getUserData());
	int8_t index = (int8_t)*((char *)event->getUserData() + sizeof(int32_t));
	bool ready = (bool)*((char *)event->getUserData() + sizeof(int32_t) + sizeof(int8_t));
	int8_t cnumber = (int8_t)*((char *)event->getUserData() + sizeof(int32_t) + sizeof(int8_t) + sizeof(bool));
	std::string account = (char *)event->getUserData() + sizeof(int32_t) + sizeof(int8_t) + sizeof(bool) + sizeof(int8_t);
	
	m_user_seat_map[uid].index = index;
	m_user_seat_map[uid].playerInfo._userId = uid;
	m_user_seat_map[uid].playerInfo._userName = account;
	m_user_seat_map[uid].ready = ready;
	
	int clientPos = computeClientPosition(index);
	auto player = m_players[clientPos];
	auto name = (ui::Text *)player->getChildByName("txt_name");
	auto spHead = player->getChildByName("sp_head"); 
	auto spReady = player->getChildByName("sp_ready");
	name->setString(account);
	spHead->setVisible(true);

	updatePlayerPokerNumber(index, cnumber);
	
	if (!m_start) {
		spReady->setVisible(ready);
	}
}

void CRoomLayer::onPlayerEnterInRoom(cocos2d::EventCustom * event)
{
	int32_t rid = (int32_t)*((char *)event->getUserData());
	int32_t uid = (int32_t)*((char *)event->getUserData() + sizeof(int32_t));
	int8_t index = (int8_t)*((char *)event->getUserData() + sizeof(int32_t) + sizeof(int32_t));
	bool ready = (bool)*((char *)event->getUserData() + sizeof(int32_t) + sizeof(int32_t) + sizeof(int8_t));

	room::GetSeatInfoReq req;
	req.set_uid(CDataCenter::getInstance()->getCurrentUserId());
	req.set_rid(CDataCenter::getInstance()->getCurrentRoomId());
	req.set_find(uid);
	NetManagerIns->getGameServerSocket().send(kC2SGetSeatInfo, req);
}

void CRoomLayer::onPlayerReady(cocos2d::EventCustom * event)
{
	int8_t index = *(int8_t *)event->getUserData();
	bool ready = *(bool *)((char *)event->getUserData() + sizeof(int8_t));
	int clientPos = computeClientPosition(index);
	m_players[clientPos]->getChildByName("sp_ready")->setVisible(ready);
}

void CRoomLayer::onReadySuccess(cocos2d::EventCustom * event)
{
	int clientPos = computeClientPosition(CDataCenter::getInstance()->getCurrentSeatPosition());
	auto ready = m_players[clientPos]->getChildByName("sp_ready");
	ready->setVisible(!ready->isVisible());
}

void CRoomLayer::onGameStart(cocos2d::EventCustom * event)
{
	int landlord = *(int *)event->getUserData();
	m_start = true;
	m_callLandlordLayer->setVisible(true);

	for (int i = 0; i < 3; i++) {
		updatePlayerPokerNumber(i, 17);
		m_players[i]->getChildByName("sp_ready")->setVisible(false);
	}

	// 是否地主
	if (landlord == CDataCenter::getInstance()->getCurrentSeatPosition()) {
		updateCallLandlordButton(true);
	}
	else {
		updateCallLandlordButton(false, false);
		int landlordPosition = computeClientPosition(landlord);
		m_players[landlordPosition]->getChildByName("img_wait")->setVisible(true);
	}
}

void CRoomLayer::onInitPoker(cocos2d::EventCustom * event)
{
	std::vector<Card> * cards = (std::vector<Card> *)event->getUserData();
	Sort(*cards);
	updateCurrentPoker(*cards);
}

void CRoomLayer::onCallLandlordSuccess(cocos2d::EventCustom * event)
{
	int8_t cindex = (int8_t)*((char *)event->getUserData());
	int8_t nindex = (int8_t)*((char *)event->getUserData() + sizeof(int8_t));
	bool call = (bool)*((char *)event->getUserData() + sizeof(int8_t) + sizeof(int8_t));
	bool currentType = (bool)*((char *)event->getUserData() + sizeof(int8_t) + sizeof(int8_t) + sizeof(bool));
	bool nextType = (bool)*((char *)event->getUserData() + sizeof(int8_t) + sizeof(int8_t) + sizeof(bool) + sizeof(bool));

	// 当前操作是自己
	if (cindex == CDataCenter::getInstance()->getCurrentSeatPosition()) {
		// 隐藏操作按钮
		updateCallLandlordButton(false, false);			
	}
	else if (nindex == CDataCenter::getInstance()->getCurrentSeatPosition()) {
		// 显示操作按钮
		updateCallLandlordButton(!nextType, true);
	}
	// 显示操作玩家消息
	updateCallLandlordMsg(cindex, currentType, call);
	// 隐藏操作玩家倒计时
	showTimeClock(cindex, false);

	// 显示他人倒计时
	showTimeClock(nindex, true);
	// 隐藏他人玩家消息
	updateCallLandlordMsg(nindex, nextType, call, false);
}

void CRoomLayer::onShowLandlordPoker(cocos2d::EventCustom * event)
{
	int8_t landlord = *(int8_t *)event->getUserData();
	std::vector<Card> & cards = *(std::vector<Card> *)((char *)event->getUserData() + sizeof(int8_t));
	auto panelLandlordCard = m_callLandlordLayer->getChildByName("panel_card");
	char buf[20];

	for (int i = 0; i < 3; i++) {
		sprintf(buf, "sp_handlord_card_%d", i + 1);
		auto spCard = (Sprite *)panelLandlordCard->getChildByName(buf);
		spCard->setDisplayFrame(
			GSpriteFrameCache->getSpriteFrameByName(
				CPoker::getPokerFileName(cards[i].point, cards[i].suit)));
		spCard->setScale(0.7f);
		
	}
	
	m_landlordServerPosition = landlord;

	// 隐藏地主的按钮
	updateCallLandlordButton(false, false);
	// 隐藏所有消息框
	hideAllMsg();
	// 隐藏所有倒计时
	hideAllClock();

	m_playCardLayer->setVisible(true);
	// 当前用户为地主
	if (landlord == CDataCenter::getInstance()->getCurrentSeatPosition()) {
		// 显示出牌按钮
		showPlayButton(true);
		std::vector< Card > updateCard = std::move(m_currentCards);
		for (auto & card : cards) {
			updateCard.push_back(card);
		}
		Sort(updateCard);
		updateCurrentPoker(updateCard);
	}
	// 等待地主出牌
	else {
		// 显示地主倒计时
		showTimeClock(landlord, true);
		// 地主牌数+3
		updatePlayerPokerNumber(landlord, 20);
	}

	// 更新头像
	updateHead();
}

void CRoomLayer::onPlaySuccess(cocos2d::EventCustom * event)
{
	//auto posi = CDataCenter::getInstance()->getCurrentSeatPosition();
	//int next = *(int *)event->getUserData();
}

void CRoomLayer::onShowOtherPlayerPoker(cocos2d::EventCustom * event)
{
	auto posi = CDataCenter::getInstance()->getCurrentSeatPosition();
	int8_t curr = *(int8_t *)event->getUserData();
	int8_t next = *(int8_t *)((char*)event->getUserData() + sizeof(int8_t));
	int16_t multiple = *(int8_t *)((char*)event->getUserData() + sizeof(int8_t) + sizeof(int8_t));
	std::vector<Card> & cards = *(std::vector<Card> *)((char *)event->getUserData() + sizeof(int8_t) + sizeof(int8_t) + sizeof(int16_t));

	// 下一位玩家是自己的话要隐藏之前所有的提示并显示按钮
	if (next == posi) {
		showTimeClock(posi, false);
		showNoPlay(posi, false);
		showPlayButton(true);
	}
	//　否则要显示下一位玩家的倒计时以及隐藏下一位玩家的提示
	else {
		showTimeClock(next, true);
		showNoPlay(next, false);
		showPlayButton(false);
	}

	// 清除下一位玩家出的牌
	clearPlayPoker(next);

	// 该玩家不出则要显示不出按钮
	if (cards.size() == 0) {
		showNoPlay(curr, true);
		showTimeClock(curr, false);

		// 两次不出则要清空牌桌上的牌
		if (m_noPlayFlag == 1) {
			clearAllPlayPoker();
			m_noPlayFlag = 0;
		}
		else {
			m_noPlayFlag++;
		}
	}
	// 否则要隐藏该玩家的所有信息并显示牌
	else {
		showTimeClock(curr, false);
		showNoPlay(curr, false);
		m_noPlayFlag = 0;
		updatePlayPoker(cards, curr);
	}

	// 该玩家不是自己的话要更新牌数
	if (curr != posi) {
		int currentIndex = computeClientPosition(curr);
		auto player = m_players[currentIndex];
		auto pokerNumber = (ui::TextBMFont *)player->getChildByName("txt_surplus_card_number");
		int number = atoi(pokerNumber->getString().c_str()) - cards.size();
		updatePlayerPokerNumber(curr, number > 0 ? number : 0);
	}
	// 更新手牌
	else {
		std::vector< Card > updateCard;
		for (auto & card : m_currentCards) {
			bool find = false;
			for (auto & outCard : cards) {
				if (card.id == outCard.id) {
					find = true;
					break;
				}
			}
			if (!find) {
				updateCard.push_back(card);
			}	
		}
		Sort(updateCard);
		updateCurrentPoker(updateCard);
	}
}

void CRoomLayer::onGameOver(cocos2d::EventCustom * event)
{
	auto posi = CDataCenter::getInstance()->getCurrentSeatPosition();
	int8_t winner = *(int8_t *)event->getUserData();
	int8_t landlord = *(int8_t *)((char*)event->getUserData() + sizeof(int8_t));
	int16_t multiple = *(int8_t *)((char*)event->getUserData() + sizeof(int8_t) + sizeof(int8_t));

	if (landlord == posi) {
		multiple *= 2;
	}

	if (winner == posi) {
		UIManagerIns->getTopLayer()->showDialog("good", "you win");
	}
	else {
		UIManagerIns->getTopLayer()->showDialog("fuck", "you lose");
	}
	reinit();
	//m_readyButton->setVisible(false);
	//room::ReadyReq req;
	//req.set_ready(true);
	//req.set_rid(CDataCenter::getInstance()->getCurrentRoomId());
	//req.set_uid(CDataCenter::getInstance()->getCurrentUserId());
	//NetManagerIns->getGameServerSocket().send(kC2SReady, req);
}

void CRoomLayer::onPlayerOut(cocos2d::EventCustom * event)
{
	int32_t uid = *(int*)event->getUserData();
	int index = m_user_seat_map[uid].index;
	int clientPos = computeClientPosition(index);
	
	auto player = m_players[clientPos];
	auto name = (ui::Text *)player->getChildByName("txt_name");
	auto spHead = player->getChildByName("sp_head");
	auto spReady = player->getChildByName("sp_ready");
	name->setString("waiting");
	spHead->setVisible(false);
	spReady->setVisible(false);
	m_user_seat_map.erase(uid);
}

void CRoomLayer::onNoOneCallLandlord(cocos2d::EventCustom * event)
{
	UIManagerIns->getTopLayer()->showDialog("hint", "No one call, Restart!");
	hideAllMsg();
	hideAllClock();
}

void CRoomLayer::onReconnect(cocos2d::EventCustom * event)
{
	auto posi = CDataCenter::getInstance()->getCurrentSeatPosition();
	int current = *(int *)event->getUserData();
	int landlord = *(int *)((char*)event->getUserData() + sizeof(int));
	int multiple = *(int *)((char*)event->getUserData() + sizeof(int) + sizeof(int));
	ReconnectRoomState state = *(ReconnectRoomState *)((char*)event->getUserData() + sizeof(int) + sizeof(int) + sizeof(int));
	std::vector<int> & other = *(std::vector<int> *)((char*)event->getUserData() + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(ReconnectRoomState));
	std::vector<Card> & card = *(std::vector<Card> *)((char*)event->getUserData() + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(ReconnectRoomState) + sizeof(other));
	int noplay = *(int *)((char*)event->getUserData() + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(ReconnectRoomState) + sizeof(other) + sizeof(card));

	for (auto & id : other) {
		room::GetSeatInfoReq req;
		req.set_uid(CDataCenter::getInstance()->getCurrentUserId());
		req.set_rid(CDataCenter::getInstance()->getCurrentRoomId());
		req.set_find(id);
		NetManagerIns->getGameServerSocket().send(kC2SGetSeatInfo, req);
	}
	m_landlordServerPosition = landlord;
	updateCurrentPoker(card);
	if (current == posi) {
		if (state == ReconnectRoomState::CALL) {
			updateCallLandlordButton(true, true);
		}
		else if (state == ReconnectRoomState::RUSH) {
			updateCallLandlordButton(false, true);
		}
		else if (state == ReconnectRoomState::PLAY) {
			showPlayButton(true);
		}
	}
	else {
		showTimeClock(current, true);
	}
	m_playCardLayer->setVisible(true);
	m_noPlayFlag = noplay == 1 ? 1 : 0;
	m_start = true;
	auto player = m_players[computeClientPosition(posi)];
	auto spReady = player->getChildByName("sp_ready");
	spReady->setVisible(false);
	updateHead();
}

void CRoomLayer::handCardPanelTouchListener(cocos2d::Ref * ref, cocos2d::ui::Widget::TouchEventType type)
{
	auto pokerPanel = (ui::Layout *)ref;
	auto pokers = pokerPanel->getChildren();

	auto beginPosition = pokerPanel->convertToNodeSpace(pokerPanel->getTouchBeganPosition());
	auto endPosition = pokerPanel->convertToNodeSpace(pokerPanel->getTouchEndPosition());
	auto movePosition = pokerPanel->convertToNodeSpace(pokerPanel->getTouchMovePosition());

	Rect rect;

	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		for (auto & temp : pokers) {
			auto poker = (CPoker *)temp;

			if (poker->getTouchFlag() == true) {
				poker->click();
				poker->setTouchFlag(false);
				poker->updateColor();
			}
		}
	}
	else if (type == cocos2d::ui::Widget::TouchEventType::MOVED) {
		for (auto & temp : pokers) {
			auto poker = (CPoker *)temp;
			if (poker->getTouchFlag() == false) {
				rect.setRect(poker->getPositionX(), poker->getPositionY(), poker->getBoundingBox().size.width / 4, poker->getBoundingBox().size.height);
				if (rect.containsPoint(movePosition)) {
					poker->setTouchFlag(true);
					poker->updateColor();
				}
			}	
		}
	}
	else if (type == cocos2d::ui::Widget::TouchEventType::BEGAN) {
		for (auto & temp : pokers) {
			auto poker = (CPoker *)temp;
			rect.setRect(poker->getPositionX(), poker->getPositionY(), poker->getBoundingBox().size.width / 4, poker->getBoundingBox().size.height);
			if (rect.containsPoint(beginPosition)) {
				poker->setTouchFlag(true);
				poker->updateColor();
			}
		}
	}
}

int CRoomLayer::computeClientPosition(int serverPosition)
{
	if (serverPosition == CDataCenter::getInstance()->getCurrentSeatPosition())
		return 0;

	int clientPos = 0;

	if (CDataCenter::getInstance()->getCurrentSeatPosition() == 0)
		clientPos = serverPosition;
	else if (CDataCenter::getInstance()->getCurrentSeatPosition() == 1)
		clientPos = (serverPosition + 2) % 3;
	else if (CDataCenter::getInstance()->getCurrentSeatPosition() == 2)
		clientPos = (serverPosition + 1) % 3;
	return clientPos;
}

void CRoomLayer::updateCallLandlordMsg(int serverPosition, bool isQiang, bool call, bool show)
{
	int clientIndex = computeClientPosition(serverPosition);
	auto msgSprite = m_playerMsgs[clientIndex];

	msgSprite->setVisible(show);

	if (!isQiang) {
		if (call) {
			// 叫
			msgSprite->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/message1.png"));
		}
		else {
			// 不叫
			msgSprite->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/message0.png"));
		}
	}
	else {
		if (call) {
			// 抢
			msgSprite->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/message3.png"));
		}
		else {
			// 不抢
			msgSprite->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/message2.png"));
		}
	}
}

void CRoomLayer::showNoPlay(int serverPosition, bool show)
{
	int clientIndex = computeClientPosition(serverPosition);
	auto player = m_players[clientIndex];
	auto msgSprite = m_playerMsgs[clientIndex];

	msgSprite->setVisible(show);

	msgSprite->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/buchu.png"));
}

void CRoomLayer::updateCallLandlordButton(bool isCall, bool show)
{
	m_callLandlordButton->setVisible(show);
	m_notCallLandlordButton->setVisible(show);

	if (isCall) {
		m_callLandlordButton->loadTextures("UITest/pic/message_jiao.png", "UITest/pic/message_jiao.png", 
			"UITest/pic/message_jiao.png", ui::Widget::TextureResType::PLIST);
		m_notCallLandlordButton->loadTextures("UITest/pic/message_bu.png", "UITest/pic/message_bu.png",
			"UITest/pic/message_bu.png", ui::Widget::TextureResType::PLIST);
	}
	else {
		m_callLandlordButton->loadTextures("UITest/pic/message_qiang.png", "UITest/pic/message_qiang.png",
			"UITest/pic/message_qiang.png", ui::Widget::TextureResType::PLIST);
		m_notCallLandlordButton->loadTextures("UITest/pic/message_buQiang.png", "UITest/pic/message_buQiang.png",
			"UITest/pic/message_buQiang.png", ui::Widget::TextureResType::PLIST);
	}
}

void CRoomLayer::showTimeClock(int serverPosition, bool show)
{
	int landlordPosition = computeClientPosition(serverPosition);
	m_players[landlordPosition]->getChildByName("img_wait")->setVisible(show);
}

void CRoomLayer::showPlayButton(bool show)
{
	m_playButton->setVisible(show);
	m_notPlayButton->setVisible(show);
	m_reelectButton->setVisible(show);
}

void CRoomLayer::updateCurrentPoker(std::vector<Card> & cards)
{
	clearAllHandPoker();

	auto pokerPanel = m_layerGraphNode->getChildByName("pannel_hand_cards");

	const float pokerWidth = CPoker::create(1, 1, 1, Suit::kClub)->getContentSize().width;
	int pokerSize = cards.size();
	const float TOTAL_WIDTH = 1 + (pokerSize - 1) / 4 + (pokerSize - 1) % 4 * 0.25f;

	int iPosition = 0;

	for (auto & card : cards) {
		auto createPoker = CPoker::create(card.id, card.point, card.grade, card.suit);
		createPoker->setPosition(
			pokerPanel->getContentSize().width / 2 - (pokerWidth * TOTAL_WIDTH / 2 - pokerWidth / 4 * iPosition),
			0);
		pokerPanel->addChild(createPoker);
		m_currentPokers.push_back(createPoker);
		m_currentCards.push_back(card);
		iPosition++;
	}
}

void CRoomLayer::updatePlayPoker(const std::vector<Card> & cards, int position)
{
	int currentIndex = computeClientPosition(position);
	auto pokerPanel = m_playCardPanel[currentIndex];
	pokerPanel->removeAllChildrenWithCleanup(true);

	const float pokerWidth = CPoker::create(1, 1, 1, Suit::kClub)->getContentSize().width;
	int pokerSize = cards.size();
	const float TOTAL_WIDTH = 1 + (pokerSize - 1) / 4 + (pokerSize - 1) % 4 * 0.25f;

	int iPosition = 0;

	for (auto & card : cards) {
		auto createPoker = CPoker::create(card.id, card.point, card.grade, card.suit);
		createPoker->setPosition(
			pokerPanel->getContentSize().width / 2 - (pokerWidth * TOTAL_WIDTH / 2 - pokerWidth / 4 * iPosition),
			0);
		pokerPanel->addChild(createPoker);
		iPosition++;
	}
}

void CRoomLayer::updatePlayerPokerNumber(int serverPosition, int number)
{
	int clientIndex = computeClientPosition(serverPosition);
	auto player = m_players[clientIndex];
	auto pokerNumber = (ui::TextBMFont *)player->getChildByName("txt_surplus_card_number");
	if (pokerNumber != NULL)
		pokerNumber->setString(StringUtils::toString(number));
}

void CRoomLayer::reelectPoker()
{
	for (auto it = m_currentPokers.begin(); it != m_currentPokers.end(); it++) {
		auto poker = (CPoker *)*it;
		poker->setChoose(false);
		poker->updatePositionY();
	}
}

void CRoomLayer::updateHead()
{
	int landlordClientPosition = computeClientPosition(m_landlordServerPosition);
	for (int i = 0; i < m_players.size(); i++) {
		auto spHead = (Sprite *)m_players[i]->getChildByName("sp_head");

		if (m_start) {
			if (landlordClientPosition == i) {
				// 更新为地主的头像
				spHead->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/logo_dizhu.png"));
			}
			else {
				// 更新为农民的头像
				spHead->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/logo_nongmin.png"));
			}
		}
		else {
			// 更新为正常的头像
			spHead->setDisplayFrame(GSpriteFrameCache->getSpriteFrameByName("UITest/pic/logo_unknown.png"));
		}	
	}
	
}

void CRoomLayer::hideAllMsg()
{
	showNoPlay(0, false);
	showNoPlay(1, false);
	showNoPlay(2, false);
}

void CRoomLayer::hideAllClock()
{
	showTimeClock(0, false);
	showTimeClock(1, false);
	showTimeClock(2, false);
}

void CRoomLayer::clearPlayPoker(int position)
{
	int currentIndex = computeClientPosition(position);
	auto pokerPanel = m_playCardPanel[currentIndex];
	pokerPanel->removeAllChildrenWithCleanup(true);
}

void CRoomLayer::clearAllPlayPoker()
{
	for (auto panel : m_playCardPanel) {
		panel->removeAllChildrenWithCleanup(true);
	}
}

void CRoomLayer::reinit()
{
	m_start = false;

	// 显示准备按钮
	m_readyButton->setVisible(true);

	updateHead();
	hideAllClock();
	hideAllMsg();
	clearAllPlayPoker();
	m_callLandlordLayer->setVisible(false);
	m_playCardLayer->setVisible(false);

	updateCallLandlordButton(false, false);
	showPlayButton(false);

	for (int i = 0; i < 3; i++) {
		updatePlayerPokerNumber(i, 0);
	}

	clearAllHandPoker();

	m_start = false;
	m_landlordServerPosition = 0;
	m_noPlayFlag = 0;
}

void CRoomLayer::clearAllHandPoker()
{
	auto pokerPanel = m_layerGraphNode->getChildByName("pannel_hand_cards");
	m_currentPokers.clear();
	m_currentCards.clear();
	pokerPanel->removeAllChildrenWithCleanup(true);
}

void CRoomLayer::Sort(std::vector<Card>& card)
{
	std::sort(card.begin(), card.end(), [](const Card & f, const Card & s) { return s.grade < f.grade; });
}

void CRoomLayer::onReadyClickListener(cocos2d::Ref * target)
{
	m_readyButton->setVisible(false);

	room::ReadyReq req;
	req.set_ready(true);
	req.set_rid(CDataCenter::getInstance()->getCurrentRoomId());
	req.set_uid(CDataCenter::getInstance()->getCurrentUserId());
	NetManagerIns->getGameServerSocket().send(kC2SReady, req);
}

