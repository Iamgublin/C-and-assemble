// NdisCoreApi.cpp : 定义 DLL 应用程序的导出函数。
//
#include "NdisCoreApi.h"
#include"define.h"
NDISCOREAPI_API HANDLE WINAPI Net_OpenFilter(void)
{
	return CreateFile(SYM_NAME, GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
}
NDISCOREAPI_API int  WINAPI Net_ShowAdapter(HANDLE FilterHandle,PIO_Packet Packet)
{
	DWORD ByteRet = 0;
	return DeviceIoControl(FilterHandle, IOCTL_SHOWADAPTER, NULL, NULL, Packet, sizeof(IO_Packet), &ByteRet, NULL);
}
NDISCOREAPI_API int WINAPI Net_GetRawPacket(HANDLE FilterHandle, PIO_Packet Packet,int AdapterIndex)
{
	IO_Packet PacketInput = { 0 };
	PacketInput.Packet.Net_Packet_InPut.ContextNum = AdapterIndex;
	DWORD ByteRet = 0;
	return DeviceIoControl(FilterHandle, IOCTL_GETRAWDATA, &PacketInput, sizeof(IO_Packet), Packet, sizeof(IO_Packet), &ByteRet, NULL);
}