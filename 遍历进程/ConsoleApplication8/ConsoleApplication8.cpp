// ConsoleApplication8.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#include<TlHelp32.h>
#include<locale.h>
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	HANDLE hand=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W info;
	info.dwSize = sizeof(PROCESSENTRY32W);
	Process32First(hand, &info);
	wprintf(TEXT("进程的名字:%s\n"), info.szExeFile);
	wprintf(TEXT("进程的ID:%u\n"), info.th32ProcessID);
	while (Process32Next(hand, &info))
	{
		wprintf(TEXT("进程的名字:%s\n"), info.szExeFile);
		wprintf(TEXT("进程的ID:%u\n"), info.th32ProcessID);
	}
	return 0;
}

