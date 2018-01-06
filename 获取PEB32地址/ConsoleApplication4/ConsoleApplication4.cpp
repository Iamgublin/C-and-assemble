// ConsoleApplication4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#include<TlHelp32.h>
#include "ntdll_undoc.h"
#pragma comment(lib,"ntdll.lib")

TCHAR buf[MAX_PATH] = { 0 };

int main()
{
	DWORD processId = -1;
	ULONG_PTR Peb32 = NULL;
	DWORD ByteRet = 0;
	printf("进程名:");
	wscanf_s(TEXT("%s"), buf, 20);
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W info;
	info.dwSize = sizeof(PROCESSENTRY32W);
	Process32First(hand, &info);
	if (wcscmp(info.szExeFile, buf) == 0)
	{
		processId = info.th32ProcessID;
	}
	else
	{
		while (Process32Next(hand, &info))
		{
			if (wcscmp(info.szExeFile, buf) == 0)
			{
				processId = info.th32ProcessID;
				break;
			}
		}
	}
	if (processId == -1)
	{
		printf("失败，找不到指定的模块\n");
		return 0;
	}
	wprintf(TEXT("进程名:%s\n"), buf);
	wprintf(TEXT("进程ID:%d\n"), processId);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    //逆向LdrQueryProcessPeb32
	NtQueryInformationProcess(hProcess, ProcessWow64Information, &Peb32, sizeof(ULONG_PTR), &ByteRet);
	printf("PEB32:%p", (void*)Peb32);
	Sleep(INFINITE);
    return 0;
}

