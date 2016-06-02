#include "head.h"
#define DEVICE_NAME L"\\Device\\ZLZNDIS Intermediate Drivers"
#define SYM_NAME L"\\??\\ZLZNDIS Intermediate Drivers"
typedef NTSTATUS (*systemadddeviceFUNC)(IN PDRIVER_OBJECT  DriverObject,
	IN PDEVICE_OBJECT  PhysicalDeviceObject);
systemadddeviceFUNC sysadddevfunc;
NTSTATUS ZlzCreateDevice(PDRIVER_OBJECT driver, NDIS_HANDLE wraphandle);
NTSTATUS ZlzIoControl(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS ZlzDeviceCreate(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS ZlzDeviceClose(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS ZlzDeviceCleanUp(PDEVICE_OBJECT dev, PIRP irp);
VOID unload(PDRIVER_OBJECT driver)
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
	if (global.controlobj)
	{
		UNICODE_STRING symname;
		RtlInitUnicodeString(&symname, SYM_NAME);
		IoDeleteSymbolicLink(&symname);
		IoDeleteDevice(global.controlobj);
	}
}
NTSTATUS myAddDevice(
	IN PDRIVER_OBJECT  DriverObject,
	IN PDEVICE_OBJECT  PhysicalDeviceObject);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	DbgBreakPoint();
	NTSTATUS sta;
	global.contextnum = 0;
	global.mininum = 0;
	globalinfopool.count = 0;
	global.controlobj = NULL;
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
	sysadddevfunc = driver->DriverExtension->AddDevice;

	driver->DriverExtension->AddDevice = myAddDevice;

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

	sta = ZlzCreateDevice(driver,wraphandle);
	if (!NT_SUCCESS(sta))
	{
		return STATUS_UNSUCCESSFUL;
	}

	NdisMRegisterUnloadHandler(wraphandle, unload);
    return STATUS_SUCCESS;
}
NTSTATUS myAddDevice(
	IN PDRIVER_OBJECT  DriverObject,
	IN PDEVICE_OBJECT  PhysicalDeviceObject)
{
	DbgBreakPoint();
	WCHAR classname[100];
	ULONG longret;
	IoGetDeviceProperty(PhysicalDeviceObject, DevicePropertyClassName, 100, classname, &longret);
	DbgPrint("class name:%ws\n", classname);
	return sysadddevfunc(DriverObject, PhysicalDeviceObject);
}
NTSTATUS ZlzCreateDevice(PDRIVER_OBJECT driver,NDIS_HANDLE wraphandle)
{
	DbgBreakPoint();
	NDIS_STATUS sta;
	UNICODE_STRING devname;
	UNICODE_STRING symname;
	PDRIVER_DISPATCH dispatchfunc[IRP_MJ_MAXIMUM_FUNCTION + 1];
	RtlInitUnicodeString(&devname, DEVICE_NAME);
	RtlInitUnicodeString(&symname, SYM_NAME);
	dispatchfunc[IRP_MJ_CREATE] = ZlzDeviceCreate;
	dispatchfunc[IRP_MJ_DEVICE_CONTROL] = ZlzIoControl;
	dispatchfunc[IRP_MJ_CLOSE] = ZlzDeviceClose;
	dispatchfunc[IRP_MJ_CLEANUP] = ZlzDeviceCleanUp;
	sta=NdisMRegisterDevice(wraphandle, &devname, &symname, dispatchfunc, &global.controlobj, &global.controlhand);
	global.controlobj->Flags |= DO_BUFFERED_IO;
	global.controlobj->Flags &= ~DO_DEVICE_INITIALIZING;
	if (sta!=NDIS_STATUS_SUCCESS)
	{
		return sta;
	}
	sta = IoCreateSymbolicLink(&symname, &devname);
	return STATUS_SUCCESS;
}
NTSTATUS ZlzIoControl(PDEVICE_OBJECT dev, PIRP irp)
{
		DbgBreakPoint();
		PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
		switch (sa->Parameters.DeviceIoControl.IoControlCode)
		{
		case IOCTL_GETIPPACKET:
			irp->IoStatus.Information = sizeof(ippackinfo) * 100;
			irp->IoStatus.Status = STATUS_SUCCESS;
			/*irp->AssociatedIrp.SystemBuffer = ExAllocatePoolWithTag(NonPagedPool, sizeof(ippackinfo) * 100, 0);*/
			PVOID buffer = irp->AssociatedIrp.SystemBuffer;
			buffer;
			RtlCopyMemory(irp->AssociatedIrp.SystemBuffer, globalinfopool.packet, sizeof(ippackinfo) * 100);
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return STATUS_SUCCESS;
		default:
			return STATUS_SUCCESS;
			break;
		}
}
NTSTATUS ZlzDeviceCreate(PDEVICE_OBJECT dev, PIRP irp)
{
		DbgBreakPoint();
		DbgPrint("Creae Success!\n");
		return STATUS_SUCCESS;
}
NTSTATUS ZlzDeviceClose(PDEVICE_OBJECT dev, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS ZlzDeviceCleanUp(PDEVICE_OBJECT dev, PIRP irp)
{
	return STATUS_SUCCESS;
}