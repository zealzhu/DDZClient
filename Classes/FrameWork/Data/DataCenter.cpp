#include "DataCenter.h"
#include <cocos2d.h>
#include <UIManager.h>
#include <AppFunc.h>
#include <NotifacationString.h>
#include <NetManager.h>

USING_NS_CC;
using namespace std;
using namespace base;

CDataCenter::CDataCenter()
{
	m_processor.RegistHandler(this, kS2CLogin, &CDataCenter::dealWithLoginResponse);
	m_processor.RegistHandler(this, kS2CRegister, &CDataCenter::dealWithRegisterResponse);
	m_processor.RegistHandler(this, kS2CEnterRoom, &CDataCenter::dealWithEnterRoomResponse);
	m_processor.RegistHandler(this, kS2CCreateRoom, &CDataCenter::dealWithCreateRoomResponse);
	m_processor.RegistHandler(this, kS2CGetRoom, &CDataCenter::dealWithGetRoomResponse);
	m_processor.RegistHandler(this, kS2CLeaveRoom, &CDataCenter::dealWithLeaveRoomResponse);
	m_processor.RegistHandler(this, kS2CReady, &CDataCenter::dealWithReadyResponse);
	m_processor.RegistHandler(this, kS2CLandlord, &CDataCenter::dealWithLandlordResponse);
	m_processor.RegistHandler(this, kS2CPlay, &CDataCenter::dealWithPlayResponse);
	m_processor.RegistHandler(this, kS2CGameBeginNtf, &CDataCenter::dealWithGameBegin);
	m_processor.RegistHandler(this, kS2CPutCardNtf, &CDataCenter::dealWithPutCard);
	m_processor.RegistHandler(this, kS2CPutLandlordCardNtf, &CDataCenter::dealWithPutLandlordCard);
	m_processor.RegistHandler(this, kS2CPlayNtf, &CDataCenter::dealWithPlayNtf);
	m_processor.RegistHandler(this, kS2CGameOverNtf, &CDataCenter::dealWithGameOver);
	m_processor.RegistHandler(this, kS2CEnterRoomNtf, &CDataCenter::dealWithOtherEnterInRoom);
	m_processor.RegistHandler(this, kS2CLeaveRoomNtf, &CDataCenter::dealWithOtherLeaveRoom);
	m_processor.RegistHandler(this, kS2CReadyNtf, &CDataCenter::dealWithOtherReady); 
	m_processor.RegistHandler(this, kS2CLandlordNtf, &CDataCenter::dealWithOtherLandlord);
	m_processor.RegistHandler(this, kS2CGetSeatInfo, &CDataCenter::dealWithGetSeatInfoResponse);

	for (int i = 0; i < 54; i++) {
		m_cards[i] = Card(i);
	}
}

CDataCenter::~CDataCenter()
{
}

void CDataCenter::dealWithLoginResponse(user::LoginResp & rsp)
{
	if (rsp.code() == SUCCESS) {
		// 登录成功
		m_userId = rsp.id();
		m_userAccount = rsp.account();

		UIManagerIns->getTopLayer()->hideLoadingCircle();
		UIManagerIns->getTopLayer()->showDialog("Hint", "Login success", [](Ref *) {
			UIManagerIns->getTopLayer()->hideDialog();
			UIManagerIns->replaceCurrentLayer(ENUM_HALL_LAYER);
		});
	}
	//登录失败
	else {
		UIManagerIns->getTopLayer()->hideLoadingCircle();
		UIManagerIns->getTopLayer()->showDialog("Login failed", rsp.desc());
	}
}

void CDataCenter::dealWithRegisterResponse(user::RegisterResp & rsp)
{
}

