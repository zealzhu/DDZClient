#ifndef RAYCOMMONFUNCTION_H
#define RAYCOMMONFUNCTION_H
#include <cocos2d.h>
#include <ui/CocosGUI.h>
#include <Box2D/Box2D.h>
#include <functional>
#define CLASS_FUNCTION_CALLBACK_0(__selector__,__target__, ...) std::bind(&(__selector__),(__target__), ##__VA_ARGS__)
#define CLASS_FUNCTION_CALLBACK_1(__selector__,__target__, ...) std::bind(&(__selector__),(__target__), std::placeholders::_1, ##__VA_ARGS__)
#define CLASS_FUNCTION_CALLBACK_2(__selector__,__target__, ...) std::bind(&(__selector__),(__target__), std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CLASS_FUNCTION_CALLBACK_3(__selector__,__target__, ...) std::bind(&(__selector__),(__target__), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
typedef std::function<void()> FunctionCallBack;


namespace RayFunction 
{
	//文字功能，在cocos2dx下面，有很类似的功能，在\cocos2d\external\flatbuffers\util.h文件中。许多toInt，toString，还有转换utf8的功能
	//字符功能，在cocos2dx下面，有很类似的功能，在D:\RemoteValue\cocos2d\cocos\platform\win32\CCUtils-win32.h下，貌似只是win32使用，具体不得而知。
	//string功能，D:\RemoteValue\cocos2d\cocos\base\ccUTF8.h，有format等等的函数可以使用。
	//例如，CC_DLL long getCharacterCountInUTF8String(const std::string& utf8);计算UTF8文字个数（中文在size会被计算位3个，只有这个函数能检测是不是1个中文还是3个字母。）
#ifdef _WINDOWS_
	std::wstring					StringUtf8ToWideChar								(const std::string& strUtf8);
	std::string						StringWideCharToUtf8								(const std::wstring& strWideChar);
	std::string						UTF8StringToMultiByte								(const std::string& strUtf8);
	std::wstring					charToWchar											(const char* chs);
	std::string						WideCharToChar										(const std::wstring& wstring);


	//遍历文件列表
	//函数功能是查询 制定目录下的所有类型的文件。包括文件夹。
	//RelativePath，指的是相对路径或者真实路径，比如查询当前目录下的DATA目录，那么输入DATA即可。如果当前目录下的A目录，那么即DATA/A
	void							TraversingFile										(const char* RelativePath, std::vector<std::string>& filesVector);
#endif//_WINDOWS_


	//位运算函数
	bool							getNumberAtPos										(const int target,int atPos);
	
	//cocos2dx 3.x 相关功能
	cocos2d::Point					b2Vec2ConverToPoint									(const b2Vec2& v0);
	b2Vec2							PointConverTob2Vec2									(const cocos2d::Point& p0);

	std::vector<cocos2d::Point>		getRectPoints										(const cocos2d::Rect& rect);						//返回矩形的4个顶点
//	cocos2d::Point					isLineTouchLine										(const cocos2d::Point& lineAPointBegin, const cocos2d::Point& lineAPointEnd, const cocos2d::Point& lineBPointBegin, const cocos2d::Point& lineBPointEnd);
	bool							linesCross											(b2Vec2 v0, b2Vec2 v1, b2Vec2 t0, b2Vec2 t1, b2Vec2 &intersectionPoint);
	bool							isRectTouchRect										(const cocos2d::Rect& A, const cocos2d::Rect& B);	//A,B矩形是否碰撞的检测
	cocos2d::Point					isLineTouchRect										(const cocos2d::Point& linePointA, const cocos2d::Point& linePointB, const cocos2d::Rect& taRect);
	bool							isPointInNode										(cocos2d::Point worldPoint, cocos2d::Node* node);
	bool							isBoxInCircle										(cocos2d::Point circleCenterPoint, float circleRadius, cocos2d::Rect boxRect);
	
	//cocos2d::Vec2 类下的功能函数非常丰富。下面我自定义的函数，在2dx的vec2中也都有了。//2016.10.31
	//distance A-B的，就是Point A,B;float distance = A.distance(B);
	//length Point A;A.length();
	//angle 弧度的，Point A,B;float angle = coco2d::Point::angle(A,B);
	//rotate,getAngle,rotateByAngle
	//isLineIntersect,两条线是否相交
	double							getPointDistance									(const cocos2d::Point& A, const cocos2d::Point& B);//绝对>=0
	cocos2d::Point					angleDistancePoint									(const cocos2d::Point& beginPoint,const float& radian,const float& distance);
	float							getA2BAngle											(const cocos2d::Point& A, const cocos2d::Point& B);				//以A为圆心，B->A线段的角度
	float							getA2BRadian										(const cocos2d::Point& A, const cocos2d::Point& B);				//以A为圆心，B->A线段的弧度
	float							radianToAngle										(float radian);													//从弧度转换成角度
	float							angleToRadian										(float angle);
	cocos2d::Point					getRectCenterPoint									(cocos2d::Rect rect);
	

	//math
	void							fixDistancePoint									(cocos2d::Point& point,const float& distance);
	int								atoiSafe											(const char* inputString);
	float							atofSafe											(const char* inputString);
	
	template<typename T>
	inline std::string				toString											(T arg)
	{
		std::stringstream ss;
		ss << arg;
		return ss.str();
	}

	//截图相关的
	void							systemScreenShoot									(std::function<void(bool, const std::string&)> saveFileCallBack,std::string fileName);
	cocos2d::Image*					systemNodeCapture									(cocos2d::Node* node,float scale = 1.0f);
    std::string						saveNodeImage										(cocos2d::Node* node);//只是做下记录，该函数并没有做具体实现功能。
	//window下的时间函数
	unsigned long long int			getCurrentUTime										();//微秒，比毫秒还小1000
	unsigned int					getCurrentMTime										();//毫秒
	//cocos 引擎依赖的时间函数
	int								getTime												();
	long long						getTimeInMilliseconds								();

	cocos2d::Label*					createDefaultLabel									(float fontSize = 30);

	std::string						getStringTimeFromInt64								(unsigned long long time64Data);
	std::string						format												(const char* format, ...) CC_FORMAT_PRINTF(1, 2);
}
#endif//RAYCOMMONFUNCTION_H
