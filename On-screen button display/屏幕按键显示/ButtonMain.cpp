#define _USE_MATH_DEFINES 
#include <iostream>
#include <iomanip>
#include <graphics.h>
#include <conio.h>
#include <cstdio>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include "FH.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <vector>
#include <thread>
#include <chrono>
#include <math.h>
#include <atomic>
#include <cmath>
#include <time.h>  
#include <tuple>
#include <fstream>
#include <io.h>
#include <mmsystem.h>

#pragma comment (lib,"winmm.lib")

#define MCX GetSystemMetrics(SM_CXSCREEN)// 绘制区宽度 
#define MCY GetSystemMetrics(SM_CYSCREEN)// 绘制区高度 

#define MAX16_0xFFFF 65535

//#define GetRValue(rgb)      (LOBYTE(rgb))
//#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
//#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#define GetRValue(rgb)  ((BYTE)(rgb))
#define GetGValue(rgb)  ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)  ((BYTE)((rgb) >> 16))

// 文字样式 5:2
int BBc = 8;

using namespace std;
using namespace FH;

// 文件处理
const string PathF = GetExePath();
// 写
ofstream OF_Log;
// 读
ifstream IF_BoxTransform;
ifstream IF_radarDate;

//数据
double Positon_num[200];
char Font_String[200][200];
time_t now = time(0);
tm gmtm;
SYSTEMTIME timeL;



class ButtonBox {
public:
	double x, y;
	double w, h;
	double LineW;
	RECT TextBox;
	RECT OtherTextBox;
	bool BoxState;
	COLORREF BoxFillColor;
	COLORREF BoxLineColor;
	COLORREF BoxTextColor;
	COLORREF InvertedFillColor;
	COLORREF InvertedLineColor;
	COLORREF InvertedTextColor;
	
	//专用按键检测（多功能）
	FHInput ButtonKeyCheck111;

	//CPS
	clock_t time_P = clock();
	clock_t time_END = clock();
	int countPress = 0;
	bool beginCPSJiSuan = false;

