// ConsoleApplication7.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#include<TlHelp32.h>
#include<locale.h>
TCHAR *buf = (TCHAR*)malloc(0);
int _tmain(int argc, _TCHAR* argv[])
{
	SIZE_T a;
	HANDLE took;
	LUID PD; 
	DWORD processId=-1;
	int i = 5;
	setlocale(LC_ALL, "chs");
	printf("请输入想要注入的进程名:");
	wscanf_s(TEXT("%s"), buf, 20);
    HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W info;
	info.dwSize = sizeof(PROCESSENTRY32W);
	Process32First(hand, &info);
	if (wcscmp(info.szExeFile, buf)==0)
	{
		processId = info.th32ProcessID;
	}
	else
	{
		while (Process32Next(hand, &info))
		{
			if (wcscmp(info.szExeFile, buf)==0)
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
	wprintf(TEXT("准备注入"));
	while (i)
	{
		Sleep(0000);
		printf(".");
		i--;
	}
	CHAR *b = "C:\\hookbyzlz.dll";
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &took);
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges->Attributes = SE_PRIVILEGE_ENABLED;
	LookupPrivilegeValue(NULL, TEXT("SeDebugPrivilege"), &PD);
	tp.Privileges->Luid = PD;
	AdjustTokenPrivileges(took, FALSE, &tp, NULL, NULL, NULL);
	HANDLE exploer=OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	PTHREAD_START_ROUTINE pfn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "LoadLibraryA");
	void* add = VirtualAllocEx(exploer, NULL, strlen(b), MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(exploer, add, b, strlen(b),&a);
	HANDLE thread=CreateRemoteThread(exploer,NULL, 0, pfn, add, 0, NULL);
	if (GetLastError() == ERROR_SUCCESS)
	{
		printf("注入成功!\n");
	}
	else
	{
		printf("注入失败,您需要以管理员身份运行程序或尝试关闭360\n");
	}
	return 0;
}

