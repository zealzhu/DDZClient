#pragma once
#ifndef _DATA_CENTER_H
#define _DATA_CENTER_H

#include "Common/Singleton.h"
#include "IModule.h"
#include "FrameWork/Net/Protocol/Code.h"
#include <list>
#include <memory>

// 房间信息
struct RoomInfo {
public:
	RoomInfo()
	{
		_roomID = 0;
		_roomName = "";
		_sitUserNum = 0;
		_statu = 0;

	}
	~RoomInfo() = default;
public:
	int									_roomID;
	short								_sitUserNum;			
	std::string							_roomName;				
	short								_statu;
};

// 玩家信息
struct PlayerInfo {
	int _userId;
	std::string _userName;
	std::string _userHead;
};

// 对局信息
struct GameOverInfo {
public:
	int _landlordPosition;
};

typedef std::shared_ptr<RoomInfo> ROOM_INFO_PTR;

class CDataCenter
	: public IModule
{
public:
	static CDataCenter * getInstance() {
		static CDataCenter instance;
		return &instance;
	}
	virtual bool init();
	virtual void handle(std::shared_ptr<zhu::SelfDescribingMessage> pTmpMsg);
	~CDataCenter();

public:
	void dealWithLoginResponse(MessagePtr pMsg);			// 处理登录响应

	void dealWithEnterRoomResponse(MessagePtr pMsg);		// 进入房间响应
	void dealWithCreateRoomResponse(MessagePtr pMsg);		// 创建房间响应
	void dealWithGetRoomResponse(MessagePtr pMsg);			// 获取房间响应
	void dealWithLeaveRoomResponse(MessagePtr pMsg);		// 离开房间响应
	void dealWithOtherEnterInRoom(MessagePtr pMsg);			// 其他玩家进入房间
	void dealWithPlayerReadyMsg(MessagePtr pMsg);			// 玩家准备消息
	void dealWithRoomStatuChangeMsg(MessagePtr pMsg);		// 房间状态改变
	void dealWithDispatchPokerMsg(MessagePtr pMsg);			// 分牌
	void dealWithCallLandlordMsg(MessagePtr pMsg);			// 请求地主
	void dealWithPlayResponse(MessagePtr pMsg);				// 出牌响应

public:
	std::list<ROOM_INFO_PTR> & getRoomInfoList();
	int getCurrentUserId();
	std::string & getUserAccount();
	int getCurrentRoomId();
	int getCurrentSeatPosition();

private:
	CDataCenter();
	
private:
	// 个人信息
	int m_userId;
	std::string m_userAccount;
	int m_currentRoomId;

	// 房间列表
	std::list<ROOM_INFO_PTR> m_roomInfoList;
	int m_currentSeatPosition;
};

#endif


