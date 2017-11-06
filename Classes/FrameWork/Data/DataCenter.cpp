#include "DataCenter.h"
#include "Protocol/Code.h"
#include <cocos2d.h>
#include <UIManager.h>
#include <user/player.pb.h>
#include <room/room.pb.h>
#include <table/table.pb.h>
#include <AppFunc.h>
#include <NotifacationString.h>
#include <NetManager.h>

USING_NS_CC;
using namespace zhu;
using namespace std;

CDataCenter::CDataCenter()
{
}

CDataCenter::~CDataCenter()
{
}

bool CDataCenter::init()
{
	return true;
}

void CDataCenter::handle(std::shared_ptr<SelfDescribingMessage> pMsg)
{
	ProtocolCode::ErrorCode errorCode;
	auto pInnerMsg = ProtocolCode::parseInnerMsg(pMsg, errorCode);

	if (ProtocolCode::ErrorCode::UNKNOWN_MESSAGE_TYPE == errorCode || ProtocolCode::ErrorCode::PARSE_ERROR == errorCode)
	{
		// 解析消息失败
		log("ProtocolCode parseInnerMsg failed");
		return;
	}

	if ("zhu.user.LoginResp" == pMsg->type_name()) {
		dealWithLoginResponse(pInnerMsg);
	}
	else if ("zhu.room.CreateRoomResp" == pMsg->type_name()) {
		dealWithCreateRoomResponse(pInnerMsg);
	}
	else if ("zhu.room.GetRoomResp" == pMsg->type_name()) {
		dealWithGetRoomResponse(pInnerMsg);
	}
	else if ("zhu.room.LeaveRoomResp" == pMsg->type_name()) {
		dealWithLeaveRoomResponse(pInnerMsg);
	}
	else if ("zhu.room.EnterRoomResp" == pMsg->type_name()) {
		dealWithEnterRoomResponse(pInnerMsg);
	}
	else if ("zhu.room.Seat" == pMsg->type_name() && m_currentRoomId != 0) {
		dealWithOtherEnterInRoom(pInnerMsg);
	}
	else if ("zhu.room.ReadyResp" == pMsg->type_name()) {
		dealWithPlayerReadyMsg(pInnerMsg);
	}
	else if ("zhu.room.RoomGameStatuChangeNotify" == pMsg->type_name()) {
		dealWithRoomStatuChangeMsg(pInnerMsg);
	}
	else if ("zhu.table.DispatchPoker" == pMsg->type_name()) {
		dealWithDispatchPokerMsg(pInnerMsg);
	}
	else if ("zhu.table.RequestLandlordResp" == pMsg->type_name()) {
		dealWithCallLandlordMsg(pInnerMsg);
	}
	else if ("zhu.table.PlayResp" == pMsg->type_name()) {
		dealWithPlayResponse(pInnerMsg);
	}
}

void CDataCenter::dealWithLoginResponse(MessagePtr pMsg)
{
	shared_ptr<zhu::user::LoginResp> pLoginResp = dynamic_pointer_cast<zhu::user::LoginResp>(pMsg);
	if (pLoginResp->loginresult() == zhu::user::ERROR_CODE::SUCCESS) {
		// 登录成功
		m_userId = pLoginResp->id();
		m_userAccount = pLoginResp->account();

		UIManagerIns->getTopLayer()->hideLoadingCircle();
		UIManagerIns->getTopLayer()->showDialog("Hint", "Login success", [](Ref *) {
			UIManagerIns->getTopLayer()->hideDialog();
			UIManagerIns->replaceCurrentLayer(ENUM_HALL_LAYER);
		});
	}
	//登录失败
	else {
		UIManagerIns->getTopLayer()->hideLoadingCircle();
		UIManagerIns->getTopLayer()->showDialog("Login failed", pLoginResp->desc());
	}
}

