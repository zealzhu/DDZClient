#pragma once
#ifndef _HALL_LAYER_H
#define _HALL_LAYER_H

#include "BaseLayer.h"
#include <ui/CocosGUI.h>
#include <DataCenter.h>

class CRoomListViewItem 
	: public cocos2d::ui::Layout
{
public:
	CREATE_FUNC(CRoomListViewItem);
	virtual bool init();
	void updateInfo(RoomInfo & info);

private:
	cocos2d::Node * m_rootNode;
	cocos2d::ui::Text* m_roomID;
	cocos2d::ui::Text* m_roomName;
	cocos2d::ui::Text* m_playerNumber;
	cocos2d::ui::Text* m_roomStatu;
};

class CHallLayer
	: public CBaseLayer<CHallLayer>
{
public:
	CHallLayer();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	void initEnterRoomDialog();

public:
	void updateRoomList(cocos2d::EventCustom * event);

public:
	void createRoomCallback(cocos2d::Ref * target);
	void enterRoomCallback(cocos2d::Ref * target);
	void dialogEnterRoomCallback(cocos2d::Ref * target);
	void dialogCancleRoomCallback(cocos2d::Ref * target);

	void roomListViewTouchListener(cocos2d::Ref * ref, cocos2d::ui::Widget::TouchEventType type);

private:
	void sendEnterRoomMessage(int iRoomId);

private:


private:
	cocos2d::Node * m_layerGraphNode;
	cocos2d::ui::ListView * m_roomList;
	cocos2d::Node * m_enterRoomInputDialog;

private:
	cocos2d::EventListenerTouchOneByOne * m_dialogLayerTouchListener;
};

#endif


