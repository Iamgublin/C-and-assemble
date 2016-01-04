// ConsoleApplication3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR *b = (TCHAR*)malloc(10);
	TCHAR *buf = (TCHAR*)malloc(0);
	STARTUPINFO info;
	ZeroMemory(&info, sizeof(info));
	PROCESS_INFORMATION proinfo = { 0 };
	printf("please input the dll you want see:");
	scanf_s("%s",b,10);
	wsprintf(buf, TEXT("cmd.exe /k DUMPBIN -exports c:\\windows\\system32\\%S.dll"), b);
	CreateProcess(NULL, buf, NULL, NULL, FALSE, NULL, NULL, NULL, &info, &proinfo);
	WaitForSingleObject(proinfo.hProcess, INFINITE);
	printf("%s", buf);
	return 0;
}

