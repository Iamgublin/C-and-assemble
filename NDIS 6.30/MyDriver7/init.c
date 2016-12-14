#include"struct.h"
#include"filter.h"
#define FILTER_MAJOR_NDIS_VERSION   NDIS_FILTER_MAJOR_VERSION
#define FILTER_MINOR_NDIS_VERSION   NDIS_FILTER_MINOR_VERSION
VOID Unload(PDRIVER_OBJECT driver)
{
	DbgBreakPoint();
	NdisFDeregisterFilterDriver(Global.DrverHandle);
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	NDIS_FILTER_DRIVER_CHARACTERISTICS      FChars;
	NDIS_STATUS sta;
	NDIS_STRING FriendlyName = RTL_CONSTANT_STRING(L"Zlz NDIS KernelMode Driver");
	NDIS_STRING UniqueName = RTL_CONSTANT_STRING(NETCFGGUID);
	NDIS_STRING ServiceName = RTL_CONSTANT_STRING(SERVICENAME);
	Global.contextnum = 0;

	NdisZeroMemory(&FChars, sizeof(NDIS_FILTER_DRIVER_CHARACTERISTICS));
	FChars.Header.Type = NDIS_OBJECT_TYPE_FILTER_DRIVER_CHARACTERISTICS;
	FChars.Header.Size = sizeof(NDIS_FILTER_DRIVER_CHARACTERISTICS);
#if NDIS_SUPPORT_NDIS61
	FChars.Header.Revision = NDIS_FILTER_CHARACTERISTICS_REVISION_2;
#else
	FChars.Header.Revision = NDIS_FILTER_CHARACTERISTICS_REVISION_1;
#endif
	FChars.MajorNdisVersion = FILTER_MAJOR_NDIS_VERSION;
	FChars.MinorNdisVersion = FILTER_MINOR_NDIS_VERSION;
	FChars.FriendlyName = FriendlyName;
	FChars.UniqueName = UniqueName;
	FChars.ServiceName = ServiceName;
	FChars.CancelSendNetBufferListsHandler = FilterCancelSendNetBufferLists;
	FChars.NetPnPEventHandler = FilterNetPnPEvent;
	FChars.AttachHandler = FilterAttach;
	FChars.CancelDirectOidRequestHandler = NULL;
	FChars.CancelOidRequestHandler = NULL;
	FChars.DetachHandler = FilterDetach;
	FChars.DevicePnPEventNotifyHandler = NULL;
	FChars.DirectOidRequestCompleteHandler = NULL;
	FChars.DirectOidRequestHandler = NULL;
	FChars.Flags = 0;
	FChars.OidRequestCompleteHandler = NULL;
	FChars.OidRequestHandler = NULL;
	FChars.PauseHandler = FilterPause;
	FChars.ReceiveNetBufferListsHandler = FilterReceiveNetBufferLists;
	FChars.RestartHandler = FilterRestart;
	FChars.ReturnNetBufferListsHandler = FilterReturnNetBufferLists;
	FChars.SendNetBufferListsCompleteHandler = FilterSendNetBufferListsComplete;
	FChars.SendNetBufferListsHandler = FilterSendNetBufferLists;
	FChars.SetFilterModuleOptionsHandler = FilterSetModuleOptions;
	FChars.SetOptionsHandler = FilterSetOptions;
	FChars.StatusHandler = NULL;
	sta = NdisFRegisterFilterDriver(driver, NULL, &FChars, &Global.DrverHandle);
	DbgPrint("0x%x\n", sta);
	if (!NT_SUCCESS(sta))
	{
		return STATUS_UNSUCCESSFUL;
	}
	return 0;
}