	double CPSjiSuan(bool keydown) {

		if (clock() - time_END >= CLOCKS_PER_SEC) {
			beginCPSJiSuan = false;
			time_END = clock();
			countPress = 0;
		}

		if (keydown) {
			if (!beginCPSJiSuan) {
				time_P = clock();
				beginCPSJiSuan = true;
			}
			time_END = clock();
			countPress += 1;
		}

		double elapsed_time = static_cast<double>(clock() - time_P) / CLOCKS_PER_SEC;

		if (elapsed_time == 0 || elapsed_time < 0.5) {
			return 0;
		}

		return countPress / elapsed_time;
	}
	//一般BOX绘制
	void DrawBox(LPCTSTR Tx) {
		if(BoxState == 0){setfillcolor(BoxLineColor);}else{ setfillcolor(InvertedLineColor); }
		solidrectangle(x - LineW, y - LineW, x + w + LineW, y + h + LineW);
		if (BoxState == 0) { setfillcolor(BoxFillColor); }else { setfillcolor(InvertedFillColor); }
		solidrectangle(x, y, x + w, y + h);
		if (BoxState == 0) { settextcolor(BoxTextColor); }else { settextcolor(InvertedTextColor); }
		drawtext(Tx, &TextBox, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	void DrawOther(int cpsn)
	{
		TCHAR text[100];
		_stprintf_s(text, _T("%d"), cpsn);
		if (BoxState == 0) { settextcolor(BoxTextColor); }else { settextcolor(InvertedTextColor); }
		drawtext(text, &OtherTextBox, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	void ButtonStateChange(int BSCdown) {
		if(BSCdown & 0x8000)
		{
			BoxState = 1;
		}
		else
		{
			BoxState = 0;
		}
	}
	bool MouseClickTheBox(double Mx,double My,int MCTBdown) {
		if ((Mx >= x) && (My >= y) && (Mx <= x + w) && (My <= y + h) && MCTBdown)
		{
			ButtonStateChange(MCTBdown & 0x8000);
			return 1;
		}
		else
		{
			ButtonStateChange(MCTBdown & 0x8000);
			return 0;
		}
	}

	void SETTextBox() {

		TextBox = { (long)x ,(long)y ,(long)(x + w) ,(long)(y + h) };
		OtherTextBox = { (long)x ,(long)y + (long)h / 3 * 2,(long)(x + w) ,(long)(y + h) };
	}
	void SETColor(COLORREF SETBoxFillColor, COLORREF SETBoxLineColor, COLORREF SETBoxTextColor){
		BoxFillColor = SETBoxFillColor;
		BoxLineColor = SETBoxLineColor;
		BoxTextColor = SETBoxTextColor;
	}
	void SETInvertedColor(COLORREF SETInvertedFillColor, COLORREF SETInvertedLineColor, COLORREF SETInvertedTextColor){
		InvertedFillColor = SETInvertedFillColor;
		InvertedLineColor = SETInvertedLineColor;
		InvertedTextColor = SETInvertedTextColor;
	}
	void SETPosition(double SETx, double SETy)
	{
		x = SETx;
		y = SETy;
		SETTextBox();
	}

	void SetButtonBox(double SETx, double SETy, double SETw, double SETh, double SETLineW, bool SETBoxState){
		x = SETx;
		y = SETy;
		w = SETw;
		h = SETh;
		LineW = SETLineW;
		BoxState = SETBoxState;
		SETTextBox();
	}

	ButtonBox(double SETx, double SETy, double SETw, double SETh,double SETLineW, bool SETBoxState) :
		x(SETx),
		y(SETy),
		w(SETw),
		h(SETh),
		LineW(SETLineW),
		BoxState(SETBoxState)
	{
		SETTextBox();
	}
};

class Slider
{
public:
	ButtonBox SliderBox;
	Rect R_Slide;// 滑轨
	Rect R_Slider;// 滑块
	bool SliderIsCircleOrRectangle = true;// true圆 false矩形
	double SliderCircle_r = 10;
	COLORREF SliderFillColor;// 滑块
	COLORREF SliderLineColor;
	COLORREF SlideFillColor;// 滑轨
	COLORREF SlideLineColor;

	//调参功能X
	double Start_End_DistanceX = 100;
	double DataDistanceX = 100;
	double SDDDX = DataDistanceX / Start_End_DistanceX;
	double ActualSliderDX = 50;
	double SlideDistanceX;
	double SDSDX = Start_End_DistanceX / SlideDistanceX;
	double ActualDataX = ActualSliderDX * SDSDX * SDDDX;
	//调参功能Y
	double Start_End_DistanceY = 0;
	double DataDistanceY = 0;
	double SDDDY = DataDistanceY / Start_End_DistanceY;
	double ActualSliderDY = 0;
	double SlideDistanceY;
	double SDSDY = Start_End_DistanceY / SlideDistanceY;
	double ActualDataY = ActualSliderDY * SDSDY * SDDDY;

	//按键逻辑
	bool bottonIsTrue = false;


	void SETColor(COLORREF SETSliderFillColor, COLORREF SETSliderLineColor, COLORREF SETSlideFillColor, COLORREF SETSlideLineColor) {
		SliderFillColor = SETSliderFillColor;
		SliderLineColor = SETSliderLineColor;

		SlideFillColor = SETSlideFillColor;
		SlideLineColor = SETSlideLineColor;
	}
		
	void SETBoxPositon(double x,double y,double w,double h,double LineW, bool SETBoxState)
	{
		SliderBox.SetButtonBox(x, y, w, h, LineW, SETBoxState);
	}

	void SETSliderAndSlideInBox(double CenterX,double CenterY,double w,double h, bool isR,double startX, double startY,double Rw, double Rh)
	{
		SliderIsCircleOrRectangle = isR;
		R_Slider.setDate(SliderBox.x + (CenterX - w / 2), SliderBox.y + (CenterY - h / 2), w, h);
		R_Slide.setDate(SliderBox.x + startX, SliderBox.y + startY, Rw, Rh);
		SliderCircle_r = w / 2;

		SlideDistanceX = Rw;
		SlideDistanceY = Rh;

		SDSDX = Start_End_DistanceX / SlideDistanceX;
		SDSDY = Start_End_DistanceY / SlideDistanceY;
	}
	
	void DrawSliderBox(bool showBox = true)
	{
		SliderBox.DrawBox(_T(""));
		setfillcolor(SlideFillColor);
		setlinecolor(SlideLineColor);
		fillrectangle(R_Slide.x, R_Slide.y, R_Slide.right, R_Slide.bottom);
		setfillcolor(SliderFillColor);
		setlinecolor(SliderLineColor);
		if(SliderIsCircleOrRectangle == true)
		{
			fillcircle(R_Slider.midx + ActualSliderDX, R_Slider.midy + ActualSliderDY, SliderCircle_r);
		}
		else
		{
			fillrectangle(R_Slider.x + ActualSliderDX, R_Slider.y + ActualSliderDY, R_Slider.right + ActualSliderDX, R_Slider.bottom + ActualSliderDY);
		}
	}

	void Updata(int HaveY = 0, bool otherBottonState = false,bool debug = false)
	{
		if(otherBottonState == false)
		{
			if ((HaveY == 0) || (HaveY == 2))
			{
				if ((GetAsyncKeyState(VK_LBUTTON) && (IfInRect(o.x, o.y, R_Slider.x + ActualSliderDX, R_Slider.y + ActualSliderDY, R_Slider.width, R_Slider.height) || IfInRect(o.x, o.y, R_Slide.x, R_Slide.y, R_Slide.width, R_Slide.height))) || bottonIsTrue == true)
				{
					bottonIsTrue = true;
					ActualSliderDX = (o.x - R_Slide.x);
				}
				if (!GetAsyncKeyState(VK_LBUTTON))
				{
					bottonIsTrue = false;
				}
				if (ActualSliderDX > SlideDistanceX)
				{
					ActualSliderDX = SlideDistanceX;
				}
				if (ActualSliderDX < 0)
				{
					ActualSliderDX = 0;
				}
			}
			if ((HaveY == 1) || (HaveY == 2))
			{
				if ((GetAsyncKeyState(VK_LBUTTON) && (IfInRect(o.x, o.y, R_Slider.x + ActualSliderDX, R_Slider.y + ActualSliderDY, R_Slider.width + ActualSliderDX, R_Slider.height + ActualSliderDY) || IfInRect(o.x, o.y, R_Slide.x, R_Slide.y, R_Slide.width, R_Slide.height))) || bottonIsTrue == true)
				{
					bottonIsTrue = true;
					ActualSliderDY = (o.y - R_Slide.bottom);
				}
				if (!GetAsyncKeyState(VK_LBUTTON))
				{
					bottonIsTrue = false;
				}
				if (ActualSliderDY > SlideDistanceY)
				{
					ActualSliderDY = SlideDistanceY;
				}
				if (ActualSliderDY < 0)
				{
					ActualSliderDY = 0;
				}
			}
		}
		if(debug == true)
		{
			setlinecolor(RGB(0, 255, 0));
			rectangle(R_Slider.x + ActualSliderDX, R_Slider.y + ActualSliderDY, R_Slider.x + R_Slider.width + ActualSliderDX, R_Slider.y + R_Slider.height + ActualSliderDY);
			rectangle(R_Slide.x, R_Slide.y, R_Slide.x + R_Slide.width, R_Slide.y + R_Slide.height);
		}

		ActualDataX = ActualSliderDX * SDSDX * SDDDX;
		ActualDataY = ActualSliderDY * SDSDX * SDDDY;
	}

	Vector2 putOutData()
	{
		Vector2 DataXY(ActualDataX, ActualDataY);
		return DataXY;
	}

	Slider(double Start_End_DistanceX, double Start_End_DistanceY, double DataDistanceX, double DataDistanceY, double ActualSliderDX, bool ActualSliderDY) :
		Start_End_DistanceX(Start_End_DistanceX),
		Start_End_DistanceY(Start_End_DistanceY),
		DataDistanceX(DataDistanceX),
		DataDistanceY(DataDistanceY),
		ActualSliderDX(ActualSliderDX),
		ActualSliderDY(ActualSliderDY),
		SliderBox(100, 100,100,100,100,1)
	{
		SDDDX = DataDistanceX / Start_End_DistanceX;
		SDDDY = DataDistanceY / Start_End_DistanceY;

		SDSDX = Start_End_DistanceX / SlideDistanceX;
		SDSDY = Start_End_DistanceY / SlideDistanceY;

		ActualDataX = ActualSliderDX * SDSDX * SDDDX;
		ActualDataY = ActualSliderDY * SDSDY * SDDDY;
	}
};

// 文件处理
//删除指定字符串
string removeSubstring(const std::string& input, const std::string& substring) 
{
	string result = input;
	size_t pos = 0;
	while ((pos = result.find(substring, pos)) != string::npos) {
		result.erase(pos, substring.length());
	}
	return result;
}
//打开
bool openF(ifstream* F_Open, string filePath,bool clearF)//if
{
	if (clearF)
	{
		F_Open->open(PathF + filePath);
	}
	else
	{
		F_Open->open(PathF + filePath, ios::app);
	}

	if (!F_Open->is_open()) {
		cerr << "无法打开文件 根目录:" << PathF << std::endl;
		return 0;
	}

	return 1;
}
bool openF(ofstream* F_Open, string filePath, bool clearF)//of
{
	if (clearF)
	{
		F_Open->open(PathF + filePath);
	}
	else
	{
		F_Open->open(PathF + filePath, ios::app);
	}

	if (!F_Open->is_open()) {
		cerr << "无法打开文件 根目录:" << PathF << std::endl;
		return 0;
	}

	return 1;
}
//使用
void ReadF_Box_cfg(ifstream* F_Open)
{
	SYSTEMTIME time1;
	string F_get;
	int Vi = 0;
	int Si = 0;
	double Value;
	string Str;
	now = time(0);// UTC时间跟新
	GetLocalTime(&timeL);
	gmtime_s(&gmtm ,&now);


	while (getline(*F_Open, F_get)){
		F_get = removeSubstring(F_get,"AllBoxX");
		F_get = removeSubstring(F_get, "AllBoxY");
		F_get = removeSubstring(F_get, "KeyboardBoxX");
		F_get = removeSubstring(F_get, "KeyboardBoxY");
		F_get = removeSubstring(F_get, "MouseBoxX");
		F_get = removeSubstring(F_get, "MouseBoxY");
		F_get = removeSubstring(F_get, "OtherBoxX");
		F_get = removeSubstring(F_get, "OtherBoxY");
		F_get = removeSubstring(F_get, "font");
		F_get.erase(remove(F_get.begin(), F_get.end(), '='), F_get.end());

		try {
			if (F_get.find('"') != string::npos)
			{
				F_get.erase(remove(F_get.begin(), F_get.end(), '"'), F_get.end());
				Str = F_get;
				Str.copy(Font_String[Si], Str.size());
				OF_Log << gmtm.tm_year + 1900 << "-" << timeL.wMonth << "-" << timeL.wDay << " " << timeL.wHour << ":" << timeL.wMinute << ":" << timeL.wSecond << "|" << clock() << "ms|[Info]" << F_get << "--[UseF\\while\\string]-->" << Str << "--[Font_String[" << Si << "]=Str]-->" << Font_String[Si] << endl;
				Si += 1;
			}
			else
			{
				F_get.erase(remove(F_get.begin(), F_get.end(), ' '), F_get.end());
				Value = stod(F_get);
				Positon_num[Vi] = Value;
				OF_Log << gmtm.tm_year + 1900 << "-" << timeL.wMonth << "-" << timeL.wDay << " " << timeL.wHour << ":" << timeL.wMinute << ":" << timeL.wSecond << "|" << clock() << "ms|[Info]" << F_get << "--[UseF\\while\\stod]-->" << Value << "--[Positon_num[" << Vi << "]=Value]-->" << Positon_num[Vi] << endl;
				Vi += 1;
			}
		}
		catch (const invalid_argument& e) {
			OF_Log << gmtm.tm_year + 1900 << "-" << timeL.wMonth << "-" << timeL.wDay << " " << timeL.wHour << ":" << timeL.wMinute << ":" << timeL.wSecond << "|" << clock() << "ms|[Error]" << F_get << "[UseF\\while\\stod]zh-cn:无效的转换 可能是空行 en-us:An invalid conversion may be a blank line" << endl;
		}
		catch (const out_of_range& e) {
			OF_Log << gmtm.tm_year + 1900 << "-" << timeL.wMonth << "-" << timeL.wDay << " " << timeL.wHour << ":" << timeL.wMinute << ":" << timeL.wSecond << "|" << clock() << "ms|[Error]" << F_get << "[UseF\\while\\stod]zh-cn:超出范围 en-us:data out of range" << endl;
		}
	}



}

bool radarDateisOpen = false;
double XYZDate[6]; // 用于存储读取的值
string textDate; // 用于存储读取的值

// 文件读取线程
void fileReader() {
	// 打开瓦基雷达数据文件
	string Date;
	bool textMode = false;
	while (true) {
		IF_radarDate.open("SB");
		radarDateisOpen = IF_radarDate.is_open();
		if (radarDateisOpen)
		{
			IF_radarDate.seekg(0);
			int i = 0;
			textMode = false;
			while (getline(IF_radarDate, Date)) { 
				if (Date == "text")
				{
					textMode = true;
				}
				if (textMode)
				{
					textDate = Date;
				}
				else
				{
					if (Date == "void")
					{
						XYZDate[i] = NULL;
					}
					else
					{
						XYZDate[i] = stod(Date);
					}
				}
				i++;
			}
			IF_radarDate.close();
		}
		// 小延迟，用于避免高频率读取带来的性能影响
		this_thread::sleep_for(chrono::milliseconds(50)); // 非阻塞，防止过于频繁读取
	}
}

bool MusicPlaying = false;
void MplaySound(string MusicPath)
{
	MusicPlaying = true;
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, MusicPath.c_str(), -1, W_Fpath, 200);
	PlaySound(W_Fpath, NULL, SND_FILENAME);
	MusicPlaying = false;
}


void PitchAndYaw(double x, double y, double z, double& pitch, double& yaw){
	// 弧度转角度
	pitch = atan2(y, sqrt(x * x + z * z)) * (180.0 / M_PI); //俯仰
	yaw = atan2(x, z) * (180.0 / M_PI); //偏航
}

class Vector3 {
public:
	double x, y, z;

	Vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
};

class Quaternion {
public:
	float w, x, y, z;

	Quaternion(double w = 1, double x = 0, double y = 0, double z = 0): w(w), x(x), y(y), z(z) {}

	// 计算共轭四元数
	Quaternion conjugate() const {
		return Quaternion(w, -x, -y, -z);
	}
};

Quaternion vectorToQuaternion(const Vector3& v) {
	return Quaternion(0, v.x, v.y, v.z);
}

Vector3 RotateVectorByQuat(const Quaternion& quat, const Vector3& v) {
	// 计算临时变量
	double x2 = quat.x * 2.0f;
	double y2 = quat.y * 2.0f;
	double z2 = quat.z * 2.0f;

	double xx = quat.x * x2;
	double yy = quat.y * y2;
	double zz = quat.z * z2;
	double xy = quat.x * y2;
	double xz = quat.x * z2;
	double yz = quat.y * z2;
	double wx = quat.w * x2;
	double wy = quat.w * y2;
	double wz = quat.w * z2;

	// 计算旋转后的向量
	Vector3 res;
	res.x = (1.0f - (yy + zz)) * v.x + (xy - wz) * v.y + (xz + wy) * v.z;
	res.y = (xy + wz) * v.x + (1.0f - (xx + zz)) * v.y + (yz - wx) * v.z;
	res.z = (xz - wy) * v.x + (yz + wx) * v.y + (1.0f - (xx + yy)) * v.z;

	return res;
}

// 欧拉转四元数
Quaternion EulerToQuaternion(double roll, double pitch, double yaw) {
	// 转换为弧度
	float c1 = cos(roll / 2.0f);
	float c2 = cos(pitch / 2.0f);
	float c3 = cos(yaw / 2.0f);
	float s1 = sin(roll / 2.0f);
	float s2 = sin(pitch / 2.0f);
	float s3 = sin(yaw / 2.0f);

	Quaternion q;
	q.w = c1 * c2 * c3 + s1 * s2 * s3;
	q.x = s1 * c2 * c3 - c1 * s2 * s3;
	q.y = c1 * s2 * c3 + s1 * c2 * s3;
	q.z = c1 * c2 * s3 - s1 * s2 * c3;

	return q;
}


// 四元数旋转函数
Vector3 rotatePoint(const Quaternion& q, const Vector3& point) {
	Quaternion qConjugate = q.conjugate();
	Quaternion p = vectorToQuaternion(point);

	// q * p * q^-1
	Quaternion result;
	// 计算 q * p
	result.w = q.w * p.w - q.x * p.x - q.y * p.y - q.z * p.z;
	result.x = q.w * p.x + q.x * p.w + q.y * p.z - q.z * p.y;
	result.y = q.w * p.y - q.x * p.z + q.y * p.w + q.z * p.x;
	result.z = q.w * p.z + q.x * p.y - q.y * p.x + q.z * p.w;

	// 计算 result * q^-1（即 result * q 的共轭）
	Vector3 rotatedPoint;
	rotatedPoint.x = result.w * qConjugate.x + result.x * qConjugate.w + result.y * qConjugate.z - result.z * qConjugate.y;
	rotatedPoint.y = result.w * qConjugate.y - result.x * qConjugate.z + result.y * qConjugate.w + result.z * qConjugate.x;
	rotatedPoint.z = result.w * qConjugate.z + result.x * qConjugate.y - result.y * qConjugate.x + result.z * qConjugate.w;

	return rotatedPoint;
}

void getFiles(string path, vector<string>& files, const char* sType)
{
	//文件句柄
	//long hFile = 0;
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files, sType);
				}
			}
			else
			{
				char* pName = fileinfo.name;
				char* pFind = strstr(pName, sType);
				if (pFind != NULL)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

vector<string> GetFiles(string folderPath,string targetFilesSuffix)//指定文件夹搜索指定后缀文件
{
	vector<string> temp;
	getFiles(folderPath, temp, targetFilesSuffix.c_str());

	return temp;
}

string DecIntToHexStr(long long num)
{
	string str;
	long long Temp = num / 16;
	int left = num % 16;
	if (Temp > 0)
		str += DecIntToHexStr(Temp);
	if (left < 10)
		str += (left + '0');
	else
		str += ('A' + left - 10);
	return str;
}


int main() {
	// 窗口创建
	initgraph(MCX, MCY,0);
	HWND hwnd = GetHWnd();

	// 打开配置文件
	bool LogisOpen = openF(&OF_Log, "\\log\\log.log", 0);
	// 打开配置文件
	bool BoxTransformisOpen = openF(&IF_BoxTransform, "\\Box\\BoxSetting.cfg", 1);
	// 搜索音乐文件
	vector<string> MusicFiles;
	MusicFiles = GetFiles(PathF + "\\Music", ".wav");

	// 打开瓦基雷达数据文件
	// 启动文件读取线程
	//thread readerThread(fileReader);
	//readerThread.detach(); // 分离线程
	/*
	string Date;
	double XYZDate[6];
	IF_radarDate.open("C:\\Users\\15280\\AppData\\Roaming\\.minecraft\\versions\\1.20.1-Forge_47.3.11\\saves\\新的世界\\computercraft\\computer\\0\\radarDate.Rdate");
	bool radarDateisOpen = IF_radarDate.is_open();
	if(radarDateisOpen)
	{
		int i = 0;
		while (getline(IF_radarDate, Date)) {
			XYZDate[i] = stod(Date);
			i++;
		}
		IF_radarDate.close();
	}
	*/


	//日志
	OF_Log << "========================================================" << endl;
	//配置文件
	ReadF_Box_cfg(&IF_BoxTransform);

	// 窗口置顶
	OnForceShow_M(hwnd, MCX, MCY);
	// 透明
	::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & (~(WS_CAPTION | WS_SYSMENU | WS_SIZEBOX)));

	//::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & (~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(hwnd, 0x000000, 200, LWA_ALPHA | LWA_COLORKEY);

	setfillcolor(RGB(0, 255, 0));

	// 量&类定义
	//鼠标坐标暂存
	double OX = 0;
	double	OY = 0;
	int Otime = 0;
	double lineSensitivity = 0.01;
	//键盘显示速度控制
	int XBtime = 0;
	//全局
	double AllBoxX = 0;
	double AllBoxY = 0;
	//键盘
	double KeyboardBoxX = 100;
	double KeyboardBoxY = 100;
	//鼠标
	double MouseBoxX = 450;
	double MouseBoxY = 100;
	//OTHER
	double OtherBoxX = 100;
	double OtherBoxY = 330;
	//字体
	char fontChar[200] = {"微软雅黑"};
	//设置
	double SettingBoxX = MCX - AllBoxX - 100;
	double SettingBoxY = 100;
	double auxiliaryX = MCX / 2 - AllBoxX;
	double auxiliaryY = MCY / 2 - AllBoxY;
	//瓦基数据
	double distanceX = 0;
	double distanceY = 0;
	double distanceZ = 0;
	double pitch = 0;
	double yaw = 0; 
	Quaternion AgQ(0, 0, 0, 0);
	Vector3 VgQ(0, 0, 0);
	Vector3 VpQ(0, 0, 0);
	double rectSize = 0;
	double radarSerchDistance = 1200;
	//音频
	double volumeDmax = 100;
	double volumeD = 31;
	string hexStr = DecIntToHexStr(volumeD * (MAX16_0xFFFF / volumeDmax));
	DWORD volume = static_cast<DWORD>(stoul(hexStr, nullptr, 16)); // 设定音量为一半 (0x7FFF 为最大值的一半)
	waveOutSetVolume(NULL, volume | (volume << 16));
	int musicId = 0;// 音量切换
	string MusicPath = MusicFiles[musicId];// 路径改变
	//时间显示
	bool ShowTimeFlag = false;
	Rect TimeNumberBox(AllBoxX + (MCX - 500), AllBoxY + 50, 100, 100);
	if (BoxTransformisOpen)
	{
		//全局
		AllBoxX = Positon_num[0];
		AllBoxY = Positon_num[1];
		//键盘
		KeyboardBoxX = Positon_num[2];
		KeyboardBoxY = Positon_num[3];
		//鼠标
		MouseBoxX = Positon_num[4];
		MouseBoxY = Positon_num[5];
		//OTHER
		OtherBoxX = Positon_num[6];
		OtherBoxY = Positon_num[7];
		//字体
		strncpy_s(fontChar, Font_String[0], sizeof(fontChar) - 1); // 复制内容
		fontChar[sizeof(fontChar) - 1] = '\0';
	}
	//转换char -> LPCTSTR
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, fontChar, -1, NULL, 0);
	wchar_t* wideStr = new wchar_t[size_needed]; // 动态分配宽字符数组
	// 转换 char 字符串为 wchar_t 字符串
	MultiByteToWideChar(CP_UTF8, 0, fontChar, -1, wideStr, size_needed);
	// 文字样式 5:2
	BBc = 8;
	settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
	setbkmode(TRANSPARENT);
	//颜色（文件读取）
	COLORREF color_fill_setAll = RGB(30, 30, 30);
	COLORREF color_line_setAll = RGB(50, 50, 50); 
	COLORREF color_text_setAll = RGB(220, 220, 220);
	COLORREF invertedColor_fill_setAll = RGB(225, 225, 225);
	COLORREF invertedColor_line_setAll = RGB(205, 205, 205);
	COLORREF invertedColor_text_setAll = RGB(35, 35, 35);
	//W
	ButtonBox B1(AllBoxX + KeyboardBoxX + 110, AllBoxY + KeyboardBoxY, 100, 100, 2, 0);
	B1.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B1.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B1.SETTextBox();
	//D
	ButtonBox B2(AllBoxX + KeyboardBoxX + 220, AllBoxY + KeyboardBoxY + 110, 100, 100, 2, 0);
	B2.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B2.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B2.SETTextBox();
	//A
	ButtonBox B3(AllBoxX + KeyboardBoxX, AllBoxY + KeyboardBoxY + 110, 100, 100, 2, 0);
	B3.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B3.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B3.SETTextBox();
	//S
	ButtonBox B4(AllBoxX + KeyboardBoxX + 110, AllBoxY + KeyboardBoxY + 110, 100, 100, 2, 0);
	B4.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B4.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B4.SETTextBox();


