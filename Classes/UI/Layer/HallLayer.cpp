#include "HallLayer.h"
#include <editor-support/cocostudio/CocoStudio.h>
#include "UIManager.h"
#include <AppFunc.h>
#include <RayCommonFunction.h>
#include <NotifacationString.h>
#include <room/room.pb.h>
#include <DataCenter.h>
#include <NetManager.h>

USING_NS_CC;

CHallLayer::CHallLayer()
{
	m_dialogLayerTouchListener = NULL;
	m_layerGraphNode = NULL;
	m_roomList = NULL;
	m_enterRoomInputDialog = NULL;
}

bool CHallLayer::init()
{
	if (!Layer::init())
		return false;

	m_layerGraphNode = CSLoader::createNode("Layer/Hall.csb");
	this->addChild(m_layerGraphNode);

	initEnterRoomDialog();

	// init list view
	m_roomList = (ui::ListView*)m_layerGraphNode->getChildByName("panel_room_list")->getChildByName("lv_room");
	m_roomList->setItemsMargin(3.f);
	m_roomList->setBounceEnabled(true);
	m_roomList->addTouchEventListener(CC_CALLBACK_2(CHallLayer::roomListViewTouchListener, this));

	// add click listener
	auto btnCreateRoom = (ui::Button*)m_layerGraphNode->getChildByName("btn_create_room");
	btnCreateRoom->addClickEventListener(CC_CALLBACK_1(CHallLayer::createRoomCallback, this));

	auto btnEnterRoom = (ui::Button*)m_layerGraphNode->getChildByName("btn_enter_room");
	btnEnterRoom->addClickEventListener(CC_CALLBACK_1(CHallLayer::enterRoomCallback, this));

	// get room list
	zhu::room::GetRoomReq msg;
	msg.set_account(StringUtils::toString(CDataCenter::getInstance()->getCurrentUserId()));
	NetManagerIns->getGameServerSocket().send(msg);

	return true;
}

void CHallLayer::onEnter()
{
	Layer::onEnter();
	GEventDispatch->addCustomEventListener(strUpdateRoomList, CC_CALLBACK_1(CHallLayer::updateRoomList, this));
	// 为输入对话框添加监听器
	if(m_dialogLayerTouchListener != NULL)
		GEventDispatch->addEventListenerWithSceneGraphPriority(m_dialogLayerTouchListener, m_enterRoomInputDialog->getParent());
}

void CHallLayer::onExit()
{
	GEventDispatch->removeCustomEventListeners(strUpdateRoomList);
	if (m_dialogLayerTouchListener != NULL)
		GEventDispatch->removeEventListener(m_dialogLayerTouchListener);
	Layer::onExit();
}

void CHallLayer::initEnterRoomDialog()
{
	auto baseLayer = (Layer *)m_layerGraphNode->getChildByName("enter_room_input_dialog");
	m_enterRoomInputDialog = baseLayer->getChildByName("panel_dialog");
	m_enterRoomInputDialog->setVisible(false);
	
	baseLayer->setVisible(true);
	//baseLayer->setTouchEnabled(false);		//加这句会报错，后面在研究

	// init dialog touch listener
	m_dialogLayerTouchListener = EventListenerTouchOneByOne::create();
	m_dialogLayerTouchListener->onTouchBegan = [](Touch * touch, Event * unused_event) {
		return true;
	};
	m_dialogLayerTouchListener->setSwallowTouches(false);

	// add click event
	auto btnDialogEnterRoom = (ui::Button*)m_enterRoomInputDialog->getChildByName("btn_enter");
	btnDialogEnterRoom->addClickEventListener(CC_CALLBACK_1(CHallLayer::dialogEnterRoomCallback, this));

	auto btnDialogCancle = (ui::Button*)m_enterRoomInputDialog->getChildByName("btn_cancle");
	btnDialogCancle->addClickEventListener(CC_CALLBACK_1(CHallLayer::dialogCancleRoomCallback, this));
}

void CHallLayer::updateRoomList(cocos2d::EventCustom * event)
{
	auto roomInfoList = CDataCenter::getInstance()->getRoomInfoList();
	m_roomList->getItems().clear();
	for (auto& roomInfo : roomInfoList) {
		auto item = CRoomListViewItem::create();
		item->updateInfo(*roomInfo);
		item->_ID = roomInfo->_roomID;
		m_roomList->pushBackCustomItem(item);
	}
}

