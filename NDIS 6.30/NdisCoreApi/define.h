#pragma once
#include<stdio.h>
#define SYM_NAME L"\\\\.\\Zlz Ndis6.30 Filter SymbolLink"
#define PATH_MAX 255
#define IO_BUF 2000
#define PACKET_TYPE_ADAPTERINFO 1
#define PACKET_TYPE_NETPACKET 2
#define IOCTL_SHOWADAPTER (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x911,METHOD_BUFFERED,FILE_WRITE_DATA|FILE_READ_DATA)
#define IOCTL_GETRAWDATA (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x912,METHOD_BUFFERED,FILE_WRITE_DATA|FILE_READ_DATA) 

typedef struct _AdapterInfo
{
	WCHAR DevPathName[PATH_MAX];
	WCHAR DevName[PATH_MAX];
	UCHAR MacAddress[32];
}AdapterInfo, *PAdapterInfo;
typedef struct _IO_Packet
{
	int Type;
	union
	{
		struct
		{
			int Num;
			AdapterInfo AdapterInfo[20];
		}ShowAdapter;
		struct
		{
			int Size;
			UCHAR Buffer[IO_BUF];         //MTU<=1500 
			BOOLEAN IsSendPacket;
		}Net_Packet_Output;
		struct
		{
			int ContextNum;
		}Net_Packet_InPut;
		unsigned u;
	}Packet;
}IO_Packet, *PIO_Packet;