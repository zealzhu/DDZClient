#pragma once
#ifndef _UI_DATA_DEFINE_H
#define _UI_DATA_DEFINE_H

// Layer 枚举
typedef enum {
	ENUM_TOP_LAYER,				// 加载
	ENUM_LOGIN_LAYER,			// 登录
	ENUM_HALL_LAYER,			// 大厅
	ENUM_ROOM_LAYER,			// 房间
} ENUM_UI_LAYER;


extern void layerCreateInit(void * mapCreateFun);
#endif