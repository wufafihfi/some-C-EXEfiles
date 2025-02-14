#pragma once
#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <cstdio>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <tuple>

using namespace std;

namespace FH
{
    #define MCX 1200// ��������� 
    #define MCY 800// �������߶�

	// ����
	//֡����
	const clock_t FPS = 1000 / 120 ;// 1S/120FPS = 8.3(1֡����ʱ�䣩(1000ms/fps = freamTime)

	// ����
	//�ͻ����������
	int MOSX = 0;
	int MOSY = 0;
	// time
	int startTime = 0;// ��ʼʱ��ʱ��
	int freamTime = 0;// �������õ�ʱ��

	// �ļ�·��������ת��
	string Fpath;
	wchar_t W_Fpath[200];

	// ��
	//����ͼ����
	class spritesAnimation
	{
	private:
		int TPID = 0;// ֡ID
		int TPS = 5;// һ������֡
		int imgSize = 70; // һ֡��С
		int AMspeed = 200;// 200MS��һ֡

	public:
		int AMstart = 0;// ������ʼ֡
		int AMplay = 2;// �����ų�����֡

	public:
		// ���ö�������
		void setAM(int tps, int imsize, int amspeed)
		{
			TPS = tps;
			imgSize = imsize;
			AMspeed = amspeed;
		}
		// ֡����ID
		int getTPID()
		{
			return TPID = ((clock() / AMspeed) % AMplay) + AMstart;
		}
		// ֡������ʼ����
		int playTPs()
		{
			return imgSize * getTPID();
		}
	};
	//��������
	class Vector2
	{
	public:
		double X = 0;
		double Y = 0;

		Vector2(double setX, double setY)
		{
			X = setX;
			Y = setY;
		}
	};
	//��������ƶ�
	class  PlanePhysicsMove
	{
	public:
		// ����ٶ�
		double MostSpeedX = 0;// x
		double MostSpeedY = 0;// y 
		// ���ٶ�
		double FSpeedX = 0;// x
		double FSpeedY = 0;// y
		// ����
		double fspeedX = 0;// X����
		double fspeedY = 0;// Y����
		// ��������
		double airfspeedX = 0;// X����
		double airfspeedY = 0;// Y����
		// ����ϵ��
		double ZuNi = 0;
	public:
		// �ٶ�
		double SpeedX = 0;// x
		double SpeedY = 0;// y


	public:
		void SetFSpeed(double X, double Y)
		{
			FSpeedX = X;
			FSpeedY = Y;
		}
		void SetMostSpeed(double X, double Y)
		{
			MostSpeedX = X;
			MostSpeedY = Y;
		}
		void Setfspeed(double X, double Y)
		{
			fspeedX = X;
			fspeedY = Y;
		}
		void Setairfspeed(double X, double Y)
		{
			airfspeedX = X;
			airfspeedY = Y;
		}
		void SetZuNi(double A)
		{
			ZuNi = A;
		}
		void Mostspeed()//����ٶ�����
		{
			if (SpeedX > MostSpeedX)
			{
				SpeedX = MostSpeedX;
			}
			else if (SpeedX < 0 - MostSpeedX)
			{
				SpeedX = -MostSpeedX;
			}
			if (SpeedY > MostSpeedY)
			{
				SpeedY = MostSpeedY;
			}
			else if (SpeedY < 0 - MostSpeedY)
			{
				SpeedY = -MostSpeedY;
			}
		}
		void fX()// ����X
		{
			if (SpeedX > 0) {
				SpeedX -= fspeedX;
				if (SpeedX <= 0)
				{
					SpeedX = 0;
				}
			}
			if (SpeedX < 0) {
				SpeedX += fspeedX;
				if (SpeedX >= 0)
				{
					SpeedX = 0;
				}
			}
		}
		void fY()// ����Y
		{
			if (SpeedY > 0) {
				SpeedY -= fspeedY;
				if (SpeedY <= 0)
				{
					SpeedY = 0;
				}
			}
			if (SpeedY < 0) {
				SpeedY += fspeedY;
				if (SpeedY >= 0)
				{
					SpeedY = 0;
				}
			}
		}
		void airfX()// ����X
		{
			if (SpeedX > 0) {
				SpeedX -= airfspeedX;
				if (SpeedX <= 0)
				{
					SpeedX = 0;
				}
			}
			if (SpeedX < 0) {
				SpeedX += airfspeedX;
				if (SpeedX >= 0)
				{
					SpeedX = 0;
				}
			}
		}
		void airfY()// ����Y
		{
			if (SpeedY > 0) {
				SpeedY -= fspeedY;
				if (SpeedY <= 0)
				{
					SpeedY = 0;
				}
			}
			if (SpeedY < 0) {
				SpeedY += fspeedY;
				if (SpeedY >= 0)
				{
					SpeedY = 0;
				}
			}
		}
		void MoveXLT()
		{
			SpeedX -= FSpeedX;
		}
		void MoveXRT()
		{
			SpeedX += FSpeedX;
		}
		void MoveYDN()
		{
			SpeedY += FSpeedY;
		}
		void MoveYUP()
		{
			SpeedY -= FSpeedY;
		}
	public:
		void physics(bool A)
		{
			Mostspeed();
			if (A)
			{
				fX();
				fY();
			}
			else
			{
				airfX();
				airfY();
			}
			
		}
		
	};
	class VerticalPhysicsMove
	{

	};
	class Object :public Vector2, public PlanePhysicsMove, public VerticalPhysicsMove
	{

	};
	// �����ƶ�����
	class AnimationRectangle
	{
	public:
		double p1[2];
		double p2[2];

