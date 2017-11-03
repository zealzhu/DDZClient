#include "RayCommonFunction.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////////
//function
#ifdef _WINDOWS_
std::wstring RayFunction::StringUtf8ToWideChar(const std::string& strUtf8)
{
	std::wstring ret;
	if (!strUtf8.empty())
	{
		int nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, nullptr, 0);
		if (nNum)
		{
			WCHAR* wideCharString = new WCHAR[nNum + 1];
			wideCharString[0] = 0;

			nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wideCharString, nNum + 1);

			ret = wideCharString;
			delete[] wideCharString;
		}
		else
		{
			CCLOG("Wrong convert to WideChar code:0x%x", GetLastError());
		}
	}
	return ret;
}

std::string RayFunction::StringWideCharToUtf8(const std::wstring& strWideChar)
{
	std::string ret;
	if (!strWideChar.empty())
	{
		int nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
		if (nNum)
		{
			char* utf8String = new char[nNum + 1];
			utf8String[0] = 0;

			nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, utf8String, nNum + 1, nullptr, FALSE);

			ret = utf8String;
			delete[] utf8String;
		}
		else
		{
			CCLOG("Wrong convert to Utf8 code:0x%x", GetLastError());
		}
	}

	return ret;
}

std::string RayFunction::UTF8StringToMultiByte(const std::string& strUtf8)
{
	std::string ret;
	if (!strUtf8.empty())
	{
		std::wstring strWideChar = StringUtf8ToWideChar(strUtf8);
		int nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
		if (nNum)
		{
			char* ansiString = new char[nNum + 1];
			ansiString[0] = 0;

			nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, ansiString, nNum + 1, nullptr, FALSE);

			ret = ansiString;
			delete[] ansiString;
		}
		else
		{
			CCLOG("Wrong convert to Ansi code:0x%x", GetLastError());
		}
	}

	return ret;
}

std::wstring RayFunction::charToWchar(const char* chs)
{
	std::wstring ret;
	//将char* 转成wchar_t*的实现函数如下：
	//这是把asii字符转换为unicode字符，和上面相同的原理
	if (chs)
	{
		size_t nNum = (size_t)MultiByteToWideChar(CP_ACP, 0, (const char *)chs, -1, NULL, 0);//检查转换后到底有多少位的wchar。得出nNum为下面的转换函数使用
		WCHAR pwstr[1024] = { 0 };
		memset(pwstr, 3, sizeof(WCHAR) * 1024);
		if (nNum)
		{
			MultiByteToWideChar(CP_ACP, 0, (const char *)chs, -1, pwstr, nNum);
			ret = pwstr;
		}
	}

	return ret;
}

std::string RayFunction::WideCharToChar(const std::wstring& wstring)
{
	char utf8ExeDir[1024] = { 0 };
	int nNum = WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), -1, utf8ExeDir, sizeof(utf8ExeDir), nullptr, nullptr);
	std::string ret = utf8ExeDir;
	return ret;
}

void RayFunction::TraversingFile(const char* RelativePath, std::vector<std::string>& filesVector)
{

	const unsigned int PATH_MAX = 1024;
	WIN32_FIND_DATA FindData;
	HANDLE hError;
	int FileCount = 0;
	char FilePathName[PATH_MAX];
	// 构造路径
	WCHAR FullPathName[PATH_MAX];
	strcpy(FilePathName, RelativePath);
	strcat(FilePathName, "\\*.*");
	std::wstring wFilePathName = charToWchar(FilePathName);

	hError = FindFirstFile(wFilePathName.c_str(), &FindData);
	if (hError == INVALID_HANDLE_VALUE)
	{
		CCLOG("搜索失败!");
		return;
	}
	while (::FindNextFile(hError, &FindData))
	{
		// 过虑.和..
		if (wcscmp(FindData.cFileName, L".") == 0
			|| wcscmp(FindData.cFileName, L"..") == 0)
		{
			continue;
		}

		// 构造完整路径
		wsprintf(FullPathName, L"%s\\%s", charToWchar(RelativePath).c_str(), FindData.cFileName);
		FileCount++;

		// 输出本级的文件
		std::string fileNameChange = WideCharToChar(FullPathName);
		CCLOG("\n%d  %s  ", FileCount, WideCharToChar(FullPathName).c_str());

		//是目录的话，递归
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//printf("<Dir>");
			TraversingFile(WideCharToChar(FullPathName).c_str(), filesVector);
		}
		else
		{
			filesVector.push_back(fileNameChange);
		}
	}
}

bool RayFunction::getNumberAtPos(const int target, int atPos)
{
	//这个可以用，但是有可能数据溢出
	//int nowTarget = target >> atPos;
	//bool ret = nowTarget & 1;

	int curNumber = 1 << atPos;
	bool ret = (curNumber & target) != 0;
	return ret;
}

