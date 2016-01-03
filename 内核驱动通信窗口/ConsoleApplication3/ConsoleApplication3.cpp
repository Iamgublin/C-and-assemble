// ConsoleApplication3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#include<locale>
#define SEND_STR (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x911,METHOD_BUFFERED,FILE_WRITE_DATA)
#define REC_STR (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x912,METHOD_BUFFERED,FILE_READ_DATA)
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	TCHAR *dev = TEXT("\\\\.\\asd_zlz");
	DWORD ret_len = 0;
	TCHAR *buf = (TCHAR*)malloc(512);
	HANDLE device=CreateFile(dev, GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	if (device == INVALID_HANDLE_VALUE)
	{
		printf("error!");
		CloseHandle(device);
		Sleep(3000);
		return 0;
	}
	else
	{
		printf("success!");
		printf("%d", device);
	}
	int sel;
	printf("\n1.输入2.输出:");
	scanf_s("%d", &sel,4);
	if (sel == 1)
	{
		TCHAR *msg = (TCHAR*)malloc(512);
		printf("\n输入要发送到内核的字符串:");
		wscanf_s(TEXT("%s"), msg, 510);
		DeviceIoControl(device, SEND_STR, msg, wcslen(msg)*sizeof(TCHAR) + 2, NULL, 0, &ret_len, 0);
	}
	if (sel == 2)
	{
		DeviceIoControl(device, REC_STR, NULL, 0, buf, 512, &ret_len, 0);
		wprintf(TEXT("%s"), buf);
	}
	Sleep(3000);
	return 0;
}