	public:
		double Mostsoeed1, Mostsoeed2;
		double Addspeed1, Addspeed2;
		double speed1, speed2;
		double end1, end2;
		double f1, f2;

	public:
		void init(double x1, double y1, double x2, double y2, double Most1, double Most2, double Add1, double Add2, double A_end1, double A_end2, double A_f1, double A_f2)
		{
			p1[0] = x1;
			p1[1] = y1;
			p2[0] = x2;
			p2[1] = y2;

			Mostsoeed1 = Most1;
			Mostsoeed2 = Most2;
			Addspeed1 = Add1;
			Addspeed2 = Add2;
			speed1 = 0;
			speed2 = 0;
			end1 = A_end1;
			end2 = A_end2;
			f1 = A_f1;
			f2 = A_f2;
		}
		void move(bool HW)
		{
			int A = 0;
			int B = 0;
			if (HW == true)
			{
				A = 1;
			}
			else
			{
				A = 0;
			}

			speed1 += Addspeed1;
			speed2 += Addspeed2;

			// ����ٶ�����
			if (fabs(Addspeed1) >= Mostsoeed1)
			{
				if (Addspeed1 > 0)
				{
					Addspeed1 = Mostsoeed1;
				}
				if (Addspeed1 < 0)
				{
					Addspeed1 = -Mostsoeed1;
				}
			}
			if (fabs(Addspeed2) >= Mostsoeed2)
			{
				if (Addspeed2 > 0)
				{
					Addspeed2 = Mostsoeed2;
				}
				if (Addspeed2 < 0)
				{
					Addspeed2 = -Mostsoeed2;
				}
			}

			// ����
			if (speed1 > 0)
			{
				speed1 -= f1;
				if (speed1 <= 0)
				{
					speed1 = 0;
				}
			}
			if (speed1 < 0)
			{
				speed1 += f1;
				if (speed1 >= 0)
				{
					speed1 = 0;
				}
			}
			if (speed2 > 0)
			{
				speed2 -= f2;
				if (speed2 <= 0)
				{
					speed2 = 0;
				}
			}
			if (speed2 < 0)
			{
				speed2 += f2;
				if (speed2 >= 0)
				{
					speed2 = 0;
				}
			}

			// ֹͣ
			if (Addspeed1 > 0)
			{
				if (p1[A] >= end1)
				{
					p1[A] = end1;
					speed1 = 0;
					Addspeed1 = 0;
					B = 1;
				}
			}
			else if (Addspeed1 < 0)
			{
				if (p1[A] <= end1)
				{
					p1[A] = end1;
					speed1 = 0;
					Addspeed1 = 0;
					B = 1;
				}
			}
			if (Addspeed2 > 0)
			{
				if (p2[A] >= end2)
				{
					p2[A] = end2;
					speed2 = 0;
					Addspeed2 = 0;
					B = 1;
				}
			}
			else if (Addspeed2 < 0)
			{
				if (p2[A] <= end2)
				{
					p2[A] = end2;
					speed2 = 0;
					Addspeed2 = 0;
					B = 1;
				}
			}

			if(!(B == 1))
			{
				p1[A] += speed1;
				p2[A] += speed2;
			}
		}
		bool ifeed(bool HW)
		{
			int A = 0;
			if (HW == true)
			{
				A = 1;
			}
			else
			{
				A = 0;
			}

			if (Addspeed1 > 0)
			{
				if (p1[A] >= end1)
				{
					return 1;
				}
			}
			else if (Addspeed1 < 0)
			{
				if (p1[A] <= end1)
				{
					return 1;
				}
			}
			if (Addspeed2 > 0)
			{
				if (p2[A] >= end2)
				{
					return 1;
				}
			}
			else if (Addspeed2 < 0)
			{
				if (p2[A] >= end2)
				{
					return 1;
				}
			}

			return 0;
		}
	};
	class Rect {
	public:
		double x;  // ���Ͻ�x����
		double y;  // ���Ͻ�y����
		double width;  // ���εĿ��
		double height;  // ���εĸ߶�
		double right = x + width;  // ���½�x����
		double bottom = y + height;  // ���½�y����
		double midx = x + (width / 2);// ������������X
		double midy = y + (height / 2);// ������������Y

