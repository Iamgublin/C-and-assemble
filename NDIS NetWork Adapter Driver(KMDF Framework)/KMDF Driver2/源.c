#include"miniportfunction.h"
VOID miniunload(PDRIVER_OBJECT driver);
NTSTATUS unload(PDRIVER_OBJECT driver);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	DbgBreakPoint();
	NTSTATUS sta;
	NDIS_STATUS ndissta;
	WDF_DRIVER_CONFIG config;
	NDIS_HANDLE wraphandle;
	driver->DriverUnload = unload;
	global.adapternum = 0;
	WDF_DRIVER_CONFIG_INIT(&config, WDF_NO_EVENT_CALLBACK);
	sta = WdfDriverCreate(driver, str, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
	NdisMInitializeWrapper(&wraphandle, driver, str, NULL);
	NDIS_MINIPORT_CHARACTERISTICS minicha;
	NdisZeroMemory(&minicha, sizeof(NDIS_MINIPORT_CHARACTERISTICS));
	minicha.MajorNdisVersion = NDIS_MINIPORT_MAJOR_VERSION;
	minicha.MinorNdisVersion = NDIS_MINIPORT_MINOR_VERSION;
	minicha.InitializeHandler = MPInitialize;
	minicha.HaltHandler = MPHalt;
	minicha.SetInformationHandler = MPSetInformation;
	minicha.QueryInformationHandler = MPQueryInformation;
	minicha.SendPacketsHandler = MPSendPackets;
	minicha.ReturnPacketHandler = MPReturnPacket;
	minicha.ResetHandler = NULL;
	minicha.CheckForHangHandler = MPCheckForHang;
	ndissta=NdisMRegisterMiniport(wraphandle, &minicha,sizeof(NDIS_MINIPORT_CHARACTERISTICS));
	global.wraphandle = wraphandle;
	NdisMRegisterUnloadHandler(wraphandle, miniunload);
	NdisAllocateSpinLock(&global.lock);
	NdisInitializeListHead(&global.list);
	return sta;
}
VOID miniunload(PDRIVER_OBJECT driver)
{
	DbgBreakPoint();
	UNREFERENCED_PARAMETER(driver);
}
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	return STATUS_SUCCESS;
}