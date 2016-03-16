#include<ntifs.h>
#include<ntddk.h>
#include<tdikrnl.h>
#define TCP L"\\Device\\Tcp"
#define UDP L"\\Device\\Udp"
#define IP L"\\Device\\RawIp"
enum DevType
{
	DEVTYPE_TCP = 0,
	DEVTYPE_UDP,
	DEVTYPE_IP
} devty;
typedef struct _CONTEXTI
{
	PDEVICE_OBJECT realdev;
	PIRP query_irp;
	PMDL mdl;
}*PCONTEXTI, CONTEXTI;
typedef struct  {
	union {
		struct { unsigned char s_b1, s_b2, s_b3, s_b4; } S_un_b;
		struct { unsigned short s_w1, s_w2; } S_un_w;
		unsigned long S_addr;
	} S_un;
}in_addr,*pin_addr;
PDRIVER_OBJECT dri;
PDEVICE_OBJECT
tcpreal = NULL,
udpreal = NULL,
ipreal = NULL,
tcpflt = NULL,
udpflt = NULL,
ipflt = NULL,
tcptop = NULL,
udptop = NULL,
iptop = NULL;
NTSTATUS attach(PUNICODE_STRING tardevname, enum DevType type);
NTSTATUS unload(PDRIVER_OBJECT driver);
NTSTATUS dispatch(PDEVICE_OBJECT dev, PIRP irp);
enum DevType check(PDEVICE_OBJECT dev);
NTSTATUS createcomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
NTSTATUS querycom(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	dri = driver;
	driver->DriverUnload = unload;
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = dispatch;
	}
	PFILE_OBJECT file;
	UNICODE_STRING tcpname;
	UNICODE_STRING udpname;
	UNICODE_STRING ipname;
	RtlInitUnicodeString(&tcpname, TCP);
	RtlInitUnicodeString(&udpname, UDP);
	RtlInitUnicodeString(&ipname, IP);
	IoGetDeviceObjectPointer(&udpname, FILE_ALL_ACCESS, &file, &udpreal);
	ObfDereferenceObject(file);
	IoGetDeviceObjectPointer(&tcpname, FILE_ALL_ACCESS, &file, &tcpreal);
	ObfDereferenceObject(file);
	IoGetDeviceObjectPointer(&ipname, FILE_ALL_ACCESS, &file, &ipreal);
	ObfDereferenceObject(file);
	attach(&tcpname, DEVTYPE_TCP);
	attach(&udpname, DEVTYPE_UDP);
	attach(&ipname, DEVTYPE_IP);
	return STATUS_SUCCESS;
}
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	if (tcpflt != NULL)
	{
		IoDetachDevice(tcpflt);
		IoDeleteDevice(tcptop);
	}
	if (udpflt != NULL)
	{
		IoDetachDevice(udpflt);
		IoDeleteDevice(udptop);
	}
	if (ipflt != NULL)
	{
		IoDetachDevice(ipflt);
		IoDeleteDevice(iptop);
	}
	return STATUS_SUCCESS;
}
NTSTATUS attach(PUNICODE_STRING tardevname, enum DevType type)
{
	UNREFERENCED_PARAMETER(tardevname);
	UNREFERENCED_PARAMETER(type);
	NTSTATUS sta;
	if (type == DEVTYPE_TCP)
	{
		sta = IoCreateDevice(dri, 0, NULL, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, TRUE, &tcptop);
		DbgPrint("tcp0x%x\n", sta);
		tcptop->Flags |= DO_DIRECT_IO;
		sta = IoAttachDevice(tcptop, tardevname, &tcpflt);
		DbgPrint("0x%x\n", sta);
	}
	if (type == DEVTYPE_UDP)
	{
		sta = IoCreateDevice(dri, 0, NULL, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, TRUE, &udptop);
		DbgPrint("udp0x%x\n", sta);
		udptop->Flags |= DO_DIRECT_IO;
		sta = IoAttachDevice(udptop, tardevname, &udpflt);
		DbgPrint("0x%x\n", sta);
	}
	if (type == DEVTYPE_IP)
	{
		sta = IoCreateDevice(dri, 0, NULL, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, TRUE, &iptop);
		DbgPrint("ip0x%x\n", sta);
		iptop->Flags |= DO_DIRECT_IO;
		sta = IoAttachDevice(iptop, tardevname, &ipflt);
		DbgPrint("0x%x\n", sta);
	}
	return STATUS_SUCCESS;
}
NTSTATUS dispatch(PDEVICE_OBJECT dev, PIRP irp)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	PDEVICE_OBJECT realobj;
	PDEVICE_OBJECT lowobj;
	ULONG pid;
	DbgPrint("enter the dispatch\n");
	switch (check(dev))
	{
	case DEVTYPE_IP:
		DbgPrint("this is ip\n");
		lowobj = ipflt;
		realobj = ipreal;
		break;
	case DEVTYPE_UDP:
		DbgPrint("this is udp\n");
		lowobj = udpflt;
		realobj = udpreal;
		break;
	case DEVTYPE_TCP:
		DbgPrint("this is tcp\n");
		lowobj = tcpflt;
		realobj = tcpreal;
		break;
	default:
		DbgPrint("error!\n");
		return STATUS_UNSUCCESSFUL;
	}
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	switch (sa->MajorFunction)
	{
	case IRP_MJ_CREATE:
		pid = (ULONG)PsGetCurrentProcessId();
		DbgPrint("Á´½ÓÍøÂçµÄPIDÎª:%d\n", pid);
		FILE_FULL_EA_INFORMATION *ea = irp->AssociatedIrp.SystemBuffer;
		if (ea != NULL)
		{
			if (ea->EaNameLength == TDI_CONNECTION_CONTEXT_LENGTH)
			{
				DbgPrint("this is tdi conn\n");
				CONNECTION_CONTEXT *cc = *(CONNECTION_CONTEXT*)(ea->EaName + ea->EaNameLength + 1);
				UNREFERENCED_PARAMETER(cc);
				break;
			}
			if (ea->EaNameLength == TDI_TRANSPORT_ADDRESS_LENGTH)
			{
				DbgPrint("this is tdi address\n");
				IoCopyCurrentIrpStackLocationToNext(irp);
				PCONTEXTI context = ExAllocatePool(NonPagedPool, sizeof(CONTEXTI));
				PIRP query_irp = TdiBuildInternalDeviceControlIrp(TDI_QUERY_INFORMATION, realobj, sa->FileObject, NULL, NULL);
				context->query_irp = query_irp;
				context->realdev = realobj;
				IoSetCompletionRoutine(irp, createcomplete, context, TRUE, TRUE, TRUE);
				return IoCallDriver(lowobj, irp);
			}
		}
		break;
	case IRP_MJ_DEVICE_CONTROL:
		if (!NT_SUCCESS(TdiMapUserRequest(dev, irp, sa)))
		{
			break;
		}
	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		if (sa->MinorFunction == TDI_CONNECT)
		{
			DbgPrint("tdi connect\n\n");
			TA_ADDRESS *ta;
			TDI_REQUEST_KERNEL_CONNECT *trk = (TDI_REQUEST_KERNEL_CONNECT *)(&sa->Parameters);
			ta = ((TRANSPORT_ADDRESS*)(trk->RequestConnectionInformation->RemoteAddress))->Address;
			TA_IP_ADDRESS *ip = (TA_IP_ADDRESS*)&(ta->Address);
			in_addr *ia = (in_addr*)&(ip->Address->Address->in_addr);
			DbgPrint("%d.%d.%d.%d:%d\n",
				ia->S_un.S_un_b.s_b1,
				ia->S_un.S_un_b.s_b2,
				ia->S_un.S_un_b.s_b3,
				ia->S_un.S_un_b.s_b4,
				RtlUshortByteSwap(((TDI_ADDRESS_IP*)(ip->Address))->sin_port));
		}
		if (sa->MinorFunction == TDI_SET_EVENT_HANDLER)
		{
			DbgPrint("tdi set event handler\n");
			PTDI_REQUEST_KERNEL_SET_EVENT r = (PTDI_REQUEST_KERNEL_SET_EVENT)&(sa->Parameters);
			if (r->EventType == TDI_EVENT_CONNECT&&r->EventHandler != NULL)
			{
				/*PVOID my_handler;
				PVOID oldcontext = r->EventContext;
				PVOID old_hand = r->EventHandler;
				r->EventHandler = my_handler;*/
			}
		}
		break;
	}
	IoSkipCurrentIrpStackLocation(irp);
	return IoCallDriver(lowobj, irp);
}
enum DevType check(PDEVICE_OBJECT dev)
{
	if (dev == tcptop)
	{
		return DEVTYPE_TCP;
	}
	if (dev == udptop)
	{
		return DEVTYPE_UDP;
	}
	if (dev == iptop)
	{
		return DEVTYPE_IP;
	}
	return -1;
}
NTSTATUS createcomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	UNREFERENCED_PARAMETER(context);
	DbgPrint("enter createcomplete\n");
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	PCONTEXTI info = context;
	DbgPrint("1\n");
	TDI_ADDRESS_INFO *tai = ExAllocatePool(NonPagedPool, sizeof(TDI_ADDRESS_INFO));
	DbgPrint("2\n");
	PMDL mdl = IoAllocateMdl(tai, sizeof(TDI_ADDRESS_INFO), FALSE, FALSE, info->query_irp);
	MmBuildMdlForNonPagedPool(mdl);
	info->mdl = mdl;
	DbgPrint("3\n");
	IoCopyCurrentIrpStackLocationToNext(irp);
	TdiBuildQueryInformation(info->query_irp, info->realdev, sa->FileObject, querycom, info, TDI_QUERY_ADDRESS_INFO, mdl);
	DbgPrint("4\n");
	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
		DbgPrint("pending\n");
	}
	IoCallDriver(info->realdev, info->query_irp);
	DbgPrint("6\n");
	IoFreeMdl(info->mdl);
	ExFreePool(context);
	return STATUS_SUCCESS;
}
NTSTATUS querycom(PDEVICE_OBJECT dev, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	UNREFERENCED_PARAMETER(context);
	DbgPrint("5\n");
	if (irp->MdlAddress)
	{
		TDI_ADDRESS_INFO *tai = (TDI_ADDRESS_INFO*)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		TA_ADDRESS *addr = tai->Address.Address;
		TA_IP_ADDRESS *ip = (TA_IP_ADDRESS*)&(addr->Address);
		in_addr *ia = (in_addr*)&(ip->Address->Address->in_addr);
		DbgPrint("%d.%d.%d.%d:%d\n",
			ia->S_un.S_un_b.s_b1 , 
			ia->S_un.S_un_b.s_b2,
			ia->S_un.S_un_b.s_b3,
			ia->S_un.S_un_b.s_b4,
			RtlUshortByteSwap(((TDI_ADDRESS_IP*)(addr->Address))->sin_port));
	}
	return STATUS_MORE_PROCESSING_REQUIRED;
}