void CDataCenter::dealWithEnterRoomResponse(room::EnterRoomResp & rsp)
{
	UIManagerIns->getTopLayer()->hideLoadingCircle();
	if (rsp.result() == SUCCESS) {
		// set room id
		m_currentRoomId = rsp.rid();
		m_currentSeatPosition = rsp.index();

		// ui change
		UIManagerIns->replaceCurrentLayer(ENUM_ROOM_LAYER);

		for (auto & id : rsp.otherid()) {
			room::GetSeatInfoReq req;
			req.set_uid(CDataCenter::getInstance()->getCurrentUserId());
			req.set_rid(CDataCenter::getInstance()->getCurrentRoomId());
			req.set_find(id);
			NetManagerIns->getGameServerSocket().send(kC2SGetSeatInfo, req);
		}
		// send current seat info to room
		//if(m_currentSeatPosition != 1)
		//	GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)&pEnterRoomResp->roominfo().seat1());
		//if (m_currentSeatPosition != 2)
		//	GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)&pEnterRoomResp->roominfo().seat2());
		//if (m_currentSeatPosition != 3)
		//	GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)&pEnterRoomResp->roominfo().seat3());
	}
	else {
		UIManagerIns->getTopLayer()->showDialog("error", rsp.desc());
	}
}

void CDataCenter::dealWithCreateRoomResponse(room::CreateRoomResp & rsp)
{
	// 创建房间成功
	if (rsp.result() == SUCCESS) {
		m_currentRoomId = rsp.rid();
		m_currentSeatPosition = rsp.index();

		// 更新房间列表
		//zhu::room::GetRoomReq msg;
		//msg.set_account(StringUtils::toString(m_userAccount));
		//NetManagerIns->getGameServerSocket().send(msg);
		// 进入房间
		UIManagerIns->getTopLayer()->hideLoadingCircle();
		UIManagerIns->replaceCurrentLayer(ENUM_ROOM_LAYER);
	}
	// 创建房间失败
	else {
		UIManagerIns->getTopLayer()->showDialog("hint", rsp.desc());
	}

}

void CDataCenter::dealWithLeaveRoomResponse(room::LeaveRoomResp & rsp)
{
	UIManagerIns->getTopLayer()->hideLoadingCircle();
	if (rsp.result() == SUCCESS) {
		UIManagerIns->replaceCurrentLayer(ENUM_HALL_LAYER);
		m_currentRoomId = 0;
	}
	else {
		UIManagerIns->getTopLayer()->showDialog("error", "leave room error");
	}
}

void CDataCenter::dealWithOtherEnterInRoom(room::EnterRoomNtf & ntf)
{
	int32_t rid = ntf.rid();
	int32_t uid = ntf.uid();
	int8_t index = ntf.index();
	bool ready = ntf.ready();

	std::string data;
	data.append((char *)&rid, sizeof(rid));
	data.append((char *)&uid, sizeof(uid));
	data.append((char *)&index, sizeof(index));
	data.append((char *)&ready, sizeof(ready));
	
	//shared_ptr<zhu::room::Seat> pSeat = dynamic_pointer_cast<zhu::room::Seat>(pMsg);
	
	GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)data.data());
}

void CDataCenter::dealWithReadyResponse(room::ReadyResp & ntf)
{
	// 当前用户准备成功
	if (ntf.result() != SUCCESS) {
		UIManagerIns->getTopLayer()->showDialog("error", "ready error");
	}
	else {
		GEventDispatch->dispatchCustomEvent(strReadySuccess, nullptr);
	}
}

//void CDataCenter::dealWithRoomStatuChangeMsg(MessagePtr pMsg)
//{
//	shared_ptr<zhu::room::RoomGameStatuChangeNotify> pStatuChangeMsg =
//		dynamic_pointer_cast<zhu::room::RoomGameStatuChangeNotify>(pMsg);
//
//	if (pStatuChangeMsg->start() == true) {
//		GEventDispatch->dispatchCustomEvent(strAllPlayerReady);
//	}
//}

void CDataCenter::dealWithGameBegin(room::GameBeginNtf & ntf)
{
	m_landlord = ntf.first();
	GEventDispatch->dispatchCustomEvent(strAllPlayerReady, (void *)&m_landlord);
}

