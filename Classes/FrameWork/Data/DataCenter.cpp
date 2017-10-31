#include "DataCenter.h"
#include "Protocol/Code.h"
#include <cocos2d.h>
#include <user/player.pb.h>

USING_NS_CC;
using namespace zhu;
using namespace std;

CDataCenter::CDataCenter()
{
}


CDataCenter::~CDataCenter()
{
}

bool CDataCenter::init()
{
	return true;
}

void CDataCenter::handle(std::shared_ptr<SelfDescribingMessage> pMsg)
{
	ProtocolCode::ErrorCode errorCode;
	auto pInnerMsg = ProtocolCode::parseInnerMsg(pMsg, errorCode);

	if (ProtocolCode::ErrorCode::UNKNOWN_MESSAGE_TYPE == errorCode || ProtocolCode::ErrorCode::PARSE_ERROR == errorCode)
	{
		// ½âÎöÏûÏ¢Ê§°Ü
		log("ProtocolCode parseInnerMsg failed");
		return;
	}

	if ("zhu.user.LoginResp" == pMsg->type_name()) {
		dealWithLoginResponse(pInnerMsg);
	}
}

void CDataCenter::dealWithLoginResponse(MessagePtr pMsg)
{
	shared_ptr<zhu::user::LoginResp> pLoginResp = dynamic_pointer_cast<zhu::user::LoginResp>(pMsg);
	if (pLoginResp->loginresult() == zhu::user::ERROR_CODE::SUCCESS) {
		// µÇÂ¼³É¹¦
		m_strAccount = pLoginResp->account();
		m_userId = pLoginResp->id();
		log("login success");
	}
	//µÇÂ¼Ê§°Ü
	else {
		log("login failed:%s", pLoginResp->desc().c_str());
	}
}
