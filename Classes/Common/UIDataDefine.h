#pragma once
#ifndef _UI_DATA_DEFINE_H
#define _UI_DATA_DEFINE_H

// Layer Ã¶¾Ù
typedef enum {
	ENUM_LOADING_LAYER,			// ¼ÓÔØ
	ENUM_LOGIN_LAYER,			// µÇÂ¼
	ENUM_HALL_LAYER,			// ´óÌü
} ENUM_UI_LAYER;


extern void layerCreateInit(void * mapCreateFun);
#endif