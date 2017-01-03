#include<Windows.h>
#ifdef NDISCOREAPI_EXPORTS
#define NDISCOREAPI_API __declspec(dllexport)
#else
#define NDISCOREAPI_API __declspec(dllimport)
#endif
#define PATH_MAX 255
typedef struct _AdapterInfo
{
	WCHAR DevPathName[PATH_MAX];
	WCHAR DevName[PATH_MAX];
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
			int Num;
			UCHAR Buffer[300][2000];
		}Net_Packet;
		unsigned u;
	}Packet;
}IO_Packet, *PIO_Packet;
// 此类是从 NdisCoreApi.dll 导出的
NDISCOREAPI_API int Net_ShowAdapter(HANDLE FilterHandle, PIO_Packet Packet);
NDISCOREAPI_API HANDLE Net_OpenFilter(void);
NDISCOREAPI_API int Net_GetRawPacket(HANDLE FilterHandle, PIO_Packet Packet);