		void setDate(double SETx, double SETy, double SETwidth, double SETheight)
		{
			x = SETx;
			y = SETy;
			width = SETwidth;
			height = SETheight;

			right = x + width;  // ���½�x����
			bottom = y + height;  // ���½�y����
			midx = x + (width / 2);// ������������X
			midy = y + (height / 2);// ������������Y
		}


		Rect(double SETx = 119, double SETy = 119, double SETwidth = 120, double SETheight = 120) :x(SETx), y(SETy), width(SETwidth), height(SETheight) {
			right = x + width;  // ���½�x����
			bottom = y + height;  // ���½�y����
			midx = x + (width / 2);// ������������X
			midy = y + (height / 2);// ������������Y
		}
	};

	// ����
	// ���(1)
	POINT o;
	// ��Ϣ
	ExMessage ms;

	// ����
	//��ͼ (1) ͼƬ����͸��
	void puting(double x, double y, IMAGE* sim, IMAGE* mak)
	{
		putimage(x, y, mak, SRCAND);
		putimage(x, y, sim, SRCPAINT);
	}
	void puting(double x, double y, double h, double w, IMAGE* sim, IMAGE* mak, double xrcX, double xrcY)
	{
		putimage(x, y, h, w, mak, xrcX, xrcY, SRCAND);
		putimage(x, y, h, w, sim, xrcX, xrcY, SRCPAINT);
	}
	//���������
	void randomSeed()
	{
		srand((unsigned)time(NULL));
	}
	// �ͻ���������꣨1��
	void GetMOSExyInTheWindows(HWND hwnd, int X, int Y)
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);
		GetCursorPos(&o);
		MOSX = o.x - rect.left - 7 + X;
		MOSY = o.y - rect.top + Y;
	}
	// ����
	class FHInput
	{
	private:
		bool keyFlag = false;
		bool keyFlag_Long = false;
		bool keyDown = false;
		bool keyDown_Long = false;

	public:
		// �������£����壩
		bool GetKeyPress(int vKey)
		{
			if (vKey & 0x8000)
			{
				if(!keyFlag && !keyDown)
				{
					keyFlag = true;
					keyDown = true;

					return true;
				}
			}
			else
			{
				keyFlag = false;
				keyDown = false;
			}

			return false;
		}
		// �������£����أ�
		bool GetKeyPressLong(int vKey)
		{
			if (vKey & 0x8000)
			{
				if(!keyDown_Long)
				{
					keyFlag_Long = !keyFlag_Long;
					keyDown_Long = true;
				}
			}
			else
			{
				keyDown_Long = false;
			}

			if (keyFlag_Long)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};


	// ���������ж�
	bool IfInRect(double Mx, double My, double x, double y, double w, double h) {
		if ((Mx >= x) && (My >= y) && (Mx <= x + w) && (My <= y + h))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	// ����ȫ�����ռ���
	BOOL OnForceShow_M(HWND hWnd, double cX, double cY)
	{
		HWND hForeWnd = NULL; 
		DWORD dwForeID = 0;
		DWORD dwCurID = 0;

		hForeWnd = ::GetForegroundWindow();
		dwCurID = ::GetCurrentThreadId();
		dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
		::AttachThreadInput(dwCurID, dwForeID, TRUE);
		::ShowWindow(hWnd, SW_SHOWNORMAL);
		::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetForegroundWindow(hWnd);
		// ��ǰ̨�����߳���������ǰ�̣߳�Ҳ���ǳ���A�еĵ����̣߳�
		::AttachThreadInput(dwCurID, dwForeID, FALSE);

		//SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_EX_LAYERED); // ȥ��������
		::SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~(WS_CAPTION | WS_SYSMENU | WS_SIZEBOX)));

		::SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & (~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) | WS_EX_LAYERED);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, cX, cY, SWP_FRAMECHANGED); // ����λ�úʹ�С

		return TRUE;
	}
	// ����ȫ��
	BOOL OnForceShow(HWND hwnd, double cX, double cY)
	{ 
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_EX_LAYERED); // ȥ��������
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & (~(WS_CAPTION | WS_SYSMENU | WS_SIZEBOX)));

		::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & (~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) | WS_EX_LAYERED);
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, cX, cY, SWP_FRAMECHANGED); // ����λ�úʹ�С

		return TRUE;
	}
	// �ǶȲ���180
	double angleP(double xp, double yp, double x1, double y1, double x2, double y2, bool A)
	{
		double angleRE;
		double angle = atan(fabs(y1 - yp) / fabs(x1 - xp)) * 180 / 3.1415;
		if (A == true)
		{
			int angleIN = angle;
			angleRE = angleIN;
		}
		else
		{
			angleRE = angle;
		}

		if (o.y > yp)
		{
			angleRE = 0 - angleRE;
			if (o.x < xp)
			{
				angleRE = 0 - 180 - angleRE - 1;
			}
		}
		if ((o.x < xp) && (o.y <= yp))
		{
			angleRE = 180 - angleRE;
		}
		return angleRE;
	}
	// �ǶȲ���360
	double angleZ(double xp, double yp, double x1, double y1, double x2, double y2, bool A)
	{
		double angleRE;
		double angle = atan(fabs(y1 - yp) / fabs(x1 - xp)) * 180 / 3.1415;
		if (A == true)
		{
			int angleIN = angle;
			angleRE = angleIN;
		}
		else
		{
			angleRE = angle;
		}

		if (o.y > yp)
		{
			if (o.x >= xp)
			{
				if (angleRE == 0)
				{
					angleRE = 0;
				}
				else
				{
					angleRE = 360 - angleRE;
				}
			}
			if (o.x < xp)
			{
				angleRE = 180 + angleRE;
			}
		}
		if ((o.x < xp) && (o.y <= yp))
		{ 
			angleRE = 180 - angleRE;
		}
		return angleRE;
	}
}
// ��ȡ������·��
string GetExePath()
{
	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	/*
	strrchr:�������ܣ�����һ���ַ�c����һ���ַ���str��ĩ�γ��ֵ�λ�ã�Ҳ���Ǵ�str���Ҳ࿪ʼ�����ַ�c�״γ��ֵ�λ�ã���
	���������λ�õĵ�ַ�����δ���ҵ�ָ���ַ�����ô����������NULL��
	ʹ�������ַ���ش����һ���ַ�c��strĩβ���ַ�����
	*/
	(strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�//
	string path = szFilePath;
	return path;
}
LPCWSTR string2LPCWSTR(std::string str)
{
	size_t size = str.length();
	int wLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t* buffer = new wchar_t[wLen + 1];
	memset(buffer, 0, (wLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, (LPWSTR)buffer, wLen);
	return buffer;
}