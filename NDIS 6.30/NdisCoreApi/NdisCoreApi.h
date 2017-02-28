/********************************************************************************
*                                                                               *
* NdisCoreApi.h -- ApiSet Contract for ZlzNdis Kernel-Mode Driver               *
*                                                                               *
* Copyright (c) Microsoft Corporation. All rights reserved.                     *
*                                                                               *
********************************************************************************/
#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#ifndef _APICORENDIS_
#define _APICORENDIS_
#include<Windows.h>
#include"define.h"
#ifdef NDISCOREAPI_EXPORTS
#define NDISCOREAPI_API __declspec(dllexport)
#else
#define NDISCOREAPI_API __declspec(dllimport)
#endif


NDISCOREAPI_API 
int 
WINAPI 
Net_ShowAdapter(
	_In_ HANDLE FilterHandle,
	_Out_ PIO_Packet Packet
);

NDISCOREAPI_API 
HANDLE 
WINAPI 
Net_OpenFilter(
	void
);

NDISCOREAPI_API 
int 
WINAPI 
Net_GetRawPacket(
	_In_ HANDLE FilterHandle,
	_Out_ PIO_Packet Packet,
	_In_ int AdapterIndex
);

NDISCOREAPI_API
int
WINAPI
Net_StartFilter(
	_In_ HANDLE FilterHandle,
	_In_opt_ int *StartFileterIndex
);

NDISCOREAPI_API
int
WINAPI
Net_StopFilter(
	_In_ HANDLE FilterHandle,
	_In_opt_ int *StopFileterIndex
);
#endif //_APICORENDIS_