#endif//_WINDOWS_
////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long long int RayFunction::getCurrentUTime()
{
	unsigned long long int retULongTime = 0;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
// 	LARGE_INTEGER t1, t2, feq;
// 	QueryPerformanceFrequency(&feq);//每秒跳动次数 
// 	QueryPerformanceCounter(&t1);//测前跳动次数 
// 	for (int i = 0; i < 10000; i++);
// 	QueryPerformanceCounter(&t2);//测后跳动次数 
// 	double d = ((double)t2.QuadPart - (double)t1.QuadPart) / ((double)feq.QuadPart);//时间差秒 
	
#if 0
	//2dx的 CCStdC-wind32中的获取时间的 2B实现方案，只能获取当前cpu运行时间
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency(&liFreq);
		QueryPerformanceCounter(&liTime);
		val->tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
		val->tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0);
	}
#endif

	clock_t clockTime;
	clockTime = clock();
	time_t timeClock;
	time(&timeClock);

	SYSTEMTIME wtm;
	GetLocalTime(&wtm);

	unsigned int beginYear = 1970;
	struct tm tTm;
	tTm.tm_year		= wtm.wYear - beginYear;
	tTm.tm_mon		= wtm.wMonth - 1;
	tTm.tm_mday		= wtm.wDay;
	tTm.tm_hour		= wtm.wHour;
	tTm.tm_min		= wtm.wMinute;
	tTm.tm_sec		= wtm.wSecond;
	
	tTm.tm_isdst	= -1;

	//算出闰年
	unsigned int addDay = 0;//遇到闰年多出的天数



#if 0
	struct timeval val;
	val.tv_sec		= (unsigned long long int)mktime(&tTm);       // time_t is 64-bit on win32
	val.tv_usec		= wtm.wMilliseconds * 1000;
#endif



#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	retULongTime = tv.tv_sec;
	retULongTime *= 1000000;
	retULongTime += tv.tv_usec;

#endif
	return retULongTime;
}

unsigned int RayFunction::getCurrentMTime()
{
	return getCurrentUTime() / 1000;
}

cocos2d::Point RayFunction::b2Vec2ConverToPoint(const b2Vec2& v0)
{
	cocos2d::Point point;
	point.x = v0.x;
	point.y = v0.y;
	return point;
}

b2Vec2 RayFunction::PointConverTob2Vec2(const cocos2d::Point& p0)
{
	b2Vec2 v0;
	v0.x = p0.x;
	v0.y = p0.y;
	return v0;
}


cocos2d::Point RayFunction::angleDistancePoint(const cocos2d::Point& beginPoint, const float& radian, const float& distance)
{
	cocos2d::Point newPosition;
	//float angle = _graphHandle->getRotation();
	//float radian = angle / 180.f * M_PI;
	
	float x = distance * cos(radian);
	float y = distance * sin(radian);
	newPosition = beginPoint + cocos2d::Point(x, y);
	
	return newPosition;
}

double RayFunction::getPointDistance(const cocos2d::Point& A, const cocos2d::Point& B)
{
	double powNumber = pow((A.x - B.x), 2) + pow((A.y - B.y), 2);
	return sqrt(powNumber);
}
bool RayFunction::isPointInNode(cocos2d::Point worldPoint, cocos2d::Node* node)
{
	cocos2d::Rect rect;
	rect.size = node->getContentSize();
	cocos2d::Mat4 mat4Vec = node->getWorldToNodeTransform();

	return isScreenPointInRect(worldPoint, cocos2d::Camera::getVisitingCamera(), mat4Vec, rect, nullptr);
}
bool RayFunction::isBoxInCircle(cocos2d::Point circleCenterPoint, float circleRadius, cocos2d::Rect boxRect)
{
	//向量，英文vector

	//判断原则用 锐式检测算法.
	//1.判断前提，矩形必须是正矩形，不能倾斜的。
	//算法原理：矩形中心到圆心的距离，其实是 矢量X的平方和矢量Y的平方 之和 开根号。

	cocos2d::Point boxCenter = getRectCenterPoint(boxRect);
	cocos2d::Point boxRightTopPoint = boxRect.origin + boxRect.size;//矩形右上角的坐标
	cocos2d::Vec2 VDistance = cocos2d::Vec2(fabs(circleCenterPoint.x - boxCenter.x), fabs(circleCenterPoint.y - boxCenter.y));//向量V
	cocos2d::Vec2 HDistance = cocos2d::Vec2(boxRightTopPoint.x - boxCenter.x, boxRightTopPoint.y - boxCenter.y);//矩形中心点到矩形右上角的点的距离。

	cocos2d::Vec2 UDistance = VDistance - HDistance;


	return true;
}

