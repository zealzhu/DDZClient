#include "TopLayer.h"
#include "AppFunc.h"
#include <editor-support/cocostudio/CocoStudio.h>
#include <ui/CocosGUI.h>

USING_NS_CC;

CTopLayer::CTopLayer()
{
}


CTopLayer::~CTopLayer()
{
}

bool CTopLayer::init()
{
	if(!Layer::init())
		return false;

	// 初始化触摸事件
	this->setSwallowsTouches(false);
	this->addTouchListener();
	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);//单点触摸 

	// 创建加载圈
	createLoadingCircle();

	// 创建提示框
	createDialog();

	return true;
}

void CTopLayer::createLoadingCircle()
{
	m_loadingCircle = Sprite::create("loadingImg/loading01.png");
	m_loadingCircle->setVisible(false);
	m_loadingCircle->setPosition(GWinSize / 2);

	auto loadingCircleAnimation = Animation::create();
	for (int i = 1; i <= 12; i++) {
		loadingCircleAnimation->addSpriteFrameWithFile(StringUtils::format("loadingImg/loading%02d.png", i));
	}
	loadingCircleAnimation->setDelayPerUnit(0.2f);
	auto loadingCircleAnimate = Animate::create(loadingCircleAnimation);
	m_loadingCircle->runAction(RepeatForever::create(loadingCircleAnimate));

	this->addChild(m_loadingCircle);
}

void CTopLayer::showLoadingCircle()
{
	m_loadingCircle->setVisible(true);
	this->setSwallowsTouches(true);
}

void CTopLayer::hideLoadingCircle()
{
	m_loadingCircle->setVisible(false);
	this->setSwallowsTouches(false);
}

void CTopLayer::createDialog()
{
	m_dialog = CSLoader::createNode("Node/Dialog.csb");
	m_dialog->setPosition(GWinSize / 2);
	m_dialog->setVisible(false);
	auto btnOk = (ui::Button*)m_dialog->getChildByName("btn_ok");
	btnOk->addClickEventListener([this](Ref *) {
		hideDialog();
	});
	this->addChild(m_dialog);
}

void CTopLayer::showDialog(std::string strTitle, std::string strMessage, std::function<void(Ref*)> callBack)
{
	m_dialog->setVisible(true);
	auto title = (ui::Text*)m_dialog->getChildByName("txt_title");
	title->setString(strTitle);

	auto message = (ui::Text*)m_dialog->getChildByName("txt_message");
	message->setString(strMessage);

	if (callBack != NULL) {
		auto btnOk = (ui::Button*)m_dialog->getChildByName("btn_ok");
		btnOk->addClickEventListener(callBack);
	}
}

void CTopLayer::hideDialog()
{
	m_dialog->setVisible(false);
}

bool CTopLayer::onTouchBegan(Touch * touch, Event * unused_event)
{
	return true;
}
