//
// Created by zhu on 2017/7/10.
//

#ifndef _APPFUNC_H
#define _APPFUNC_H

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

/**
* director
*/
#define GDirector (Director::getInstance())

/**
* running scene
*/
#define GRunningScene (GDirector->getRunningScene())

/**
*
*/
#define GScheduler (GDirector->getScheduler())

/**
* size
*/
#define GFrameSize (GDirector->getOpenGLView()->getFrameSize())
#define GWinSize (GDirector->getWinSize())

/*
* AnimationCache
*/
#define GAnimationCache (AnimationCache::getInstance())

/*
* TextureCache
*/
#define GTextureCache (GDirector->getTextureCache())

/**
* SimpleAudioEngine
*/
#define GSimpleAudioEngine (CocosDenshion::SimpleAudioEngine::getInstance())

/*
* SpriteFrameCache
*/
#define GSpriteFrameCache (SpriteFrameCache::getInstance())

#define GDesignResolutionSize (cocos2d::Size(720, 1280))

#define GEventDispatch (GDirector->getEventDispatcher())


/*
* 弹出对话框动画
* 默认修复锚点在(0,0)点的问题
*/
static void OpenNodeAnimation(Node *node, float duration = 0.3, bool isFix = true)
{
	std::function<void()> callback;
	if (isFix) {
		Vec2 originAnchor = node->getAnchorPoint();
		Vec2 originAnchorInPoints = node->getAnchorPointInPoints();
		Vec2 originPos = node->getPosition();
		Vec2 originSize = node->getContentSize();
		Vec2 deltaSize = originSize / 2 - originAnchorInPoints;
		Vec2 newPos = originPos + deltaSize;
		node->setAnchorPoint(Vec2(0.5, 0.5));
		node->setPosition(newPos);
		callback = [node, originAnchor, originPos]() {
			node->setAnchorPoint(originAnchor);
			node->setPosition(originPos);
		};
	}
	else {
		callback = []() {};
	}
	node->setScale(0);
	node->setOpacity(0);
	ActionInterval *acScale = EaseBackOut::create(ScaleTo::create(duration, 1.0));
	ActionInterval *acFadeIn = FadeIn::create(duration);
	ActionInterval *acIn = Spawn::create(acScale, acFadeIn, NULL);
	CallFunc *acReset = CallFunc::create(callback);
	node->runAction(Sequence::create(acIn, acReset, NULL));
}

/*
* 退出对话框动画
* 默认修复锚点在(0,0)点的问题
*/
static void CloseNodeAnimation(Node *node, const std::function<void()>& endCallback, float duration = 0.3, bool isFix = true)
{
	if (isFix) {
		Vec2 originAnchor = node->getAnchorPoint();
		Vec2 originAnchorInPoints = node->getAnchorPointInPoints();
		Vec2 originPos = node->getPosition();
		Vec2 originSize = node->getContentSize();
		Vec2 deltaSize = originSize / 2 - originAnchorInPoints;
		Vec2 newPos = originPos + deltaSize;
		node->setAnchorPoint(Vec2(0.5, 0.5));
		node->setPosition(newPos);
	}
	ActionInterval *acScale = EaseBackIn::create(ScaleTo::create(duration, 0));
	ActionInterval *acFadeOut = FadeOut::create(duration);
	ActionInterval *acOut = Spawn::create(acScale, acFadeOut, NULL);
	CallFunc *actionDidFinishedCallback = CallFunc::create(endCallback);
	node->runAction(Sequence::create(acOut, actionDidFinishedCallback, NULL));
}


#endif //_APPFUNC_H_