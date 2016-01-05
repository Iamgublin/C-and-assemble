// 计算器系统.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "计算器系统.h"
#include <iostream>
#include "nibolan.h"
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND HWnd;
static double cxrect,cyrect;
static int k,b,a,c,h,w,q;
BOOL YICI=0,ERCI=0,SIN=0;
static int xPos,yPos,xzb,yzb;
static int TYPE=0;
TCHAR *TEMP=(TCHAR*)malloc(100);
TCHAR *JIEGUO=(TCHAR*)malloc(0);
// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK dlg1(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK YICIDRAW(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ERCIDRAW(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK SINDRAW(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK JISUANQI(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam);
void DrawPosition(HDC hdc,int cxrect,int cyrect);
void DrawPolyLine1(HDC hdc,int k,int b);
void DrawPolyLine2(HDC hdc,int a,int b,int c);
void DrawPolyLine3(HDC hdc,int a,int w,int q,int h);
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HANDLE m_hMutex = CreateMutex(NULL,TRUE,TEXT("cplusplus_me"));
    DWORD dwRet = GetLastError();
    if (m_hMutex)
    {
        if (ERROR_ALREADY_EXISTS == dwRet)
        {
            MessageBox(NULL,TEXT("程序已经在运行中了,程序退出!"),NULL,MB_OK);
            CloseHandle(m_hMutex);
            return 0;
        }
    }
    else
    {
        MessageBox(NULL,TEXT("创建互斥量错误,程序退出!"),NULL,MB_OK);
        CloseHandle(m_hMutex);
        return 0;
    }
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;
	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!HWnd)
   {
      return FALSE;
   }
   DialogBox(hInst,MAKEINTRESOURCE(IDD_INIT),NULL,dlg1);
   ShowWindow(HWnd, nCmdShow);
   UpdateWindow(HWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR *Posi=(TCHAR*)malloc(10);

	TCHAR *BUF=(TCHAR*)malloc(10);
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_SIZE:
		cxrect=LOWORD(lParam);
		cyrect=HIWORD(lParam);
		UpdateWindow(hWnd);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case ID_JISUANQI:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_JISUANQI),hWnd,JISUANQI);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_YICI:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_YICI),hWnd,YICIDRAW);
			break;
		case IDM_ERCI:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_ERCI),hWnd,ERCIDRAW);
			break;
		case IDM_SIN:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_SIN),hWnd,SINDRAW);
			break;
			
		case ID_CLEAR:
			{
				YICI=0;
				ERCI=0;
				TYPE=0;
				InvalidateRect(HWnd,NULL,TRUE);
			}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_MOUSEMOVE:
		{
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);
			break;
		}
	case WM_LBUTTONDOWN:
		{
			if(TYPE!=0)
			{
			xzb=(xPos/(cxrect/2000))/10-100;
			if(TYPE==1)
			{
				yzb=k*xzb+b;
				wsprintf(Posi,TEXT("x=%d,y=%d"),xzb,yzb);
			}
			if(TYPE==2)
			{
				wsprintf(Posi,TEXT("x=%d,y=%d"),xzb,yzb);
				yzb=a*xzb*xzb+b*xzb+c;
			}
			if(TYPE==3)
			{
				wsprintf(Posi,TEXT("x=%d,y=%d"),xzb,yzb);
				yzb=a*sin(w*(double)xzb+q)+h;
			}
			MessageBox(NULL,Posi,TEXT("坐标详细信息"),MB_OK);
			}
		}
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			DrawPosition(hdc,cxrect,cyrect);
		if(YICI==1)
		{
			DrawPolyLine1(hdc,k,b);
			YICI=0;
		}
		if(ERCI==1)
		{
			DrawPolyLine2(hdc,a,b,c);
			ERCI=0;	
		}
		if(SIN==1)
		{
			DrawPolyLine3(hdc,a,w,q,h);
			SIN=0;	
		}
		EndPaint(hWnd, &ps);
		break;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
