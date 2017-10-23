//////////////////////////////////////////////////////////
//// Author:	Zhu 
//// Date:		2017/10/23
//// Describe:	´óÌüLayer
/////////////////////////////////////////////////////////
#pragma once
#ifndef _HALL_LAYER_H
#define _HALL_LAYER_H

#include "BaseLayer.h"

class CHallLayer
	: public CBaseLayer<CHallLayer>
{
public:
	CHallLayer();
	virtual bool init();
};

#endif