	//ML
	ButtonBox B5(AllBoxX + MouseBoxX, AllBoxY + MouseBoxY, 60, 100, 2, 0);
	B5.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B5.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B5.SETTextBox();
	//MR
	ButtonBox B6(AllBoxX + MouseBoxX + 70, AllBoxY + MouseBoxY, 60, 100, 2, 0);
	B6.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B6.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B6.SETTextBox();


	//SHIFT
	ButtonBox B7(AllBoxX + OtherBoxX, AllBoxY + OtherBoxY + 110, 180, 100, 2, 0);
	B7.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B7.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B7.SETTextBox();
	//SPACE
	ButtonBox B8(AllBoxX + OtherBoxX, AllBoxY + OtherBoxY, 330, 100, 2, 0);
	B8.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	B8.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	B8.SETTextBox();


	//Setting
	FHInput SettingB1_Input;
	ButtonBox SettingB1(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY, 100, 50, 2, 0);
	SettingB1.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SettingB1.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SettingB1.SETTextBox();
	//NULL
	ButtonBox SettingNULL(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 52, 100, 50, 2, 0);
	SettingNULL.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SettingNULL.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SettingNULL.SETTextBox();
	//Hipshot
	ButtonBox SettingHipshotAuxiliary(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 104, 100, 50, 2, 0);
	SettingHipshotAuxiliary.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SettingHipshotAuxiliary.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SettingHipshotAuxiliary.SETTextBox();
	//SniperRifl
	ButtonBox SettingSniperRiflesAuxiliary(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 156, 100, 50, 2, 0);
	SettingSniperRiflesAuxiliary.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SettingSniperRiflesAuxiliary.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SettingSniperRiflesAuxiliary.SETTextBox();
	//ColorGradient
	ButtonBox SettingColorGradient(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 208, 100, 50, 2, 0);
	SettingColorGradient.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SettingColorGradient.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SettingColorGradient.SETTextBox();
	//Music
	ButtonBox SettingMusic(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 260, 100, 50, 2, 0);
	SettingMusic.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SettingMusic.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SettingMusic.SETTextBox();
	//Music
	ButtonBox SettingShowTime(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 312, 100, 50, 2, 0);
	SettingShowTime.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SettingShowTime.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SettingShowTime.SETTextBox();