float RayFunction::getA2BAngle(const cocos2d::Point& A, const cocos2d::Point& B)
{
	return radianToAngle(getA2BRadian(A, B));
}
float RayFunction::getA2BRadian(const cocos2d::Point& A, const cocos2d::Point& B)
{
	cocos2d::Point dis = B - A;
	//float disY = (B.y - A.y);
	//float disX = (B.x - A.x);//注意，disX = 0会出错
	if (dis.x == 0.f)
	{
		if (dis.y != 0.f)
		{
			if (dis.y > 0.f)
			{
				//正无穷大
				return float(M_PI / 2.0f);
			}
			else
			{
				return float(M_PI * 1.5f);
			}
		}
		else
		{
			return 0;
		}
	}
	float Radian = atanf(dis.y / dis.x);
	
	if (dis.x < 0.f)
	{
		Radian += (float)M_PI;
	}

	return Radian;
}
float RayFunction::radianToAngle(float radian)
{
	return radian / M_PI * 180.0f;
}

float RayFunction::angleToRadian(float angle)
{
	return angle / 180.f * M_PI;
}

cocos2d::Point RayFunction::getRectCenterPoint(cocos2d::Rect rect)
{
	float x = rect.size.width / 2;
	float y = rect.size.height / 2;
	cocos2d::Point centerPoint = rect.origin + cocos2d::Point(x, y);
	return centerPoint;
}

void RayFunction::fixDistancePoint(cocos2d::Point& point, const float& distance)
{
	float currentDistance = pow(point.x, 2) + pow(point.y, 2);
	if (currentDistance > pow(distance,2))
	{
		//距离超出，修正数值
		float tanAngel = getA2BRadian(cocos2d::Point::ZERO, point);
		point.y = sinf(tanAngel)*distance;
		point.x = cosf(tanAngel)*distance;
	}
}

////////////////////////////////////change char to number;///////////////////////////////////////
int RayFunction::atoiSafe(const char* inputString)
{
	if (inputString)
	{
		return atoi(inputString);
	}

	return 0;
}

float RayFunction::atofSafe(const char* inputString)
{
	return cocos2d::utils::atof(inputString);
}

void RayFunction::systemScreenShoot(std::function<void(bool, const std::string&)> saveFileCallBack, std::string fileName)
{
	cocos2d::utils::captureScreen(saveFileCallBack, fileName);
}

cocos2d::Image* RayFunction::systemNodeCapture(cocos2d::Node* node, float scale)
{
	cocos2d::Image* image = cocos2d::utils::captureNode(node, scale);
	return image;
}

std::string RayFunction::saveNodeImage(cocos2d::Node* node)
{
	
    cocos2d::Image* image   = systemNodeCapture(node);

    std::string filePath    = cocos2d::FileUtils::getInstance()->getWritablePath();
	filePath = cocos2d::StringUtils::format("%sNodeShoot%d.png", filePath.c_str(), getCurrentMTime());
    image->saveToFile(filePath);

	return filePath;
}

int RayFunction::getTime()
{
	return cocos2d::utils::gettime();
}

long long RayFunction::getTimeInMilliseconds()
{
	return cocos2d::utils::getTimeInMilliseconds();
}



cocos2d::Label* RayFunction::createDefaultLabel(float fontSize /*= 30*/)
{
	//自动换行效果好的，请参考2dx的LabelFNTMultiLineAlignment()的代码，用LabelBMFont,正常的，单词结束后面加空格都能换行比较好。前提还有要设置MaxLineWidth
	//发送文字加图片（表情）的组合，请参考LabelTestNew.cpp中的LabelAddChildTest
	auto _textLabel = cocos2d::Label::createWithTTF("", "cocosui/UImg/fnt/ccxyt.TTF", fontSize);//"fonts/arial.ttf"

	//_textLabel = Label::createWithBMFont(, "", TextHAlignment::CENTER);//注意，字体一定要是.fnt字体。不然会出错
	_textLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	_textLabel->setMaxLineWidth(300);
	auto colorOutLine = cocos2d::Color4B(139, 72, 20, 255);
	_textLabel->enableOutline(colorOutLine, 2);

	return _textLabel;
}

std::string RayFunction::getStringTimeFromInt64(unsigned long long time64Data)
{
	time_t ttime			= (time_t)time64Data;
	struct tm *tmtime		= localtime(&ttime);
	std::string strYear		= toString(tmtime->tm_year + 1900);
	std::string strMon		= toString(tmtime->tm_mon + 1);
	std::string strDay		= toString(tmtime->tm_mday);
	std::string strHour		= toString(format("%02d", tmtime->tm_hour));
	std::string strMin		= toString(format("%02d", tmtime->tm_min));
	std::string strSec		= toString(format("%02d", tmtime->tm_sec));
	std::string TimeString	= (strYear + "/" + strMon + "/" + strDay + " " + strHour + ":" + strMin + ":" + strSec);

	return TimeString;
}