void CDataCenter::dealWithPutCard(room::PutCardNtf & ntf)
{
	std::vector<Card> card;
	for (auto & cid : ntf.cid()) {
		card.emplace_back(m_cards[cid]);
	}
	GEventDispatch->dispatchCustomEvent(strFirstDispatchPoker, (void *)&card);
	//// 服务器初始分牌
	//if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::DEAL_POKER) {
	//	GEventDispatch->dispatchCustomEvent(strFirstDispatchPoker, (void *)pDispatchPockerResp.get());
	//}
	//// 显示地主牌
	//else if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::LANDLORD_POKER) {
	//	GEventDispatch->dispatchCustomEvent(strShowLandlordPoker, (void *)pDispatchPockerResp.get());
	//}
	//// 显示当前牌
	//else if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::CURRENT_POKER) {
	//	GEventDispatch->dispatchCustomEvent(strUpdateCurrentPoker, (void *)pDispatchPockerResp.get());
	//}
	//// 显示出的牌
	//else if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::PLAYER_POKER) {
	//	GEventDispatch->dispatchCustomEvent(strShowOtherPlayerPoker, (void *)pDispatchPockerResp.get());
	//}
}

void CDataCenter::dealWithPutLandlordCard(room::PutLandlordCardNtf & ntf)
{
	int8_t landlord = ntf.landlord();
	std::vector< Card > cards;
	for (auto & cid : ntf.cid()) {
		cards.emplace_back(m_cards[cid]);
	}

	std::string data;
	data.append((char *)&landlord, sizeof(landlord));
	data.append((char *)&cards, sizeof(std::vector< Card >));
	GEventDispatch->dispatchCustomEvent(strShowLandlordPoker, (void *)data.data());
}

void CDataCenter::dealWithPlayNtf(room::PlayNtf & ntf)
{
	int8_t curr = ntf.current();
	int8_t next = ntf.next();
	int16_t multiple = ntf.multiple();
	std::vector< Card > cards;
	for (auto & cid : ntf.cid()) {
		cards.emplace_back(m_cards[cid]);
	}

	std::string data;
	data.append((char *)&curr, sizeof(curr));
	data.append((char *)&next, sizeof(next));
	data.append((char *)&multiple, sizeof(multiple));
	data.append((char *)&cards, sizeof(std::vector< Card >));
	GEventDispatch->dispatchCustomEvent(strShowOtherPlayerPoker, (void *)data.data());
}

void CDataCenter::dealWithGameOver(room::GameOverNtf & ntf)
{
	int8_t winner = ntf.winner();
	int8_t landlord = ntf.landlord();
	int16_t multiple = ntf.multiple();

	std::string data;
	data.append((char *)&winner, sizeof(winner));
	data.append((char *)&landlord, sizeof(landlord));
	data.append((char *)&multiple, sizeof(multiple));
	GEventDispatch->dispatchCustomEvent(strGameOver, (void *)data.data());
}

void CDataCenter::dealWithLandlordResponse(room::LandlordResp & rsp)
{
	if (rsp.result() != SUCCESS) {
		UIManagerIns->getTopLayer()->showDialog("error", "call error");
	}
}

void CDataCenter::dealWithPlayResponse(room::PlayResp & rsp)
{
	auto topLayer = UIManagerIns->getTopLayer();
	int next = rsp.next();
	if (rsp.result() != SUCCESS) {
		topLayer->showDialog("error", "play error");
	}
	else {
		GEventDispatch->dispatchCustomEvent(strPlayPokerSuccess, (void *)&next);
	}
	//shared_ptr<zhu::table::PlayResp> pPlayResp = dynamic_pointer_cast<zhu::table::PlayResp>(pMsg);
	//auto topLayer = UIManagerIns->getTopLayer();
	//bool landlordWin = false;

	//// 比牌失败
	//if (pPlayResp->playresult() == table::ERROR_CODE::COMPARE_LOSE && 
	//	pPlayResp->account() == m_userAccount) {
	//	topLayer->showDialog("error", "can't big more");
	//}
	//// 不能不出
	//else if (pPlayResp->playresult() == table::ERROR_CODE::CAN_NOT_NO_PLAY &&
	//	pPlayResp->account() == m_userAccount) {
	//	topLayer->showDialog("warning", "turn to you, can't not no play");
	//}
	//// 出牌类型错误
	//else if (pPlayResp->playresult() == table::ERROR_CODE::PLAY_TYPE_ERROR &&
	//	pPlayResp->account() == m_userAccount) {
	//	topLayer->showDialog("warning", "play poker type error, please reelect");
	//}
	//// 出牌成功
	//else if (pPlayResp->playresult() == table::ERROR_CODE::SUCCESS ||
	//	pPlayResp->playresult() == table::ERROR_CODE::NO_PLAY) {
	//	GEventDispatch->dispatchCustomEvent(strPlayPokerSuccess, (void *)pPlayResp.get());
	//}
	//// 地主胜利游戏结束
	//else if (pPlayResp->playresult() == table::ERROR_CODE::LANDLORD_WIN) {
	//	landlordWin = true;
	//	GEventDispatch->dispatchCustomEvent(strGameOver, (void *)&landlordWin);
	//}
	//// 农名胜利游戏结束
	//else if (pPlayResp->playresult() == table::ERROR_CODE::PEASANT_WIN) {
	//	landlordWin = false;
	//	GEventDispatch->dispatchCustomEvent(strGameOver, (void *)&landlordWin);
	//}
}