void CHallLayer::createRoomCallback(cocos2d::Ref * target)
{
	// 创建房间
	static zhu::room::CreateRoomReq msg;
	msg.set_account(CDataCenter::getInstance()->getUserAccount());
	NetManagerIns->getGameServerSocket().send(msg);
	UIManagerIns->getTopLayer()->showLoadingCircle();
}

void CHallLayer::enterRoomCallback(cocos2d::Ref * target)
{
	m_enterRoomInputDialog->setVisible(true);
	m_dialogLayerTouchListener->setSwallowTouches(true);
}

void CHallLayer::dialogEnterRoomCallback(cocos2d::Ref * target)
{
	auto input = (ui::TextField *)m_enterRoomInputDialog->getChildByName("txt_input_room_id");
	std::string strInput = input->getString();

	int enterRoomId = atoi(strInput.c_str());
	if (enterRoomId == 0) {
		UIManagerIns->getTopLayer()->showDialog("error", "please input correct number");
		return;
	}

	sendEnterRoomMessage(enterRoomId);
}

void CHallLayer::dialogCancleRoomCallback(cocos2d::Ref * target)
{
	m_enterRoomInputDialog->setVisible(false);
	m_dialogLayerTouchListener->setSwallowTouches(false);
}

void CHallLayer::roomListViewTouchListener(cocos2d::Ref * ref, cocos2d::ui::Widget::TouchEventType type)
{
	const float NO_MOVE_DISTANCE = 10.0f;

	auto listView = (ui::ListView *)ref;

	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		Vec2 pBegin = listView->getTouchBeganPosition();
		Vec2 pEnd = listView->getTouchEndPosition();

		float distance = pBegin.distance(pEnd);

		if (distance <= NO_MOVE_DISTANCE)
		{
			for (auto& iter : listView->getItems())
			{
				CRoomListViewItem* item = (CRoomListViewItem*)(iter);

				if (RayFunction::isPointInNode(pEnd, item))
				{
					// 进入房间
					sendEnterRoomMessage(item->_ID);
					break;
				}
			}
		}
	}

}

void CHallLayer::sendEnterRoomMessage(int iRoomId)
{
	UIManagerIns->getTopLayer()->showLoadingCircle();
	zhu::room::EnterRoomReq msg;
	msg.set_account(CDataCenter::getInstance()->getUserAccount());
	msg.set_roomid(iRoomId);
	NetManagerIns->getGameServerSocket().send(msg);
}

bool CRoomListViewItem::init()
{
	if(!Layout::init())
		return false;

	auto rootNode = CSLoader::createNode("Node/RoomListItem.csb");	

	m_rootNode = rootNode->getChildByName("Panel_item"); 
	m_rootNode->setAnchorPoint(Vec2(0, -1.f));
	this->addChild(rootNode);
	this->setContentSize(m_rootNode->getContentSize());

	m_roomID = (ui::Text *)m_rootNode->getChildByName("txt_room_id");
	m_roomName = (ui::Text *)m_rootNode->getChildByName("txt_room_name");
	m_playerNumber = (ui::Text *)m_rootNode->getChildByName("txt_number");
	m_roomStatu = (ui::Text *)m_rootNode->getChildByName("txt_statu");

	return true;
}

void CRoomListViewItem::updateInfo(RoomInfo & info)
{
	m_roomID->setString(StringUtils::toString(info._roomID));
	m_roomName->setString(info._roomName);
	m_playerNumber->setString(StringUtils::toString(info._sitUserNum));
	switch (info._statu)
	{
	case zhu::room::RoomStatus::DESTORYE:
		m_roomStatu->setString("wait");
		break;
	case zhu::room::RoomStatus::FULL:
		m_roomStatu->setString("full");
		break;
	case zhu::room::RoomStatus::START:
		m_roomStatu->setString("start");
		break;
	case zhu::room::RoomStatus::WAIT:
		m_roomStatu->setString("wait");
		break;
	default:
		break;
	}
	
}

