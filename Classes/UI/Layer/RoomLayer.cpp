#include "RoomLayer.h"
#include <editor-support/cocostudio/CocoStudio.h>
#include <NetManager.h>
#include <DataCenter.h>
#include <room/room.pb.h>
#include <UIManager.h>

#include "AppFunc.h"
#include "UI/Node/Poker.h"
#include "NotifacationString.h"

USING_NS_CC;

CRoomLayer::CRoomLayer()
{
}

bool CRoomLayer::init()
{
	if(!Layer::init())
		return false;

	m_layerGraphNode = CSLoader::createNode("Layer/Room.csb");
	this->addChild(m_layerGraphNode);

	// 退出房间按钮
	auto exitButton = (ui::Button *)m_layerGraphNode->getChildByName("btn_exit_room");
	exitButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::onExitRoomCallback, this));

	// 准备按钮
	m_readyButton = (ui::Button *)m_layerGraphNode->getChildByName("btn_ready");
	m_readyButton->addClickEventListener(CC_CALLBACK_1(CRoomLayer::readyClickListener, this));

	// 房间号
	auto roomId = (ui::Text *)m_layerGraphNode->getChildByName("txt_room_id");
	roomId->setString(StringUtils::format("Room Id : %d", CDataCenter::getInstance()->getCurrentRoomId()));

	// 初始化玩家信息
	initPlayer();

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

void CRoomLayer::onEnter()
{
	Layer::onEnter();
	GEventDispatch->addCustomEventListener(strPlayerEnterInRoom, CC_CALLBACK_1(CRoomLayer::onPlayerEnterInRoom, this));
	GEventDispatch->addCustomEventListener(strPlayerReady, CC_CALLBACK_1(CRoomLayer::onPlayerReady, this));
}

void CRoomLayer::onExit()
{
	GEventDispatch->removeCustomEventListeners(strPlayerEnterInRoom);
	GEventDispatch->removeCustomEventListeners(strPlayerReady);

	Layer::onExit();
}

void CRoomLayer::onExitRoomCallback(cocos2d::Ref * target)
{
	static zhu::room::LeaveRoomReq msg;
	msg.set_account(CDataCenter::getInstance()->getUserAccount());
	msg.set_roomid(CDataCenter::getInstance()->getCurrentRoomId());
	NetManagerIns->getGameServerSocket().send(msg);

	UIManagerIns->getTopLayer()->showLoadingCircle();
}

void CRoomLayer::onPlayerEnterInRoom(cocos2d::EventCustom * event)
{
	zhu::room::Seat * seat = (zhu::room::Seat *)event->getUserData();
	int serverPosition = seat->position();
	int clientPos = computeClientPosition(serverPosition);

	auto player = m_players[clientPos - 1];
	auto name = (ui::Text *)player->getChildByName("txt_name");
	auto spHead = player->getChildByName("sp_head"); 
	auto spReady = player->getChildByName("sp_ready");
	
	switch (seat->statu())
	{
	case zhu::room::Seat::UNREADY:
		name->setString(seat->playeraccount());
		spHead->setVisible(true);
		spReady->setVisible(false);
		break;
	case zhu::room::Seat::READY:
		name->setString(seat->playeraccount());
		spHead->setVisible(true);
		spReady->setVisible(true);
		break;
	case zhu::room::Seat::NO_PLAYER:
		name->setString("waiting");
		spHead->setVisible(false);
		spReady->setVisible(false);
		break;
	default:
		break;
	}
}

void CRoomLayer::onPlayerReady(cocos2d::EventCustom * event)
{
	zhu::room::ReadyResp * ready = (zhu::room::ReadyResp *)event->getUserData();
	int clientPos = computeClientPosition(ready->position());

	m_players[clientPos - 1]->getChildByName("sp_ready")->setVisible(ready->ready());
}

int CRoomLayer::computeClientPosition(int serverPosition)
{
	if (serverPosition == CDataCenter::getInstance()->getCurrentSeatPosition())
		return 1;

	int clientPos = 0;

	if (CDataCenter::getInstance()->getCurrentSeatPosition() == 1)
		clientPos = serverPosition;
	else if (CDataCenter::getInstance()->getCurrentSeatPosition() == 2)
		clientPos = serverPosition % 3 + 2;
	else if (CDataCenter::getInstance()->getCurrentSeatPosition() == 3)
		clientPos = serverPosition % 3 + 1;
	return clientPos;
}

void CRoomLayer::readyClickListener(cocos2d::Ref * target)
{
	m_readyButton->setVisible(false);

	static zhu::room::ReadyReq msg;
	msg.set_account(CDataCenter::getInstance()->getUserAccount());
	msg.set_ready(true);
	msg.set_roomid(CDataCenter::getInstance()->getCurrentRoomId());

	NetManagerIns->getGameServerSocket().send(msg);
}