void CDataCenter::dealWithGetSeatInfoResponse(room::GetSeatInfoResp & rsp)
{
	if (rsp.result() == SUCCESS) {
		int32_t uid = rsp.uid();
		std::string account = rsp.account();
		int8_t index = rsp.index();
		bool ready = rsp.ready();
		int32_t nameSize = account.size();

		std::string data;
		data.append((char *)&uid, sizeof(uid));
		data.append((char *)&index, sizeof(index));
		data.append((char *)&ready, sizeof(ready));
		data.append((char *)&nameSize, sizeof(nameSize));
		data.append(account.data(), account.size());
		GEventDispatch->dispatchCustomEvent(strGetSeatInfo, (void *)data.data());
	}
	else {
		UIManagerIns->getTopLayer()->showDialog("error", "get seat info error");
	}
}

void CDataCenter::dealWithOtherLeaveRoom(room::LeaveRoomNtf & ntf)
{
	int32_t uid = ntf.uid();
	GEventDispatch->dispatchCustomEvent(strPlayerOut, (void *)&uid);
}

void CDataCenter::dealWithOtherReady(room::ReadyNtf & ntf)
{
	int8_t index = ntf.index();
	bool ready = ntf.ready();

	std::string data;
	data.append((char *)&index, sizeof(index));
	data.append((char *)&ready, sizeof(ready));

	GEventDispatch->dispatchCustomEvent(strPlayerReady, (void *)data.data());
}

void CDataCenter::dealWithOtherLandlord(room::LandlordNtf & ntf)
{
	int8_t cindex = ntf.cindex();
	int8_t nindex = ntf.nindex();
	bool call = ntf.call();
	bool ctype = ntf.ctype();
	bool ntype = ntf.ntype();

	std::string data;
	data.append((char *)&cindex, sizeof(int8_t));
	data.append((char *)&nindex, sizeof(int8_t));
	data.append((char *)&call, sizeof(bool));
	data.append((char *)&ctype, sizeof(bool));
	data.append((char *)&ntype, sizeof(bool));
	GEventDispatch->dispatchCustomEvent(strCallLandlordResult, (void *)data.data());
}

void CDataCenter::dealWithGetRoomResponse(room::GetRoomResp & rsp)
{
	m_roomInfoList.clear();

	for (int i = 0; i < rsp.rooms_size(); i++) {
		ROOM_INFO_PTR roomInfo(new RoomInfo);
		auto room = rsp.rooms(i);
		roomInfo->_roomID = room.id();
		roomInfo->_roomName = room.name();
		roomInfo->_sitUserNum = room.count();
		roomInfo->_statu = room.state();
		m_roomInfoList.push_back(roomInfo);
	}
	// 更新房间列表
	GEventDispatch->dispatchCustomEvent(strUpdateRoomList);
}

std::list<ROOM_INFO_PTR>& CDataCenter::getRoomInfoList()
{
	// TODO: 在此处插入 return 语句
	return m_roomInfoList;
}

int CDataCenter::getCurrentUserId()
{
	return m_userId;
}

std::string & CDataCenter::getUserAccount()
{
	// TODO: 在此处插入 return 语句
	return m_userAccount;
}

int CDataCenter::getCurrentRoomId()
{
	return m_currentRoomId;
}

int CDataCenter::getCurrentSeatPosition()
{
	return m_currentSeatPosition;
}

void CDataCenter::process(std::shared_ptr<proto::Protobuf> msg)
{
	m_processor.Dispatch(*msg);
}
