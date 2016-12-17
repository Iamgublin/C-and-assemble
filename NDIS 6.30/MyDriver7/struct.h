#pragma once
#pragma warning(disable:4201)
#pragma warning(disable:4100)
#define NDIS_WDM
#define NDIS630
#include<wdm.h>
#include<ndis.h>

#define FILTER_MAJOR_NDIS_VERSION   NDIS_FILTER_MAJOR_VERSION
#define FILTER_MINOR_NDIS_VERSION   NDIS_FILTER_MINOR_VERSION
#define NETCFGGUID L"{5cbf81bd-5055-47cd-9055-a76b2b4e3697}"
#define SERVICENAME L"ZlzNdisLwf"
#define DEVICE_NAME L"\\Device\\Zlz Ndis6.30 Filter Kernel-Mode Device"
#define SYM_NAME L"\\??\\Zlz Ndis6.30 Filter Kernel-Mode Device SymbolicLink"

PDRIVER_DISPATCH devcon;
typedef struct _S_PACKET
{
	int size;
	PVOID buffer;
}S_PACKET, *PS_PACKET;
typedef struct _FILTER_CONTEXT
{
	char magic[8];
	NDIS_HANDLE FilterHandle;
	NDIS_HANDLE NetBufferSendPool;
	BOOLEAN IsRunning;
	BOOLEAN IsFiltering;
	int FliterIndex;
	int CurrentRecvNum;
	PS_PACKET PacketRecvPool[100];
	KSPIN_LOCK PoolLock;
}FILTER_CONTEXT, *PFILTER_CONTEXT;
typedef struct _GLOBAL
{
	NDIS_HANDLE DriverHandle;
	int contextnum;
	PFILTER_CONTEXT context[20];
	PDEVICE_OBJECT FilterDev;
	UNICODE_STRING symname;
}GLOBAL, *PGLOBAL;
GLOBAL Global;