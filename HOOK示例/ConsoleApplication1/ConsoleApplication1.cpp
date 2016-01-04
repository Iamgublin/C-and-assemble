// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#include "Win32Project2.h"
#pragma comment(lib,"Win32Project2.lib")
int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE ev = CreateEvent(NULL, FALSE, FALSE, NULL);
	instailhook(NULL);
	WaitForSingleObject(ev, INFINITE);
}