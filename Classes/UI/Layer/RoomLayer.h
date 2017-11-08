#ifndef _ROOM_LAYER_H
#define _ROOM_LAYER_H

#include "BaseLayer.h"
#include <ui/CocosGUI.h>
#include <table/table.pb.h>

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
	void initMsg();
	void initCallLandlordLayer();
	void initPlayCardLayer();
	void initHandCardPanel();
	virtual void onEnter();
	virtual void onExit();

private:
	// Button click listeners
	void onExitRoomCallback(cocos2d::Ref * target);
	void onReadyClickListener(cocos2d::Ref * target);
	void onCallLandlordClickListener(cocos2d::Ref * target);
	void onNotCallLandlordClickListener(cocos2d::Ref * target);
	void onPlayClickListener(cocos2d::Ref * target);
	void onNotPlayClickListener(cocos2d::Ref * target);
	void onReelectClickListener(cocos2d::Ref * target);

private:
	// Event
	void onPlayerEnterInRoom(cocos2d::EventCustom * event);
	void onPlayerReady(cocos2d::EventCustom * event);
	void onGameStart(cocos2d::EventCustom * event);
	void onInitPoker(cocos2d::EventCustom * event);
	void onCallLandlordResult(cocos2d::EventCustom * event);
	void onShowLandlordPoker(cocos2d::EventCustom * event);
	void onUpdateCurrentPoker(cocos2d::EventCustom * event);
	void onPlaySuccess(cocos2d::EventCustom * event);
	void onShowOtherPlayerPoker(cocos2d::EventCustom * event);
	void onGameOver(cocos2d::EventCustom * event);
	void onPlayerOut(cocos2d::EventCustom * event);
	void onNoOneCallLandlord(cocos2d::EventCustom * event);

	void handCardPanelTouchListener(cocos2d::Ref * ref, cocos2d::ui::Widget::TouchEventType type);
	
private:
	int computeClientPosition(int serverPosition);
	void updateCallLandlordMsg(int serverPosition, int msg, bool call, bool show = true);
	void showNoPlay(int serverPosition, bool show);
	void updateCallLandlordButton(bool isCall, bool show = true);
	void showTimeClock(int serverPosition, bool show);
	void showPlayButton(bool show);
	void updateCurrentPoker(const ::google::protobuf::RepeatedPtrField<zhu::table::Poker> & pokers);
	void updatePlayPoker(const ::google::protobuf::RepeatedPtrField<zhu::table::Poker> & pokers, int position);
	void updatePlayerPokerNumber(int serverPosition, int number);
	void reelectPoker();
	void updateHead();
	void hideAllMsg();
	void hideAllClock();
	void clearPlayPoker(int position);
	void clearAllPlayPoker();
	void reinit();
	void clearAllHandPoker();

private:
	cocos2d::Node * m_layerGraphNode;

	cocos2d::Layer * m_callLandlordLayer;
	cocos2d::Layer * m_playCardLayer;
	cocos2d::ui::Layout * m_currentCardPanel;

	std::vector<cocos2d::Node *> m_players;
	std::vector<cocos2d::Sprite *> m_playerMsgs;
	std::vector<cocos2d::Node *> m_playCardPanel;
	std::list<cocos2d::Node *> m_currentPokers;

	cocos2d::ui::Button * m_readyButton;
	cocos2d::ui::Button * m_callLandlordButton;
	cocos2d::ui::Button * m_notCallLandlordButton;
	cocos2d::ui::Button * m_playButton;
	cocos2d::ui::Button * m_notPlayButton;
	cocos2d::ui::Button * m_reelectButton;

private:
	int m_landlordServerPosition;
	int m_noPlayFlag;
	bool m_start;
};

#endif