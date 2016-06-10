// Win32Project1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Project1.h"
#include"resource.h"
#include<Windows.h>
#include<windowsx.h>
#include<TlHelp32.h>
#include<locale>
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
MEMORYSTATUS ms;
// 此代码模块中包含的函数的前向声明:
void getprocess(HWND HDlg);
void showmodule(HWND HDlg);
void getmodule(DWORD id, HWND HDlg);
void CALLBACK TimerProc(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime);
BOOL CALLBACK DlgProc(HWND HDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	setlocale(LC_ALL, "chs");
	HANDLE took;
	LUID PD;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &took);
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges->Attributes = SE_PRIVILEGE_ENABLED;
	LookupPrivilegeValue(NULL, TEXT("SeDebugPrivilege"), &PD);
	tp.Privileges->Luid = PD;
	AdjustTokenPrivileges(took, FALSE, &tp, NULL, NULL, NULL);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_Main), NULL, DlgProc);
	return 0;
}
BOOL CALLBACK DlgProc(HWND HDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		getprocess(HDlg);
		SetTimer(HDlg, 0, 1000, TimerProc);
		break;
	case WM_CLOSE:
		EndDialog(HDlg, 0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_showmodule:
			showmodule(HDlg);
			break;
		default:
			break;
		}
	default:;
	}
	return FALSE;
}
void CALLBACK TimerProc(HWND HDlg, UINT message, UINT iTimerID, DWORD dwTime)
{
	TCHAR* buf[3] = { NULL, NULL, NULL };
	GlobalMemoryStatus(&ms);
	buf[0] = (TCHAR*)malloc(sizeof(int) + sizeof(TEXT("%")));
	wsprintf(buf[0], TEXT("%d%%"), ms.dwMemoryLoad);
	SetDlgItemText(HDlg, IDC_mem0, buf[0]);
	buf[1] = (TCHAR*)malloc(sizeof(int) + sizeof(TEXT("GB")));
	wsprintf(buf[1], TEXT("%dGB"), ms.dwTotalPhys / 1024 / 1024 / 1024);
	SetDlgItemText(HDlg, IDC_mem1, buf[1]);
	buf[2] = (TCHAR*)malloc(sizeof(int) + sizeof(TEXT("MB")));
	wsprintf(buf[2], TEXT("%dMB"), ms.dwAvailPhys / 1024 / 1024);
	SetDlgItemText(HDlg, IDC_mem2, buf[2]);
}
void getprocess(HWND HDlg)
{
	HANDLE pro;
	HWND combo = GetDlgItem(HDlg, IDC_process);
	PROCESSENTRY32 info;
	info.dwSize = sizeof(PROCESSENTRY32);
	pro=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	Process32First(pro, &info);
	ComboBox_AddString(combo, info.szExeFile);
	while (Process32Next(pro, &info))
	{
		ComboBox_AddString(combo, info.szExeFile);
	}
}
void showmodule(HWND HDlg)
{
	HANDLE pro;
	DWORD id;
	TCHAR* buf = (TCHAR*)malloc(20);
	GetDlgItemText(HDlg, IDC_process, buf, 20);
	PROCESSENTRY32 info;
	info.dwSize = sizeof(PROCESSENTRY32);
	pro = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	Process32First(pro, &info);
	if (wcscmp(buf, info.szExeFile)==0)
	{
		id = info.th32ProcessID;
		getmodule(id, HDlg);
		return;
	}
	else
	{
		while (Process32Next(pro, &info))
		{
			if (wcscmp(buf, info.szExeFile) == 0)
			{
				id = info.th32ProcessID;
				getmodule(id, HDlg);
				return;
			}
		}
	}
}
void getmodule(DWORD id,HWND HDlg)
{
	MODULEENTRY32 info;
	TCHAR* buf = (TCHAR*)malloc(400);
	HWND listbo = GetDlgItem(HDlg, IDC_module);
	ListBox_ResetContent(listbo);
	info.dwSize = sizeof(MODULEENTRY32);
	HANDLE md=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, id);
	DWORD err = GetLastError();
	if (err != ERROR_SUCCESS)
	{
		MessageBox(NULL, TEXT("权限不够"), NULL, MB_OK);
		return;
	}
	Module32First(md, &info);
	{
		wsprintf(buf, TEXT("%ws        0x%x        0x%x  "), info.szExePath, info.modBaseAddr, info.modBaseSize);
		ListBox_AddString(listbo, buf);
	}
	while (Module32Next(md,&info))
	{
		wsprintf(buf, TEXT("%ws        0x%x        0x%x  "), info.szExePath, info.modBaseAddr, info.modBaseSize);
		ListBox_AddString(listbo, buf);
	}
}