void CDataCenter::dealWithEnterRoomResponse(MessagePtr pMsg)
{
	shared_ptr<zhu::room::EnterRoomResp> pEnterRoomResp = dynamic_pointer_cast<zhu::room::EnterRoomResp>(pMsg);
	UIManagerIns->getTopLayer()->hideLoadingCircle();
	if (pEnterRoomResp->enterresult() == zhu::room::ERROR_CODE::SUCCESS) {
		// set room id
		m_currentRoomId = pEnterRoomResp->roominfo().id();
		m_currentSeatPosition = pEnterRoomResp->position();

		// ui change
		UIManagerIns->replaceCurrentLayer(ENUM_ROOM_LAYER);

		// send current seat info to room
		if(m_currentSeatPosition != 1)
			GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)&pEnterRoomResp->roominfo().seat1());
		if (m_currentSeatPosition != 2)
			GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)&pEnterRoomResp->roominfo().seat2());
		if (m_currentSeatPosition != 3)
			GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)&pEnterRoomResp->roominfo().seat3());
	}
	else {
		UIManagerIns->getTopLayer()->showDialog("error", pEnterRoomResp->desc());
	}
}

void CDataCenter::dealWithCreateRoomResponse(MessagePtr pMsg)
{
	shared_ptr<zhu::room::CreateRoomResp> pCreateRoomResp = dynamic_pointer_cast<zhu::room::CreateRoomResp>(pMsg);
	// 创建房间成功
	if (pCreateRoomResp->createresult() == zhu::room::ERROR_CODE::SUCCESS) {
		m_currentRoomId = pCreateRoomResp->createroom().id();
		m_currentSeatPosition = 1;

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
		UIManagerIns->getTopLayer()->showDialog("hint", pCreateRoomResp->desc());
	}

}

void CDataCenter::dealWithLeaveRoomResponse(MessagePtr pMsg)
{
	shared_ptr<zhu::room::LeaveRoomResp> pLeaveRoomResp = dynamic_pointer_cast<zhu::room::LeaveRoomResp>(pMsg);
	UIManagerIns->getTopLayer()->hideLoadingCircle();
	if (pLeaveRoomResp->leaveroomresult() == zhu::room::ERROR_CODE::SUCCESS) {
		UIManagerIns->replaceCurrentLayer(ENUM_HALL_LAYER);
		m_currentRoomId = 0;
	}
	else {
		UIManagerIns->getTopLayer()->showDialog("error", "leave room error");
	}
}

void CDataCenter::dealWithOtherEnterInRoom(MessagePtr pMsg)
{
	shared_ptr<zhu::room::Seat> pSeat = dynamic_pointer_cast<zhu::room::Seat>(pMsg);

	// send current seat info to room
	GEventDispatch->dispatchCustomEvent(strPlayerEnterInRoom, (void *)pSeat.get());
}

void CDataCenter::dealWithPlayerReadyMsg(MessagePtr pMsg)
{
	shared_ptr<zhu::room::ReadyResp> pReadyResp = dynamic_pointer_cast<zhu::room::ReadyResp>(pMsg);

	// 当前用户准备成功
	if (pReadyResp->readyresult() == zhu::room::ERROR_CODE::SUCCESS) {
		GEventDispatch->dispatchCustomEvent(strPlayerReady, (void *)pReadyResp.get());
	}
	else if(pReadyResp->account() == m_userAccount){
		UIManagerIns->getTopLayer()->showDialog("error", pReadyResp->desc());
	}
}

void CDataCenter::dealWithRoomStatuChangeMsg(MessagePtr pMsg)
{
	shared_ptr<zhu::room::RoomGameStatuChangeNotify> pStatuChangeMsg =
		dynamic_pointer_cast<zhu::room::RoomGameStatuChangeNotify>(pMsg);

	if (pStatuChangeMsg->start() == true) {
		GEventDispatch->dispatchCustomEvent(strAllPlayerReady);
	}
}

