#include "UIDataDefine.h"
#include "IncludeLayersHead.h"
#include "UIManager.h"

void layerCreateInit(void * mapCreateFun)
{
	MAP_LAYER_CREATE_FUNC * mapCreateFunHandle = (MAP_LAYER_CREATE_FUNC *)mapCreateFun;
	mapCreateFunHandle->insert(std::make_pair(ENUM_TOP_LAYER, FUN_LAYER_CREATE(CTopLayer::create)));
	mapCreateFunHandle->insert(std::make_pair(ENUM_LOGIN_LAYER, FUN_LAYER_CREATE(CLoginLayer::create)));
	mapCreateFunHandle->insert(std::make_pair(ENUM_HALL_LAYER, FUN_LAYER_CREATE(CHallLayer::create)));
	mapCreateFunHandle->insert(std::make_pair(ENUM_ROOM_LAYER, FUN_LAYER_CREATE(CRoomLayer::create)));
}