	//按键检测
	FHInput B5_IN1;
	FHInput B6_IN1;

	//滑块控件
	//音量控制
	Slider SmusicVolumeControl(100, 0, MAX16_0xFFFF, 1, 31, 0);
	SmusicVolumeControl.SETBoxPositon(SettingMusic.x - 120 - 5, SettingMusic.y, 120, 50, 2, 0);
	SmusicVolumeControl.SliderBox.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	SmusicVolumeControl.SliderBox.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	SmusicVolumeControl.SETSliderAndSlideInBox(15, SmusicVolumeControl.SliderBox.h / 2, SmusicVolumeControl.SliderBox.h / 2 - 4, SmusicVolumeControl.SliderBox.h / 2 - 4, true, 15, SmusicVolumeControl.SliderBox.h / 2 - 2, SmusicVolumeControl.SliderBox.w - 30, 4);
	SmusicVolumeControl.SETColor(invertedColor_fill_setAll, color_line_setAll, color_line_setAll, color_line_setAll);
	//音乐切换
	Slider MusicIdChange(100, 0, MusicFiles.size() - 1, 1, 31, 0);
	MusicIdChange.SETBoxPositon(SettingMusic.x - 390 - 5, SettingMusic.y, 200, 50, 2, 0);
	MusicIdChange.SliderBox.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	MusicIdChange.SliderBox.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	MusicIdChange.SETSliderAndSlideInBox(15, MusicIdChange.SliderBox.h / 2, MusicIdChange.SliderBox.h / 2 - 4, MusicIdChange.SliderBox.h / 2 - 4, true, 15, MusicIdChange.SliderBox.h / 2 - 2, MusicIdChange.SliderBox.w - 30, 4);
	MusicIdChange.SETColor(invertedColor_fill_setAll, color_line_setAll, color_line_setAll, color_line_setAll);
	//音量显示
	ButtonBox ShowMusicVolume(SettingMusic.x - 170 - 5, SettingMusic.y, 50, 50, 2, 0);
	ShowMusicVolume.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	ShowMusicVolume.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	ShowMusicVolume.SETTextBox();
	//音乐ID显示
	ButtonBox ShowMusicId(SettingMusic.x - 440 - 5, SettingMusic.y, 50, 50, 2, 0);
	ShowMusicId.SETColor(color_fill_setAll, color_line_setAll, color_text_setAll);
	ShowMusicId.SETInvertedColor(invertedColor_fill_setAll, invertedColor_line_setAll, invertedColor_text_setAll);
	ShowMusicId.SETTextBox();