void CDataCenter::dealWithDispatchPokerMsg(MessagePtr pMsg)
{
	shared_ptr<zhu::table::DispatchPoker> pDispatchPockerResp = dynamic_pointer_cast<zhu::table::DispatchPoker>(pMsg);

	// 服务器初始分牌
	if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::DEAL_POKER) {
		GEventDispatch->dispatchCustomEvent(strFirstDispatchPoker, (void *)pDispatchPockerResp.get());
	}
	// 显示地主牌
	else if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::LANDLORD_POKER) {
		GEventDispatch->dispatchCustomEvent(strShowLandlordPoker, (void *)pDispatchPockerResp.get());
	}
	// 显示当前牌
	else if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::CURRENT_POKER) {
		GEventDispatch->dispatchCustomEvent(strUpdateCurrentPoker, (void *)pDispatchPockerResp.get());
	}
	// 显示出的牌
	else if (pDispatchPockerResp->type() == zhu::table::DispatchPokerType::PLAYER_POKER) {
		GEventDispatch->dispatchCustomEvent(strShowOtherPlayerPoker, (void *)pDispatchPockerResp.get());
	}
}

void CDataCenter::dealWithCallLandlordMsg(MessagePtr pMsg)
{
	shared_ptr<zhu::table::RequestLandlordResp> pRequestLandlordResp = dynamic_pointer_cast<zhu::table::RequestLandlordResp>(pMsg);
	// 未轮到该玩家请求地主
	if (pRequestLandlordResp->calllandlordresult() == table::ERROR_CODE::NO_TURN_TO_REQUEST) {
		//cout << "还没轮到你请求" << endl;
		return;
	}
	GEventDispatch->dispatchCustomEvent(strCallLandlordResult, (void *)pRequestLandlordResp.get());
}

void CDataCenter::dealWithPlayResponse(MessagePtr pMsg)
{
	shared_ptr<zhu::table::PlayResp> pPlayResp = dynamic_pointer_cast<zhu::table::PlayResp>(pMsg);
	auto topLayer = UIManagerIns->getTopLayer();
	bool landlordWin = false;

	// 比牌失败
	if (pPlayResp->playresult() == table::ERROR_CODE::COMPARE_LOSE && 
		pPlayResp->account() == m_userAccount) {
		topLayer->showDialog("error", "can't big more");
	}
	// 不能不出
	else if (pPlayResp->playresult() == table::ERROR_CODE::CAN_NOT_NO_PLAY &&
		pPlayResp->account() == m_userAccount) {
		topLayer->showDialog("warning", "turn to you, can't not no play");
	}
	// 出牌类型错误
	else if (pPlayResp->playresult() == table::ERROR_CODE::PLAY_TYPE_ERROR &&
		pPlayResp->account() == m_userAccount) {
		topLayer->showDialog("warning", "play poker type error, please reelect");
	}
	// 出牌成功
	else if (pPlayResp->playresult() == table::ERROR_CODE::SUCCESS ||
		pPlayResp->playresult() == table::ERROR_CODE::NO_PLAY) {
		GEventDispatch->dispatchCustomEvent(strPlayPokerSuccess, (void *)pPlayResp.get());
	}
	// 地主胜利游戏结束
	else if (pPlayResp->playresult() == table::ERROR_CODE::LANDLORD_WIN) {
		landlordWin = true;
		GEventDispatch->dispatchCustomEvent(strGameOver, (void *)&landlordWin);
	}
	// 农名胜利游戏结束
	else if (pPlayResp->playresult() == table::ERROR_CODE::PEASANT_WIN) {
		landlordWin = false;
		GEventDispatch->dispatchCustomEvent(strGameOver, (void *)&landlordWin);
	}
}

void CDataCenter::dealWithGetRoomResponse(MessagePtr pMsg)
{
	shared_ptr<zhu::room::GetRoomResp> pGetRoomResp = dynamic_pointer_cast<zhu::room::GetRoomResp>(pMsg);

	m_roomInfoList.clear();

	for (int i = 0; i < pGetRoomResp->count(); i++) {
		ROOM_INFO_PTR roomInfo(new RoomInfo);
		auto room = pGetRoomResp->room(i);
		roomInfo->_roomID = room.id();
		roomInfo->_roomName = room.roomname();
		roomInfo->_sitUserNum = room.usercount();
		roomInfo->_statu = room.status();
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
