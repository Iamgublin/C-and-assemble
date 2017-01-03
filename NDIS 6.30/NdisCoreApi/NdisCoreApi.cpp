// NdisCoreApi.cpp : 定义 DLL 应用程序的导出函数。
//
#include "NdisCoreApi.h"
#include"define.h"
NDISCOREAPI_API HANDLE Net_OpenFilter(void)
{
	HANDLE hDevice = CreateFile(SYM_NAME, GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	return hDevice;
}
NDISCOREAPI_API int Net_ShowAdapter(HANDLE FilterHandle,PIO_Packet Packet)
{
	DWORD ByteRet = 0;
	return DeviceIoControl(FilterHandle, IOCTL_SHOWADAPTER, NULL, NULL, Packet, sizeof(IO_Packet), &ByteRet, NULL);
}
NDISCOREAPI_API int Net_GetRawPacket(HANDLE FilterHandle, PIO_Packet Packet)
{
	DWORD ByteRet = 0;
	return DeviceIoControl(FilterHandle, IOCTL_GETRAWDATA, NULL, NULL, Packet, sizeof(IO_Packet), &ByteRet, NULL);
}
NDISCOREAPI_API int Net_GetRawPacket(void)
{
    return 42;
}