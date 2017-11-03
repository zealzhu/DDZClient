#ifndef _ROOM_LAYER_H
#define _ROOM_LAYER_H

#include "BaseLayer.h"
#include <ui/CocosGUI.h>

enum Player {
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
};

class CRoomLayer
	: public CBaseLayer<CRoomLayer>
{
public:
	CRoomLayer();
	virtual bool init();
	void initPlayer();
	virtual void onEnter();
	virtual void onExit();

private:
	void onExitRoomCallback(cocos2d::Ref * target);
	void readyClickListener(cocos2d::Ref * target);

private:
	void onPlayerEnterInRoom(cocos2d::EventCustom * event);
	void onPlayerReady(cocos2d::EventCustom * event);

private:
	int computeClientPosition(int serverPosition);

private:
	cocos2d::Node * m_layerGraphNode;
	std::vector<cocos2d::Node *> m_players;
	cocos2d::ui::Button * m_readyButton;
};

#endif

