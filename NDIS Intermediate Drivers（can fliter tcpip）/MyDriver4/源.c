#include "head.h"
typedef NTSTATUS (*systemadddeviceFUNC)(IN PDRIVER_OBJECT  DriverObject,
	IN PDEVICE_OBJECT  PhysicalDeviceObject);
systemadddeviceFUNC sysadddevfunc;
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	DbgBreakPoint();
	NDIS_STATUS ndissta;
	for (int i = 0; i < global.mininum; i++)
	{
		ndissta = NdisIMDeInitializeDeviceInstance(global.miniportcontext[i]->Miniportadapterhandle);
	}
	if (global.driverhandle != NULL)
	{
		NdisIMDeregisterLayeredMiniport(global.driverhandle);
	}
	if (global.protocolhandle)
	{
		NdisDeregisterProtocol(&ndissta, global.protocolhandle);
	}
	return STATUS_SUCCESS;
}
NTSTATUS myAddDevice(
	IN PDRIVER_OBJECT  DriverObject,
	IN PDEVICE_OBJECT  PhysicalDeviceObject);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	DbgBreakPoint();
	driver->DriverUnload = unload;
    DbgPrint("%wZ", str);
	global.contextnum = 0;
	global.mininum = 0;
	driver->DriverUnload = unload;
	sysadddevfunc = driver->DriverExtension->AddDevice;
	driver->DriverExtension->AddDevice = myAddDevice;
	NDIS_STATUS ndissta;
	NDIS_HANDLE wraphandle=NULL;
	NDIS_MINIPORT_CHARACTERISTICS minicha;
	NDIS_PROTOCOL_CHARACTERISTICS pc;
	NDIS_STRING ns = NDIS_STRING_CONST("zlzpass");
	NdisZeroMemory(&global, sizeof(GLOBAL));
	NdisInitializeWrapper(&wraphandle, driver, str, NULL);
	if (wraphandle == NULL)
	{
		return STATUS_UNSUCCESSFUL;
	}
	NdisZeroMemory(&minicha, sizeof(NDIS_MINIPORT_CHARACTERISTICS));
	minicha.MajorNdisVersion = NDIS_MINIPORT_MAJOR_VERSION;
	minicha.MinorNdisVersion = NDIS_MINIPORT_MINOR_VERSION;
	minicha.InitializeHandler = MPInitialize;
	minicha.HaltHandler = MPHalt;
	minicha.SetInformationHandler = MPSetInformation;
	minicha.QueryInformationHandler = MPQueryInformation;
	minicha.ReturnPacketHandler = MPReturnPacket;
	minicha.ResetHandler = NULL;
	minicha.CheckForHangHandler = NULL;
	minicha.TransferDataHandler = MPTransferData;
	minicha.SendHandler = MPSend;
	ndissta=NdisIMRegisterLayeredMiniport(wraphandle, &minicha, sizeof(NDIS_MINIPORT_CHARACTERISTICS), &global.driverhandle);
	if (ndissta!=NDIS_STATUS_SUCCESS)
	{
		return STATUS_UNSUCCESSFUL;
	}
	pc.MajorNdisVersion = 5;
	pc.MinorNdisVersion = 0;
	pc.Name = ns;
	pc.CloseAdapterCompleteHandler = NdisProtCloseAdapterComplete;
	pc.SendCompleteHandler = NdisProtSendComplete;
	pc.TransferDataCompleteHandler = NdisProtTransferDataComplete;
	pc.ResetCompleteHandler = NdisProtResetComplete;
	pc.RequestCompleteHandler = NdisProtRequestComplete;
	pc.ReceiveHandler = NdisProtReceive;
	pc.ReceiveCompleteHandler = NdisProtReceiveComplete;
	pc.StatusHandler = NdisProtStatus;
	pc.StatusCompleteHandler = NdisProtStatusComplete;
	pc.BindAdapterHandler = NdisProtBindAdapter;
	pc.OpenAdapterCompleteHandler = NdisOpenAdapterComplete;
	pc.UnbindAdapterHandler = NdisProtUnbindAdapter;
	pc.UnloadHandler = NULL;
	pc.ReceivePacketHandler = NdisProtReceivePacket;
	pc.PnPEventHandler = NdisProtPnPEventHandler;
	pc.CoAfRegisterNotifyHandler = NdisProtRegisterAf;
	NdisRegisterProtocol(&ndissta,&global.protocolhandle, &pc, sizeof(NDIS_PROTOCOL_CHARACTERISTICS));
	if (ndissta != NDIS_STATUS_SUCCESS)
	{
		return STATUS_UNSUCCESSFUL;
	}
	NdisIMAssociateMiniport(global.driverhandle, global.protocolhandle);
    return STATUS_SUCCESS;
}
NTSTATUS myAddDevice(
	IN PDRIVER_OBJECT  DriverObject,
	IN PDEVICE_OBJECT  PhysicalDeviceObject)
{
	DbgBreakPoint();
	DbgPrint("1\n");
	return sysadddevfunc(DriverObject, PhysicalDeviceObject);
}