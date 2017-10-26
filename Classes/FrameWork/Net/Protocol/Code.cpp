#include "Code.h"
#include <WinSock2.h>
#include "google/protobuf/message.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <BaseMsg.pb.h>
#include <Basic.h>

using namespace google::protobuf;
using namespace google::protobuf::io;

const std::string STR_SUCCESS = "Success";
const std::string STR_INVALID_NAME_LEN = "InvalidNameLen";
const std::string STR_UNKNOWN_MESSAGE_TYPE = "UnknownMessageType";
const std::string STR_PARSE_ERROR = "ParseError";
const std::string STR_UNKNOWN_ERROR = "UnknownError";

const char * ProtocolCode::encode(const google::protobuf::Message & message, int & iBuffSize)
{
	// 4个字节的头部（长度）+数据包内容长度
	iBuffSize = message.ByteSize() + HEADER_SIZE;

	// 创建一个该大小的缓存区
	char * pBuf = new char[iBuffSize];

	// 清空缓冲区
	ZeroMemory(pBuf, iBuffSize);

	// 将本地字节流转换为网络字节流
	unsigned int iNumber = htonl(message.ByteSize());

	// 头四个字节用来存储数据包大小
	memcpy(pBuf, &iNumber, HEADER_SIZE);

	// 以下就是将protobuf数据序列化到内存中
	ArrayOutputStream aos(pBuf + HEADER_SIZE, message.ByteSize());
	CodedOutputStream pCodedOutput(&aos);
	message.SerializeToCodedStream(&pCodedOutput);

	return pBuf;
}

MessagePtr ProtocolCode::decode(SocketLib::DataSocket * cSock, unsigned int iSize)
{
	unsigned uiByteCount = 0;
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(new zhu::SelfDescribingMessage());
	char * pBuf = new char[iSize];
	ZeroMemory(pBuf, iSize);

	unsigned uiTotal = 0;
	while (uiTotal < iSize && (uiByteCount = recv(cSock->GetSock(), pBuf + uiByteCount, iSize - uiByteCount, 0) > 0))
	{
		if (uiByteCount == -1)
		{
			throw SocketLib::Exception(SocketLib::GetError());
		}

		uiTotal += uiByteCount;
	}
	//Assign ArrayInputStream with enough memory 
	google::protobuf::io::ArrayInputStream ais(pBuf, iSize);
	CodedInputStream codedInput(&ais);
	//De-Serialize
	pPayload->ParseFromCodedStream(&codedInput);
	pPayload->add_socket(cSock->GetSock());

	SAFE_DELETE(pBuf);

	return pPayload;
}

unsigned int ProtocolCode::readHdr(char * pBuf)
{
	unsigned int iSize;

	//Decode the HDR and get the size
	//iSize = (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | pBuf[3];	
	// 如果不使用unsigned int会导致长度不够
	iSize = ntohl(*(unsigned int *)pBuf);
	//cout << "size of payload is " << iSize << endl;
	return iSize;
}

google::protobuf::Message * ProtocolCode::createMessage(const std::string & strTypeName)
{
	google::protobuf::Message* pMessage = NULL;
	const google::protobuf::Descriptor* pDescriptor =
		google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(strTypeName);
	if (pDescriptor)
	{
		const google::protobuf::Message* pPrototype =
			google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
		if (pPrototype)
		{
			pMessage = pPrototype->New();
		}
	}
	return pMessage;
}

MessagePtr ProtocolCode::parseInnerMsg(std::shared_ptr<zhu::SelfDescribingMessage> pOutterMsg, ErrorCode & error)
{
	MessagePtr pMessage(ProtocolCode::createMessage(pOutterMsg->type_name()));
	if (pMessage)
	{
		if (pMessage->ParseFromString(pOutterMsg->message_data()))
		{
			error = SUCCESS;
		}
		else
		{
			error = PARSE_ERROR;
		}
	}
	else
	{
		error = UNKNOWN_MESSAGE_TYPE;
	}

	return pMessage;
}

const std::string & ProtocolCode::ErrorCodeToString(ErrorCode errorCode)
{
	switch (errorCode)
	{
	case SUCCESS:
		return STR_SUCCESS;
	case INVALID_NAME_LEN:
		return STR_INVALID_NAME_LEN;
	case UNKNOWN_MESSAGE_TYPE:
		return STR_UNKNOWN_MESSAGE_TYPE;
	case PARSE_ERROR:
		return STR_PARSE_ERROR;
	default:
		return STR_UNKNOWN_ERROR;
	}
}