BOOL CALLBACK dlg1(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		SetTimer(dlg,1,1000,NULL);
		return TRUE;
	case WM_TIMER:
		static int sum=1;
		switch(sum)
		{
	case 1:
		{
			SetDlgItemText(dlg,IDC_JIAZAI,TEXT("加载基础功能..."));
			sum++;
			break;
		}
	case 2:
		{
			SetDlgItemText(dlg,IDC_JIAZAI,TEXT("加载图形绘制..."));
			sum++;
			break;
		}
	case 3:
		{
			SetDlgItemText(dlg,IDC_JIAZAI,TEXT("加载系统中..."));
			sum++;
			break;
		}
	case 4:
		{
		    EndDialog(dlg,0);
		    KillTimer(dlg,1);
		    break;
		}
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CALLBACK YICIDRAW(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	HWND PNT;
	HDC hdc;
	RECT *p;
	LPWSTR TEMP=(LPWSTR)malloc(10);
	std::wstring temp;
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
	case IDOK:
			GetDlgItemText(dlg,IDC_EDIT1,TEMP,100);
			temp=TEMP;
			k=_wtoi(temp.c_str());
			GetDlgItemText(dlg,IDC_EDIT2,TEMP,100);
			temp=TEMP;
			b=_wtoi(temp.c_str());
			YICI=1;
			InvalidateRect(HWnd,NULL,TRUE);
			EndDialog(dlg,0);
			return TRUE;
	case IDCANCEL:
			EndDialog(dlg,0);
			return TRUE;
		}
		break;
    }
	return FALSE;
}
BOOL CALLBACK JISUANQI(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	char* string=(char*)malloc(100);
	double jieguo;
	switch(message)
	{
	case WM_CLOSE:
		TEMP=(TCHAR*)malloc(100);
        JIEGUO=(TCHAR*)malloc(0);
		EndDialog(dlg,0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON1:
			TEMP=TEXT("1");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON2:
			TEMP=TEXT("2");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON3:
			TEMP=TEXT("3");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON4:
			TEMP=TEXT("4");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON5:
			TEMP=TEXT("5");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON6:
			TEMP=TEXT("6");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON7:
			TEMP=TEXT("7");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON8:
			TEMP=TEXT("8");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON9:
			TEMP=TEXT("9");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON10:
			TEMP=TEXT("0");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON11:
			TEMP=TEXT("+");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON12:
			TEMP=TEXT("-");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON13:
			TEMP=TEXT("*");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON14:
			TEMP=TEXT("/");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON15:
			TEMP=TEXT(".");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON16:
			TEMP=TEXT("(");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON17:
			TEMP=TEXT(")");
			wcscat(JIEGUO,TEMP);
			SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		case IDC_BUTTON18:
            JIEGUO=(TCHAR*)malloc(0);
			SetDlgItemText(dlg,IDC_EDIT1,TEXT(""));
			break;
	    case IDOK:
			    EndDialog(dlg,0);
				break;
		case IDC_JISUAN: 
				GetDlgItemTextA(dlg,IDC_EDIT1,string,100);
				jieguo=jisuan(string);
				swprintf(JIEGUO,L"%lf",jieguo);
				SetDlgItemText(dlg,IDC_EDIT1,JIEGUO);
			break;
		}
	}
	return FALSE;
}
void DrawPosition(HDC hdc,int cxrect,int cyrect)
{
	int se=0;
	TCHAR* BUF=(TCHAR*)malloc(4);
	SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(192,192,192)));
			for(int sum=0;sum<105;sum++)
			{
				MoveToEx(hdc,cxrect/100*sum,0,NULL);
				LineTo(hdc,cxrect/100*sum,cyrect);
			}
			for(int sum=0;sum<105;sum++)
			{
				MoveToEx(hdc,0,cyrect/100*sum,NULL);
				LineTo(hdc,cxrect,cyrect/100*sum);
			}
				SelectObject(hdc,CreatePen(PS_SOLID,4,RGB(0,0,0)));
				MoveToEx(hdc,0,cyrect/2,NULL);
				LineTo(hdc,cxrect,cyrect/2);
				MoveToEx(hdc,cxrect,cyrect/2,NULL);
				LineTo(hdc,cxrect/100*99.8,cyrect/100*55);
				MoveToEx(hdc,cxrect,cyrect/2,NULL);
				LineTo(hdc,cxrect/100*99.8,cyrect/100*51);
				MoveToEx(hdc,cxrect/2,cyrect,NULL);
				LineTo(hdc,cxrect/2,0);
				LineTo(hdc,cxrect/100*53,cyrect/100*7);
				MoveToEx(hdc,cxrect/2,0,NULL);
				LineTo(hdc,cxrect/100*50,cyrect/100*7);
			SetBkMode(hdc, TRANSPARENT);                      //设置TEXTOUT输出透明背景
            for(int sum=0;sum<11;sum++)
			{
				    wsprintf(BUF,TEXT("%d"),-(se*10));
					MoveToEx(hdc,cxrect/2-cxrect/20*se,cyrect/2,NULL);
					LineTo(hdc,cxrect/2-cxrect/20*se,(cyrect/2)-3);
					TextOut(hdc,cxrect/2-cxrect/20*se,(cyrect/2)+3,BUF,wcslen(BUF));
					se++;
			}
			se=1;
			  for(int sum=0;sum<9;sum++)
			{
				    wsprintf(BUF,TEXT("%d"),(se*10));
					MoveToEx(hdc,cxrect/2+cxrect/20*se,cyrect/2,NULL);
					LineTo(hdc,cxrect/2+cxrect/20*se,(cyrect/2)-3);
					TextOut(hdc,cxrect/2+cxrect/20*se,(cyrect/2)+3,BUF,wcslen(BUF));
					se++;
			}
			se=1;
			for(int sum=0;sum<9;sum++)
			{
				    wsprintf(BUF,TEXT("%d"),(se*10));
					MoveToEx(hdc,cxrect/2,cyrect/2-cyrect/20*se,NULL);
					LineTo(hdc,cxrect/2+3,cyrect/2-cyrect/20*se);
					TextOut(hdc,cxrect/2-25,cyrect/2-cyrect/20*se,BUF,wcslen(BUF));
					se++;
			}
			se=1;
			for(int sum=0;sum<10;sum++)
			{
				    wsprintf(BUF,TEXT("%d"),-(se*10));
					MoveToEx(hdc,cxrect/2,cyrect/2+cyrect/20*se,NULL);
					LineTo(hdc,cxrect/2+3,cyrect/2+cyrect/20*se);
					TextOut(hdc,cxrect/2-25,cyrect/2+cyrect/20*se,BUF,wcslen(BUF));
					se++;
			}
			
}
void DrawPolyLine1(HDC hdc,int k,int b)
{
	POINT p[2001];
	double x,y;
	for(int sum=0;sum<=2000;sum++)
	{
		x=-100+sum*0.1;
		y=k*x+b;
		p[sum].x=(cxrect/2000)*sum;
		p[sum].y=(cyrect/2)-(cyrect/200)*y;
	}
	TYPE=1;
	Polyline(hdc,p,2001);
}
BOOL CALLBACK ERCIDRAW(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	LPWSTR TEMP=(LPWSTR)malloc(10);
	std::wstring temp;
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
	case IDOK:
			GetDlgItemText(dlg,IDC_EDIT1,TEMP,100);
			temp=TEMP;
			a=_wtoi(temp.c_str());
			GetDlgItemText(dlg,IDC_EDIT2,TEMP,100);
			temp=TEMP;
			b=_wtoi(temp.c_str());
			GetDlgItemText(dlg,IDC_EDIT3,TEMP,100);
			temp=TEMP;
			c=_wtoi(temp.c_str());
			ERCI=1;
			InvalidateRect(HWnd,NULL,TRUE);
			EndDialog(dlg,0);
			return TRUE;
	case IDCANCEL:
			EndDialog(dlg,0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
void DrawPolyLine2(HDC hdc,int a,int b,int c)
{
	POINT p[2001];
	double x,y;
	for(int sum=0;sum<=2000;sum++)
	{
		x=-100+sum*0.1;
		y=a*x*x+b*x+c;
		p[sum].x=(cxrect/2000)*sum;
		p[sum].y=(cyrect/2)-(cyrect/200)*y;
	}
	TYPE=2;
	Polyline(hdc,p,2001);
}
BOOL CALLBACK SINDRAW(HWND dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
		LPWSTR TEMP=(LPWSTR)malloc(10);
	std::wstring temp;
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
	case IDOK:
			GetDlgItemText(dlg,IDC_EDIT1,TEMP,100);
			temp=TEMP;
			a=_wtoi(temp.c_str());
			GetDlgItemText(dlg,IDC_EDIT2,TEMP,100);
			temp=TEMP;
			w=_wtoi(temp.c_str());
			GetDlgItemText(dlg,IDC_EDIT3,TEMP,100);
			temp=TEMP;
			q=_wtoi(temp.c_str());
			GetDlgItemText(dlg,IDC_EDIT4,TEMP,100);
			temp=TEMP;
			h=_wtoi(temp.c_str());
			SIN=1;
			InvalidateRect(HWnd,NULL,TRUE);
			EndDialog(dlg,0);
			return TRUE;
	case IDCANCEL:
			EndDialog(dlg,0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
void DrawPolyLine3(HDC hdc,int a,int w,int q,int h)
{
	POINT p[2001];
	double x,y,k;
	for(int sum=0;sum<=2000;sum++)
	{
		x=-100+sum*0.1;
		y=a*sin(w*x+q)+h;
		p[sum].x=(cxrect/2000)*sum;
		p[sum].y=(cyrect/2)-(cyrect/200)*y;
	}
	TYPE=3;
	Polyline(hdc,p,2001);
}