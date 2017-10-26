#include "UIManager.h"
#include "AppFunc.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

CUIManager::CUIManager()
{
	m_currentScene = NULL;
}

CUIManager::~CUIManager()
{
}

void CUIManager::initCocosSetting(const std::string strWindowsName)
{
	// initialize director
	auto glview = GDirector->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect(strWindowsName.c_str(), cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
		glview = GLViewImpl::create(strWindowsName.c_str());
#endif
		GDirector->setOpenGLView(glview);
	}

	// turn on display FPS
	GDirector->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	GDirector->setAnimationInterval(1.0f / 60);

	// Set the design resolution
	glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
	auto frameSize = glview->getFrameSize();
	// if the frame's height is larger than the height of medium size.
	if (frameSize.height > mediumResolutionSize.height)
	{
		GDirector->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
	}
	// if the frame's height is larger than the height of small size.
	else if (frameSize.height > smallResolutionSize.height)
	{
		GDirector->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
	}
	// if the frame's height is smaller than the height of medium size.
	else
	{
		GDirector->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
	}
}

bool CUIManager::init()
{

	layerCreateInit(&m_mapLayerCreateFunc);

	return true;
}

void CUIManager::initGraphBase()
{
	if (m_currentScene == NULL) {
		m_currentScene = Scene::create();	
		auto layer = createLayer(ENUM_HALL_LAYER);
		m_currentScene->addChild(layer);
		GDirector->runWithScene(m_currentScene);
	}
}

cocos2d::Layer * CUIManager::createLayer(ENUM_UI_LAYER layerId)
{
	Layer * layer = NULL;
	auto it = m_mapLayerCreateFunc.find(layerId);

	CCAssert(it != m_mapLayerCreateFunc.end(), "Layer not found.");

	// ����layer
	layer = it->second();

	return layer;
}