	double RePointx = AllBoxX;
	double RePointy = AllBoxY;
	double Xd = 10;
	double Yd = 10;

	// begin
	BeginBatchDraw();
	while (1) {
		GetCursorPos(&o);// 获取标坐标 
		startTime = clock();// 记录时间start
		now = time(0);// UTC时间更新
		GetLocalTime(&timeL);
		gmtime_s(&gmtm, &now);
		//########################################//
		// 临时屏幕标记
		/*
		setfillcolor(RGB(0, 255, 0));
		fillrectangle(0, 0, CX, CY);
		clearrectangle(0 + 10, 0 + 10, CX - 10, CY - 10);
		*/

		if (clock() - Otime >= 50) {
			Otime = clock();
			OX = o.x;
			OY = o.y;
		}

		//setlinecolor(RGB(0, 255, 0));
		//line(CX / 2, CY / 2, CX / 2 - (o.x - OX) * lineSensitivity, CY / 2 - (o.y - OY) * lineSensitivity);
		//circle(CX / 2 - (o.x - OX) * lineSensitivity, CY / 2 - (o.y - OY) * lineSensitivity, 18);

		AllBoxX += (o.x - OX) * lineSensitivity;
		AllBoxY += (o.y - OY) * lineSensitivity;

		AllBoxX -= (AllBoxX - RePointx) / Xd;
		AllBoxY -= (AllBoxY - RePointy) / Yd;

		//按键显示
		B1.SETPosition(AllBoxX + KeyboardBoxX + 110, AllBoxY + KeyboardBoxY);
		B2.SETPosition(AllBoxX + KeyboardBoxX + 220, AllBoxY + KeyboardBoxY + 110);
		B3.SETPosition(AllBoxX + KeyboardBoxX, AllBoxY + KeyboardBoxY + 110);
		B4.SETPosition(AllBoxX + KeyboardBoxX + 110, AllBoxY + KeyboardBoxY + 110);
		B5.SETPosition(AllBoxX + MouseBoxX, AllBoxY + MouseBoxY);
		B6.SETPosition(AllBoxX + MouseBoxX + 70, AllBoxY + MouseBoxY);
		B7.SETPosition(AllBoxX + OtherBoxX, AllBoxY + OtherBoxY + 110);
		B8.SETPosition(AllBoxX + OtherBoxX, AllBoxY + OtherBoxY);
		//设置
		SettingB1.SETPosition(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY);
		SettingNULL.SETPosition(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 52);
		SettingHipshotAuxiliary.SETPosition(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 104);
		SettingSniperRiflesAuxiliary.SETPosition(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 156);
		SettingColorGradient.SETPosition(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 208);
		SettingMusic.SETPosition(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 260);
		SettingShowTime.SETPosition(AllBoxX + SettingBoxX - 100, AllBoxY + SettingBoxY + 312);
		//滑块
		SmusicVolumeControl.SETBoxPositon(SettingMusic.x - 120 - 5, SettingMusic.y, 120, 50, 2, 0);
		SmusicVolumeControl.SETSliderAndSlideInBox(15, SmusicVolumeControl.SliderBox.h / 2, SmusicVolumeControl.SliderBox.h / 2 - 4, SmusicVolumeControl.SliderBox.h / 2 - 4, true, 15, SmusicVolumeControl.SliderBox.h / 2 - 2, SmusicVolumeControl.SliderBox.w - 30, 4);
		MusicIdChange.SETBoxPositon(SettingMusic.x - 370 - 5, SettingMusic.y, 200, 50, 2, 0);
		MusicIdChange.SETSliderAndSlideInBox(15, MusicIdChange.SliderBox.h / 2, MusicIdChange.SliderBox.h / 2 - 4, MusicIdChange.SliderBox.h / 2 - 4, true, 15, MusicIdChange.SliderBox.h / 2 - 2, MusicIdChange.SliderBox.w - 30, 4);
		//数据显示方块
		ShowMusicVolume.SETPosition(SettingMusic.x - 170 - 5, SettingMusic.y);
		ShowMusicId.SETPosition(SettingMusic.x - 420 - 5, SettingMusic.y);
		//非Botton类
		TimeNumberBox.setDate(AllBoxX + (MCX - 500), AllBoxY + 50, 300, 100);

		// 文字样式 5:2
		BBc = 8;
		settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
		// 颜色渐变
		static int bR1, bG1, bB1;
		static int bR2, bG2, bB2;
		static int bR3, bG3, bB3;
		if (clock() - XBtime >= 100) {
			XBtime = clock();
			/////
			bR1 += 3;
			bG1 -= 1;
			bB1 += 5;
			/////
			bR2 += 3;
			bG2 += 2;
			bB2 += 4;
			/////
			bR3 += 3;
			bG3 -= 2;
			bB3 -= 4;

			if (bR1 > 255) { bR1 = 0; }
			if (bG1 > 255) { bG1 = 0; }
			if (bB1 > 255) { bB1 = 0; }
			/////
			if (bR2 > 255) { bR2 = 0; }
			if (bG2 > 255) { bG2 = 0; }
			if (bB2 > 255) { bB2 = 0; }
			/////
			if (bR3 > 255) { bR3 = 0; }
			if (bG3 > 255) { bG3 = 0; }
			if (bB3 > 255) { bB3 = 0; }
		}

		//键盘
		B1.DrawBox(_T("W"));
		B1.ButtonStateChange(GetAsyncKeyState(0x57));

		B2.DrawBox(_T("D"));
		B2.ButtonStateChange(GetAsyncKeyState(0x44));

		B3.DrawBox(_T("A"));
		B3.ButtonStateChange(GetAsyncKeyState(0x41));

		B4.DrawBox(_T("S"));
		B4.ButtonStateChange(GetAsyncKeyState(0x53));

		//SHIFT
		B7.DrawBox(_T("SHIFT"));
		B7.ButtonStateChange(GetAsyncKeyState(0xA0));
		//SPACE
		B8.DrawBox(_T("SPACE"));
		B8.ButtonStateChange(GetAsyncKeyState(VK_SPACE));

		//鼠标
		BBc = 8;
		settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
		B5.DrawBox(_T("ML"));
		BBc = 6;
		settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
		B5.DrawOther(B5.CPSjiSuan(B5_IN1.GetKeyPress(GetAsyncKeyState(VK_LBUTTON))));
		B5.ButtonStateChange(GetAsyncKeyState(VK_LBUTTON));

		BBc = 8;
		settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
		B6.DrawBox(_T("MR"));
		BBc = 6;
		settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
		B6.DrawOther(B6.CPSjiSuan(B6_IN1.GetKeyPress(GetAsyncKeyState(VK_RBUTTON))));
		B6.ButtonStateChange(GetAsyncKeyState(VK_RBUTTON));
		// 文字样式 5:2
		BBc = 8;
		settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);

