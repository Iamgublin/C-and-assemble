// ConsoleApplication3 control.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#define SYN_NAME L"\\\\.\\tdi_control_device"
#define IOCTL_CONSEND (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x911,METHOD_BUFFERED,FILE_WRITE_DATA|FILE_READ_DATA)  
#define IOCTL_CONRECV (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x912,METHOD_BUFFERED,FILE_WRITE_DATA|FILE_READ_DATA)

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE device = CreateFile(SYN_NAME, GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	printf("0x%x\n", GetLastError());
	DeviceIoControl(device, IOCTL_CONSEND, 0, 0, NULL, 0, 0, 0);
	printf("0x%x\n", GetLastError());
	Sleep(2000);
	return 0;
}