std::vector<cocos2d::Point> RayFunction::getRectPoints(const cocos2d::Rect& rect)
{
	const unsigned int RectPointCount = 4;
	std::vector<cocos2d::Point> vecPoints;
	vecPoints.push_back(cocos2d::Point(rect.origin));
	vecPoints.push_back(cocos2d::Point(rect.origin.x + rect.size.width, rect.origin.y));
	vecPoints.push_back(cocos2d::Point(rect.origin.x, rect.origin.y + rect.size.height));
	vecPoints.push_back(cocos2d::Point(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height));

	return vecPoints;
}
#if 0
cocos2d::Point RayFunction::isLineTouchLine(const cocos2d::Point& lineAPointBegin, const cocos2d::Point& lineAPointEnd, const cocos2d::Point& lineBPointBegin, const cocos2d::Point& lineBPointEnd)
{
	float AXLineA = lineAPointEnd.x - lineAPointBegin.x;
	float AYLineA = lineAPointEnd.y - lineAPointBegin.y;
	if (AXLineA == 0)
	{
		// lineAX = lineAPointEnd.x;
		float lineAX = lineAPointEnd.x;

	}else if (AYLineA == 0)
	{
		// lineAY = lineAPointEnd.y;
		float lineAY = lineAPointEnd.y;
	}
	else
	{
		float ScaleXY = AYLineA / AXLineA;
		float X = 0;
		//ScaleXY * X == Y + N;
	}



	

	return Point::ZERO;
}
#endif
bool areVecsEqual(b2Vec2 v1, b2Vec2 v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}

bool RayFunction::linesCross(b2Vec2 v0, b2Vec2 v1, b2Vec2 t0, b2Vec2 t1, b2Vec2 &intersectionPoint)
{
	if (areVecsEqual(v1, t0) ||
		areVecsEqual(v0, t0) ||
		areVecsEqual(v1, t1) ||
		areVecsEqual(v0, t1))
		return false;

	b2Vec2 vnormal = v1 - v0;
	vnormal = b2Cross(1.0f, vnormal);
	float v0d = b2Dot(vnormal, v0);
	float t0d = b2Dot(vnormal, t0);
	float t1d = b2Dot(vnormal, t1);
	if (t0d > v0d && t1d > v0d)
		return false;
	if (t0d < v0d && t1d < v0d)
		return false;

	b2Vec2 tnormal = t1 - t0;
	tnormal = b2Cross(1.0f, tnormal);
	t0d = b2Dot(tnormal, t0);
	v0d = b2Dot(tnormal, v0);
	float v1d = b2Dot(tnormal, v1);
	if (v0d > t0d && v1d > t0d)
		return false;
	if (v0d < t0d && v1d < t0d)
		return false;

	intersectionPoint = v0 + ((t0d - v0d) / (v1d - v0d)) * (v1 - v0);

	return true;
}

bool RayFunction::isRectTouchRect(const cocos2d::Rect& A, const cocos2d::Rect& B)
{
	//检测原理。A,和B矩形如果有重叠区域，那么A,B矩形中必定至少有一方包含另一方的顶点。于是用顶点算法。
	std::vector<cocos2d::Point> vecPoints = getRectPoints(A);

	for (auto& iter : vecPoints)
	{
		if (B.containsPoint(iter))
		{
			return true;
		}
	}

	vecPoints.clear();
	vecPoints = getRectPoints(B);

	for (auto& iter : vecPoints)
	{
		if (A.containsPoint(iter))
		{
			return true;
		}
	}

	return false;
}
#if 0
cocos2d::Point RayFunction::isLineTouchRect(const cocos2d::Point& linePointA, const cocos2d::Point& linePointB, const cocos2d::Rect& taRect)
{
	float lineFunction,	lineYScale, lineXScale,	y, x;

	if (linePointA.x == linePointB.x)
	{
		lineFunction = y;
	}else if (linePointA.y == linePointB.y)
	{
		lineFunction = x;
	}

	
	return Point::ZERO;
}
#endif


std::string RayFunction::format(const char* format, ...)
{
#define CC_MAX_STRING_LENGTH (1024*100)

	std::string ret;

	va_list ap;
	va_start(ap, format);

	char* buf = (char*)malloc(CC_MAX_STRING_LENGTH);
	if (buf != nullptr)
	{
		vsnprintf(buf, CC_MAX_STRING_LENGTH, format, ap);
		ret = buf;
		free(buf);
	}
	va_end(ap);

	return ret;
}