		////////////////////////////MC瓦基
		if (radarDateisOpen)
		{
			// 目标位置
			if ((XYZDate[0] == NULL) || (XYZDate[1] == NULL) || (XYZDate[2] == NULL))
			{
				settextcolor(RGB(0, 255, 0));
				RECT ctr1 = { 500,500,600,700 };
				drawtext(_T("-.-"), &ctr1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				RECT ctr2 = { 600,500,700,700 };
				drawtext(_T("-.-"), &ctr2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				RECT ctr3 = { 700,500,800,700 };
				drawtext(_T("-.-"), &ctr3, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				settextcolor(RGB(0, 255, 0));
				TCHAR text[100];
				_stprintf_s(text, _T("%0.1f"), XYZDate[0]);
				RECT ctr1 = { 500,500,600,700 };
				drawtext(text, &ctr1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_stprintf_s(text, _T("%0.1f"), XYZDate[1]);
				RECT ctr2 = { 600,500,700,700 };
				drawtext(text, &ctr2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_stprintf_s(text, _T("%0.1f"), XYZDate[2]);
				RECT ctr3 = { 700,500,800,700 };
				drawtext(text, &ctr3, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			// 玩家位置
			if ((XYZDate[3] == NULL) || (XYZDate[4] == NULL) || (XYZDate[5] == NULL))
			{
				settextcolor(RGB(0, 255, 0));
				RECT ctr1 = { 500,600,600,800 };
				drawtext(_T("-.-"), &ctr1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				RECT ctr2 = { 600,600,700,800 };
				drawtext(_T("-.-"), &ctr2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				RECT ctr3 = { 700,600,800,800 };
				drawtext(_T("-.-"), &ctr3, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				settextcolor(RGB(0, 255, 0));
				TCHAR text[100];
				_stprintf_s(text, _T("%0.1f"), XYZDate[3]);
				RECT ctr1 = { 500,600,600,800 };
				drawtext(text, &ctr1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_stprintf_s(text, _T("%0.1f"), XYZDate[4]);
				RECT ctr2 = { 600,600,700,800 };
				drawtext(text, &ctr2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_stprintf_s(text, _T("%0.1f"), XYZDate[5]);
				RECT ctr3 = { 700,600,800,800 };
				drawtext(text, &ctr3, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			if (!((XYZDate[0] == NULL) || (XYZDate[1] == NULL) || (XYZDate[2] == NULL)))
			{
				distanceX = XYZDate[0] - XYZDate[3];
				distanceY = XYZDate[1] - XYZDate[4];
				distanceZ = XYZDate[2] - XYZDate[5];

				PitchAndYaw(distanceX, distanceY, distanceZ, pitch, yaw);
				VgQ.x = distanceX;
				VgQ.y = distanceY;
				VgQ.z = distanceZ;

				//Vector3 Cd1(-55 - XYZDate[3], -56 - XYZDate[4], -48 - XYZDate[5]);
				//PitchAndYaw(Cd1.x, Cd1.y, Cd1.z, pitch, yaw);
				//VpQ = RotateVectorByQuat(AgQ, Cd1);

				double distanceALL = sqrt(VgQ.x * VgQ.x + VgQ.y * VgQ.y + VgQ.z * VgQ.z);

				if (distanceALL < 200 - 20)
				{
					rectSize = 200 - sqrt(VgQ.x * VgQ.x + VgQ.y * VgQ.y + VgQ.z * VgQ.z);
				}
				else
				{
					rectSize = 20;
				}

				setlinecolor(RGB(0, 255, 0));
				rectangle(MCX / 2 - rectSize, MCY / 2 - rectSize, MCX / 2 + rectSize, MCY / 2 + rectSize);
				//rectangle(CX / 2 + VpQ.x - 100, CY / 2 + VpQ.y - 100, CX / 2 + VpQ.x + 100, CY / 2 + VpQ.y + 100);

				TCHAR text[100];
				_stprintf_s(text, _T("%0.1f"), pitch);
				RECT ctr1 = { 500,700,600,900 };
				drawtext(text, &ctr1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_stprintf_s(text, _T("%0.1f"), yaw);
				RECT ctr2 = { 600,700,700,900 };
				drawtext(text, &ctr2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				RECT ctr1 = { 500,700,600,900 };
				drawtext(_T("-.-"), &ctr1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				RECT ctr2 = { 600,700,700,900 };
				drawtext(_T("-.-"), &ctr2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			TCHAR text[100];
			_stprintf_s(text, _T("%S"), textDate.c_str());
			RECT ctr2 = { 700,100,1800,200 };
			drawtext(text, &ctr2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		////////////////////////////MC瓦基


		//Setting
		static int Bend = 0;
		static int onS = 0;
		static int onColor = 0;
		static int playMusic = 0;
		static int lineNumber = 40;
		switch (onS)
		{
		case 0:
			onColor = 0;
			B1.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B1.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			B2.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B2.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			B3.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B3.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			B4.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B4.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			B5.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B5.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			B6.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B6.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			B7.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B7.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			B8.SETColor(RGB(30, 30, 30), RGB(50, 50, 50), RGB(220, 220, 220));
			B8.SETInvertedColor(RGB(225, 225, 225), RGB(205, 205, 205), RGB(35, 35, 35));
			break;

		case 1:
			setlinecolor(RGB(0, 255, 0));
			settextcolor(RGB(0, 255, 0));
			if (GetAsyncKeyState(VK_RBUTTON))
			{
				// 文字样式 5:2
				BBc = 6;
				settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
				//1
				line(AllBoxX + auxiliaryX - 450, AllBoxY + (long)auxiliaryY, AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY);

				static RECT TL1 = { AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY - 20 , AllBoxX + auxiliaryX + 500, AllBoxY + (long)auxiliaryY + 20 };
				TL1 = { (long)AllBoxX + (long)auxiliaryX + 450, (long)AllBoxY + (long)auxiliaryY - 20 , (long)AllBoxX + (long)auxiliaryX + 500, (long)AllBoxY + (long)auxiliaryY + 20 };
				drawtext(_T("50"), &TL1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				//2
				line(AllBoxX + auxiliaryX - 450, AllBoxY + (long)auxiliaryY + 160, AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY + 160);

				static RECT TL2 = { AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY + 160 - 20 , AllBoxX + auxiliaryX + 500, AllBoxY + (long)auxiliaryY + 160 + 20 };
				TL2 = { (long)AllBoxX + (long)auxiliaryX + 450, (long)AllBoxY + (long)auxiliaryY + 160 - 20 , (long)AllBoxX + (long)auxiliaryX + 500, (long)AllBoxY + (long)auxiliaryY + 160 + 20 };

				drawtext(_T("100"), &TL2, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				//3
				line(AllBoxX + auxiliaryX - 450, AllBoxY + (long)auxiliaryY + 360, AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY + 360);

				static RECT TL3 = { AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY + 360 - 20 , AllBoxX + auxiliaryX + 500, AllBoxY + (long)auxiliaryY + 360 + 20 };
				TL3 = { (long)AllBoxX + (long)auxiliaryX + 450, (long)AllBoxY + (long)auxiliaryY + 360 - 20 , (long)AllBoxX + (long)auxiliaryX + 500, (long)AllBoxY + (long)auxiliaryY + 360 + 20 };

				drawtext(_T("150"), &TL3, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				//4
				line(AllBoxX + auxiliaryX - 450, AllBoxY + (long)auxiliaryY + 563, AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY + 563);

				static RECT TL4 = { AllBoxX + auxiliaryX + 450, AllBoxY + (long)auxiliaryY + 563 - 20 , AllBoxX + auxiliaryX + 500, AllBoxY + (long)auxiliaryY + 563 + 20 };
				TL4 = { (long)AllBoxX + (long)auxiliaryX + 450, (long)AllBoxY + (long)auxiliaryY + 563 - 20 , (long)AllBoxX + (long)auxiliaryX + 500, (long)AllBoxY + (long)auxiliaryY + 563 + 20 };

				drawtext(_T("200"), &TL4, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);


				//5
				line(AllBoxX + auxiliaryX, AllBoxY + auxiliaryY - 242, AllBoxX + auxiliaryX, AllBoxY + auxiliaryY - 142);
				line(AllBoxX + auxiliaryX - 50, AllBoxY + auxiliaryY - 192, AllBoxX + auxiliaryX + 50, AllBoxY + auxiliaryY - 192);
			}
			break;

		case 2:
			setlinecolor(RGB(0, 255, 0));
			settextcolor(RGB(0, 255, 0));
			if (!GetAsyncKeyState(VK_RBUTTON))
			{
				circle(AllBoxX + auxiliaryX, AllBoxY + auxiliaryY, 30);
				circle(AllBoxX + auxiliaryX, AllBoxY + auxiliaryY, 70);
			}
			break;

		case 3:
			setlinecolor(RGB(0, 255, 0));
			settextcolor(RGB(0, 255, 0));
			if (GetAsyncKeyState(VK_RBUTTON))
			{
				line(AllBoxX + auxiliaryX, AllBoxY + auxiliaryY, AllBoxX + auxiliaryX, AllBoxY + (auxiliaryY * 2));
				for (int i = 0; i <= 50; i++)
				{
					line(AllBoxX + auxiliaryX - 22, AllBoxY + auxiliaryY + auxiliaryY / lineNumber * i, AllBoxX + auxiliaryX + 22, AllBoxY + auxiliaryY + auxiliaryY / lineNumber * i);
				}
			}
			break;

		case 4:
			onColor = 1;
			break;
		}
		//特殊功能实现(绕过 “无” 按钮的控制)
		//颜色渐变
		if (onColor == 1)
		{
			B1.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B2.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B3.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B4.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B5.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B6.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B7.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B8.SETColor(RGB(bR1, bG1, bB1), RGB(bR2 + 20, bG2 + 20, bB2 + 20), RGB(bR3 + 190, bG3 + 190, bB3 + 190));
			B1.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
			B2.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
			B3.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
			B4.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
			B5.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
			B6.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
			B7.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
			B8.SETInvertedColor(RGB(255 - bR1, 255 - bG1, 255 - bB1), RGB(255 - (bR2 + 20), 255 - (bG2 + 20), 255 - (bB2 + 20)), RGB(255 - (bR3 + 190), 255 - (bG3 + 190), 255 - (bB3 + 190)));
		}
		static bool MusicPlaysSuccessfully = false;
		//背景音乐
		if (playMusic == 1)
		{
			SettingMusic.BoxState = 1;
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, MusicPath.c_str(), -1, W_Fpath, 200);
			if (MusicPlaysSuccessfully == false)
			{
				MusicPlaysSuccessfully = PlaySound(W_Fpath, NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
				OF_Log << clock() << "ms|[info]FileOpen(Music)-->Path: [" << MusicPath << "]-->[result]successfully" << endl;
			}
		}
		else
		{
			PlaySound(NULL, NULL, 0);
			MusicPlaysSuccessfully = false;
			SettingMusic.BoxState = 0;
		}
		//时间显示
		if (ShowTimeFlag == true)
		{
			// 文字样式 5:2
			BBc = 8;
			settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 100, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);
			settextcolor(RGB(0, 255, 0));

			TCHAR text[100];
			_stprintf_s(text, _T("%d : %d : %d"), timeL.wHour, timeL.wMinute, timeL.wSecond);
			RECT ctr1 = { TimeNumberBox.x ,TimeNumberBox.y,TimeNumberBox.x + TimeNumberBox.width,TimeNumberBox.y + TimeNumberBox.height};
			drawtext(text, &ctr1, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		//################鼠标之上#################//
		// 鼠标防误触
		clearcircle(o.x, o.y, 20);

		// 文字样式 5:2
		BBc = 5;
		settextstyle(5 * BBc, 2 * BBc, wideStr, 0, 0, 700, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0);

		SettingB1.DrawBox(_T("其他功能"));
		if (SmusicVolumeControl.bottonIsTrue == false && MusicIdChange.bottonIsTrue == false)
		{
			if (IfInRect(o.x, o.y, SettingB1.x, SettingB1.y, SettingB1.w, SettingB1.h))
			{
				if (SettingB1_Input.GetKeyPressLong(GetAsyncKeyState(VK_LBUTTON)))
				{
					SettingB1.BoxState = 1;
					Bend = 1;
				}
				else
				{
					Bend = 0;
				}
			}
		}

		//其他功能内显示
		//特殊功能
		if (Bend == 1)
		{
			if (playMusic == 1)
			{
				SmusicVolumeControl.SliderBox.DrawBox(_T(""));
				SmusicVolumeControl.DrawSliderBox(true);
				SmusicVolumeControl.Updata(0, MusicIdChange.bottonIsTrue, 0);
				Vector2 Cdata_a = SmusicVolumeControl.putOutData();

				hexStr = DecIntToHexStr(Cdata_a.X);
				volume = static_cast<DWORD>(stoul(hexStr, nullptr, 16)); // 设定音量为一半 (0x7FFF 为最大值的一半)
				waveOutSetVolume(NULL, volume | (volume << 16));

				static Vector2 Cid_a(0, 0);
				int MusicIdSave = Cid_a.X;
				MusicIdChange.SliderBox.DrawBox(_T(""));
				MusicIdChange.DrawSliderBox(true);
				MusicIdChange.Updata(0, SmusicVolumeControl.bottonIsTrue, 0);
				Cid_a = MusicIdChange.putOutData();
				MusicPath = MusicFiles[(int)Cid_a.X];
				cout << MusicPath << endl;
				if (MusicIdSave != (int)Cid_a.X)
				{
					MusicPlaysSuccessfully = false;
				}

				TCHAR text[100];
				_stprintf_s(text, _T("%d"), (int)Cid_a.X);
				ShowMusicId.DrawBox(text);
				_stprintf_s(text, _T("%d"), (int)(SmusicVolumeControl.ActualSliderDX * SmusicVolumeControl.SDSDX));
				ShowMusicVolume.DrawBox(text);

				/* 滑轨控件的祖师爷
				*   ~  ~  ~
				*   |  |  |
				*   |  |  |
				* 【       】
				*   【   】
				* 给他烧上三根香
				musicVolumeControl.DrawBox(_T(""));
				setfillcolor(color_line_setAll);
				setlinecolor(color_line_setAll);
				fillrectangle(musicVolumeControl.x + 15, musicVolumeControl.y + musicVolumeControl.h / 2 - 2, musicVolumeControl.x + musicVolumeControl.w - 15, musicVolumeControl.y + musicVolumeControl.h / 2 + 2);
				setfillcolor(invertedColor_fill_setAll);
				double VDC = fabs(musicVolumeControl.x + 15 - (musicVolumeControl.x + musicVolumeControl.w - 15)) / volumeDmax;
				double moveDistance = volumeD * VDC;
				Vector2 circleFirsrPos(musicVolumeControl.x + 15, musicVolumeControl.y + musicVolumeControl.h / 2);
				if (GetAsyncKeyState(VK_LBUTTON) && IfInRect(o.x, o.y, musicVolumeControl.x, circleFirsrPos.Y - (musicVolumeControl.h / 2 - 10), musicVolumeControl.w - 15, 2 * (musicVolumeControl.h / 2 - 10)))
				{
					volumeD = (o.x - circleFirsrPos.X) / VDC;
				}
				if (volumeD > 100)
				{
					volumeD = 100;
				}
				if (volumeD < 0)
				{
					volumeD = 0;
				}
				fillcircle(circleFirsrPos.X + moveDistance, circleFirsrPos.Y, musicVolumeControl.h / 2 - 15);
				hexStr = DecIntToHexStr(volumeD * (MAX16_0xFFFF / volumeDmax));
				volume = static_cast<DWORD>(stoul(hexStr, nullptr, 16)); // 设定音量为一半 (0x7FFF 为最大值的一半)
				waveOutSetVolume(NULL, volume | (volume << 16));
				*/
			}
		}
		//设置按钮的显示和点击判定
		if (Bend == 1)
		{
			SettingB1.BoxState = 1;
			SettingNULL.DrawBox(_T("无"));
			SettingHipshotAuxiliary.DrawBox(_T("腰射辅助"));
			SettingSniperRiflesAuxiliary.DrawBox(_T("倍镜辅助"));
			SettingColorGradient.DrawBox(_T("颜色渐变"));
			SettingMusic.DrawBox(_T("BGM!!!"));
			SettingShowTime.DrawBox(_T("显示时间"));

			if (SmusicVolumeControl.bottonIsTrue == false && MusicIdChange.bottonIsTrue == false)
			{
				//无
				if (GetAsyncKeyState(VK_LBUTTON) && IfInRect(o.x, o.y, SettingNULL.x, SettingNULL.y, SettingNULL.w, SettingNULL.h))
				{
					SettingNULL.BoxState = 1;
					onS = 0;
				}
				else
				{
					SettingNULL.BoxState = 0;
				}
				//腰射辅助
				if (GetAsyncKeyState(VK_LBUTTON) && IfInRect(o.x, o.y, SettingHipshotAuxiliary.x, SettingHipshotAuxiliary.y, SettingHipshotAuxiliary.w, SettingHipshotAuxiliary.h))
				{
					SettingHipshotAuxiliary.BoxState = 1;
					onS = 2;
				}
				else
				{
					SettingHipshotAuxiliary.BoxState = 0;
				}
				//倍镜辅助
				if (GetAsyncKeyState(VK_LBUTTON) && IfInRect(o.x, o.y, SettingSniperRiflesAuxiliary.x, SettingSniperRiflesAuxiliary.y, SettingSniperRiflesAuxiliary.w, SettingSniperRiflesAuxiliary.h))
				{
					SettingSniperRiflesAuxiliary.BoxState = 1;
					onS = 3;
				}
				else
				{
					SettingSniperRiflesAuxiliary.BoxState = 0;
				}
				//颜色渐变
				if (GetAsyncKeyState(VK_LBUTTON) && IfInRect(o.x, o.y, SettingColorGradient.x, SettingColorGradient.y, SettingColorGradient.w, SettingColorGradient.h))
				{
					SettingColorGradient.BoxState = 1;
					onS = 4;
				}
				else
				{
					SettingColorGradient.BoxState = 0;
				}
				//背景音乐
				if(IfInRect(o.x, o.y, SettingMusic.x, SettingMusic.y, SettingMusic.w, SettingMusic.h))
				{
					if (SettingMusic.ButtonKeyCheck111.GetKeyPressLong(GetAsyncKeyState(VK_LBUTTON)))
					{
						playMusic = 1;
						SettingMusic.BoxState = 1;
					}
					else
					{
						playMusic = 0;
						SettingMusic.BoxState = 0;
					}
				}
				//时间显示
				if (IfInRect(o.x, o.y, SettingShowTime.x, SettingShowTime.y, SettingShowTime.w, SettingShowTime.h))
				{
					if (SettingShowTime.ButtonKeyCheck111.GetKeyPressLong(GetAsyncKeyState(VK_LBUTTON)))
					{
						ShowTimeFlag = true;
						SettingShowTime.BoxState = 1;
					}
					else
					{
						ShowTimeFlag = false;
						SettingShowTime.BoxState = 0;
					}
				}
			}
			bool ShowCheckBox = 0;
			if(ShowCheckBox)
			{
				setlinecolor(RGB(0, 255, 0));
				rectangle(SettingNULL.x, SettingNULL.y, SettingNULL.x + SettingNULL.w, SettingNULL.y + SettingNULL.h);
				rectangle(SettingHipshotAuxiliary.x, SettingHipshotAuxiliary.y, SettingHipshotAuxiliary.x + SettingHipshotAuxiliary.w, SettingHipshotAuxiliary.y + SettingHipshotAuxiliary.h);
				rectangle(SettingSniperRiflesAuxiliary.x, SettingSniperRiflesAuxiliary.y, SettingSniperRiflesAuxiliary.x + SettingSniperRiflesAuxiliary.w, SettingSniperRiflesAuxiliary.y + SettingSniperRiflesAuxiliary.h);
				rectangle(SettingColorGradient.x, SettingColorGradient.y, SettingColorGradient.x + SettingColorGradient.w, SettingColorGradient.y + SettingColorGradient.h);
				rectangle(SettingMusic.x, SettingMusic.y, SettingMusic.x + SettingMusic.w, SettingMusic.y + SettingMusic.h);
				rectangle(SettingShowTime.x, SettingShowTime.y, SettingShowTime.x + SettingShowTime.w, SettingShowTime.y + SettingShowTime.h);
			}

		}
		else
		{
			SettingB1.BoxState = 0;
		}

		//################鼠标之上#################//
		cout << fixed << setprecision(5);
		cout << "_T等线:" << 1241 << "   SmusicVolumeControl.bottonIsTrue :" << SmusicVolumeControl.bottonIsTrue << "  MusicIdChange.bottonIsTrue:" << MusicIdChange.bottonIsTrue << "   CX:" << MCX << "   CY:" << MCY << endl;
		//########################################//
		// 刷新
		FlushBatchDraw();
		// 清空
		cleardevice();

		freamTime = clock() - startTime;// 计算时间
		if (freamTime < FPS)
		{
			Sleep(FPS - freamTime);
		}// 让程序睡觉
	}


}