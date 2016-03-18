#include"protocol.h"

#define NT_DEVICE_NAME L"\\Device\\NDIS protocol device create by zlz"
#define NT_DEVICE_WIN_NAME L"\\??\\WIN NDIS protocol device create by zlz"
#define IOCTL_NDISPROT_BIND_WAIT (ULONG)CTL_CODE(FILE_DEVICE_NETWORK,0x912,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_NDISPROT_OPEN_DEVICE (ULONG)CTL_CODE(FILE_DEVICE_NETWORK,0x913,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_NDISPROT_OUTPUT_CURRENT_NETWORKCARD (ULONG)CTL_CODE(FILE_DEVICE_NETWORK,0x914,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define NUIOO_PACKET_FILTER  (NDIS_PACKET_TYPE_DIRECTED|    \
                              NDIS_PACKET_TYPE_MULTICAST|   \
                              NDIS_PACKET_TYPE_BROADCAST)
NTSTATUS unload(PDRIVER_OBJECT driver);
NTSTATUS ndiscreate(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS ndisclose(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS ndisread(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS ndiswrite(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS ndisclean(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS ndisdevcon(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	driver->DriverUnload = unload;
	NDIS_PROTOCOL_CHARACTERISTICS pc;
	NTSTATUS sta;
	NDIS_STATUS ndissta;
	NDIS_STRING ns = NDIS_STRING_CONST("ndisprot");
	UNICODE_STRING devname;
	UNICODE_STRING windevname;
	PDEVICE_OBJECT dev;
	Globals.driver = driver;
	Globals.contextnum = 0;
	Globals.bindinghandlenum = 0;
	do
	{
		RtlInitUnicodeString(&devname, NT_DEVICE_NAME);
		sta=IoCreateDeviceSecure(driver,
			0,
			&devname,
			FILE_DEVICE_NETWORK,
			FILE_DEVICE_SECURE_OPEN,
			FALSE, 
			&SDDL_DEVOBJ_SYS_ALL_ADM_ALL,
			NULL,
			&dev);
		DbgPrint("0x%x\n", sta);
		if (!NT_SUCCESS(sta))
		{
			break;
		}
		RtlInitUnicodeString(&windevname, NT_DEVICE_WIN_NAME);
		sta = IoCreateSymbolicLink(&windevname, &devname);
		DbgPrint("0x%x\n", sta);
		if (!NT_SUCCESS(sta))
		{
			break;
		}
		dev->Flags |= DO_DIRECT_IO;
		dev->Flags &= ~DO_DEVICE_INITIALIZING;
		Globals.dev = dev;
		NdisZeroMemory(&pc, sizeof(NDIS_PROTOCOL_CHARACTERISTICS));
		pc.MajorNdisVersion = 5;
		pc.MinorNdisVersion = 0;
		pc.Name = ns;
		pc.OpenAdapterCompleteHandler = NdisProtOpenAdapterComplete;
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
		pc.UnbindAdapterHandler = NdisProtUnbindAdapter;
		pc.UnloadHandler = NULL;
		pc.ReceivePacketHandler = NdisProtReceivePacket;
		pc.PnPEventHandler = NdisProtPnPEventHandler;
		NdisRegisterProtocol(&ndissta, &Globals.ndisprotocolhandle, &pc, sizeof(pc));
		DbgPrint("0x%x\n", ndissta);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		driver->MajorFunction[IRP_MJ_CREATE] = ndiscreate;
		driver->MajorFunction[IRP_MJ_CLOSE] = ndisclose;
		driver->MajorFunction[IRP_MJ_READ] = ndisread;
		driver->MajorFunction[IRP_MJ_WRITE] = ndiswrite;
		driver->MajorFunction[IRP_MJ_CLEANUP] = ndisclean;
		driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ndisdevcon;
	} while (FALSE);
	return STATUS_SUCCESS;
}
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	UNICODE_STRING windevname;
	NTSTATUS sta;
	NDIS_STATUS ndissta;
	NdisDeregisterProtocol(&ndissta, Globals.ndisprotocolhandle);
	RtlInitUnicodeString(&windevname, NT_DEVICE_WIN_NAME);
	sta=IoDeleteSymbolicLink(&windevname);
	DbgPrint("0x%x\n", sta);
	IoDeleteDevice(Globals.dev);
	for (int num = 0; num < Globals.contextnum; num++)
	{
		ExFreePool(Globals.context[num]);
	}
	return STATUS_SUCCESS;
}
NTSTATUS ndiscreate(PDEVICE_OBJECT device, PIRP irp)
{
	DbgPrint("open success\n");
	return STATUS_SUCCESS;
}
NTSTATUS ndisclose(PDEVICE_OBJECT device, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS ndisread(PDEVICE_OBJECT device, PIRP irp)
{
	NTSTATUS sta = STATUS_SUCCESS;
	PVOID buffer = NULL;
	do
	{
		if (irp->MdlAddress == NULL)
		{
			DbgPrint("mdl is null!\n");
			sta = STATUS_UNSUCCESSFUL;
			break;
		}
		buffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		if (buffer == NULL)
		{
			DbgPrint("mdl get error!\n");
			sta = STATUS_UNSUCCESSFUL;
			break;
		}
	} while (FALSE);
	return sta;
}
NTSTATUS ndiswrite(PDEVICE_OBJECT device, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS ndisclean(PDEVICE_OBJECT device, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS ndisdevcon(PDEVICE_OBJECT device, PIRP irp)
{
	NTSTATUS sta;
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	switch (sa->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_NDISPROT_BIND_WAIT:
		break;
	case IOCTL_NDISPROT_OPEN_DEVICE:
	   {
		ULONG PacketFilter = NUIOO_PACKET_FILTER;
		ULONG byteret;
		for (int i = 0; i < Globals.contextnum; i++)
		{
			sta = ndisprotDoRequest(Globals.context[i], NdisRequestSetInformation, OID_GEN_CURRENT_PACKET_FILTER, &PacketFilter, sizeof(PacketFilter), &byteret);
		}
		break;
	   }
	case IOCTL_NDISPROT_OUTPUT_CURRENT_NETWORKCARD:
		DbgPrint("current networkcard:%d\n", Globals.bindinghandlenum);
		break;
	default:
		break;
	}
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
} 