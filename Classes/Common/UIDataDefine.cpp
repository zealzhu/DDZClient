#include "UIDataDefine.h"
#include "IncludeLayersHead.h"
#include "UIManager.h"

void layerCreateInit(void * mapCreateFun)
{
	MAP_LAYER_CREATE_FUNC * mapCreateFunHandle = (MAP_LAYER_CREATE_FUNC *)mapCreateFun;
	mapCreateFunHandle->insert(std::make_pair(ENUM_HALL_LAYER, FUN_LAYER_CREATE(CHallLayer::create)));
}
