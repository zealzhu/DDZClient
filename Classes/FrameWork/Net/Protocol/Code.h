////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/25
// Describe：协议类,用来编码解码数据包
////////////////////////////////////////////////////////////////////////
#ifndef _CODE_H
#define _CODE_H

#include <google/protobuf/message.h>
#include <BaseMsg.pb.h>

#define HEADER_SIZE 4

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

class ProtocolCode {

public:
	// 错误枚举
	enum ErrorCode
	{
		SUCCESS = 0,
		INVALID_NAME_LEN,
		UNKNOWN_MESSAGE_TYPE,
		PARSE_ERROR,
	};

public:
	
	// 编码
	static const char * encode(const google::protobuf::Message & message, int & iBuffSize);

	// 解码
	static google::protobuf::Message * decode(char * pBuf);

	// 读取头部
	static unsigned int readHdr(char * pBuf);

	// 创建消息
	static google::protobuf::Message * createMessage(const std::string & strTypeName);

	// 解析Message成真正有用的数据包
	static MessagePtr parseInnerMsg(std::shared_ptr<zhu::SelfDescribingMessage> pOutterMsg, ErrorCode& error);
};

#endif // !_CODE_H
