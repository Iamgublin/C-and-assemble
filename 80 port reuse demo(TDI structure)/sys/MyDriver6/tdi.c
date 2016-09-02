#include"head.h"
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	dri = driver;
	globals.count = 0;
	driver->DriverUnload = unload;
	
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = dispatch;
	}
	PFILE_OBJECT file;
	UNICODE_STRING tcpname;
	RtlInitUnicodeString(&tcpname, TCP);
	IoGetDeviceObjectPointer(&tcpname, FILE_ALL_ACCESS, &file, &tcpreal);

	//方式一：hook tcpip.sys(稳定性较低)
	/*DbgBreakPoint();
	NTSTATUS status;
	PDRIVER_OBJECT new_DriverObject;
	UNICODE_STRING drv_name;
	RtlInitUnicodeString(&drv_name, TCPDRI);
	status=ObReferenceObjectByName(
		&drv_name,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		&new_DriverObject);
	if (status != STATUS_SUCCESS)
	{
		KdPrint(("[tdi_hook] hook_driver: ObReferenceObjectByName failed.\n"));
		return status;
	}
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		new_DriverObject->MajorFunction[i] = dispatch;
	}
	tcpflt = IoGetLowerDeviceObject(new_DriverObject->DeviceObject);*/

	ObfDereferenceObject(file);


	//方式二：设置TDI过滤层
	attach(&tcpname, DEVTYPE_TCP);
	UNICODE_STRING devname;
	UNICODE_STRING synname;
	RtlInitUnicodeString(&devname, DEVICE_NAME);
	RtlInitUnicodeString(&synname, SYN_NAME);
	IoCreateSymbolicLink(&synname, &devname);
	IoCreateDevice(dri, 0, &devname, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, TRUE,&condev);
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
	if (condev != NULL)
	{
		IoDeleteDevice(condev);
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
		DbgPrint("this is condev\n");
		lowobj = tcpflt;
		realobj=tcpreal;
	}
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	switch (sa->MajorFunction)
	{
	case IRP_MJ_CREATE:
		if (dev == condev)
		{
			return STATUS_SUCCESS;
		}
		pid = (ULONG)PsGetCurrentProcessId();
		DbgPrint("链接网络的PID为:%d\n", pid);
		FILE_FULL_EA_INFORMATION *ea = irp->AssociatedIrp.SystemBuffer;
		if (ea != NULL)
		{
			if (ea->EaNameLength == TDI_CONNECTION_CONTEXT_LENGTH)      //建立连接终端
			{
				DbgPrint("this is tdi conn\n");
				CONNECTION_CONTEXT *cc = *(CONNECTION_CONTEXT*)(ea->EaName + ea->EaNameLength + 1);
				UNREFERENCED_PARAMETER(cc);
				break;
			}
			if (ea->EaNameLength == TDI_TRANSPORT_ADDRESS_LENGTH)       //生成地址句柄
			{
				DbgPrint("this is tdi address\n");
			}
		}
		break;
	case IRP_MJ_DEVICE_CONTROL:
		if (dev == condev)
		{
			return tdi_io_control(dev, irp);
		}
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
			TDI_ADDRESS_IP* ipaddr = (TDI_ADDRESS_IP*)(&ta->Address[0]);
			DbgPrint(" ==> %u.%u.%u.%u",
				ipaddr->in_addr & 0xff,
				(ipaddr->in_addr & 0xff00) >> 8,
				(ipaddr->in_addr & 0xff0000) >> 16,
				(ipaddr->in_addr & 0xff000000) >> 24);
			DbgPrint("port = %d\r\n", tdi_ntohs(ipaddr->sin_port));
		}
		if (sa->MinorFunction == TDI_SEND)
		{
			DbgPrint("tdi send");
		}
		if (sa->MajorFunction == TDI_ACCEPT)
		{
			DbgPrint("tdi accept");
		}
		if (sa->MinorFunction == TDI_RECEIVE)
		{
			DbgPrint("tdi receive");
		}
		if (sa->MinorFunction == TDI_ASSOCIATE_ADDRESS)    //FILE_OBJECT（连接终端）绑定地址句柄时（如打开一个网页http协议建立连接）调用
		{
			DbgPrint("TDI ASSOCIATE\n");
		}
		if (sa->MinorFunction == TDI_DISASSOCIATE_ADDRESS)  //FILE_OBJECT解除绑定地址句柄时（http断开连接时）调用
		{
			DbgPrint("TDI DISASSOCIATE\n");
		}
		if (sa->MinorFunction == TDI_SET_EVENT_HANDLER)
		{
			DbgPrint("tdi set event handler\n");
			PTDI_REQUEST_KERNEL_SET_EVENT r = (PTDI_REQUEST_KERNEL_SET_EVENT)&(sa->Parameters);
			if (r->EventType == TDI_EVENT_CONNECT&&r->EventHandler != NULL)    //服务器端被连接时调用（保存File_object）
			{
				DbgBreakPoint();
				temp_context* context = (ptemp_context)ExAllocatePool(NonPagedPool, sizeof(temp_context));
				context->old_context = r->EventContext;
				context->old_handler = r->EventHandler;
				context->magic = 0x907690;
				r->EventContext = context;
				r->EventHandler = (PVOID)my_ClientEventConnect;
			}
			if (r->EventType == TDI_EVENT_RECEIVE&&r->EventHandler != NULL)
			{
				temp_context* context=(ptemp_context)ExAllocatePool(NonPagedPool,sizeof(temp_context));
			    context->old_context = r->EventContext;
				context->old_handler = r->EventHandler;
				context->magic = 0x907690;
				r->EventHandler = (PVOID)my_ClientEventReceive;
				r->EventContext = context;
			}
			if (r->EventType == TDI_EVENT_DISCONNECT&&r->EventHandler != NULL)  //服务器端断开连接时调用（移除File_object）
			{
				temp_context* context = (ptemp_context)ExAllocatePool(NonPagedPool, sizeof(temp_context));
				context->old_context = r->EventContext;
				context->old_handler = r->EventHandler;
				context->magic = 0x907690;
				r->EventContext = context;
				r->EventHandler = (PVOID)my_ClientEventDisconnect;
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
	return -1;
}
NTSTATUS my_ClientEventConnect(
	_In_  PVOID              TdiEventContext,
	_In_  LONG               RemoteAddressLength,
	_In_  PVOID              RemoteAddress,
	_In_  LONG               UserDataLength,
	_In_  PVOID              UserData,
	_In_  LONG               OptionsLength,
	_In_  PVOID              Options,
	_Out_ CONNECTION_CONTEXT *ConnectionContext,             //ring3：listen接收到了链接请求，准备accept
	_Out_ PIRP               *AcceptIrp                      //服务端处理回调函数（获取acceptirp）
	)
{
	DbgBreakPoint();
	NTSTATUS sta;
	temp_context* context = (temp_context*)TdiEventContext;
	ClientEventConnect func = (ClientEventConnect)context->old_handler;
	PVOID old_context = context->old_context;
	if (context->magic == 0x907690)
	{
		sta = func(old_context, RemoteAddressLength, RemoteAddress, UserDataLength, UserData, OptionsLength,
			Options, ConnectionContext, AcceptIrp);     //原函数会调用类似ring3层的accept获取远程主机的socket，在ring0层为acceptirp
		/*ExFreePool(TdiEventContext);*/
	}
	else
	{
		return STATUS_UNSUCCESSFUL;
	}
	//在这里截取acceptirp
	if (AcceptIrp != NULL)
	{
		if (*AcceptIrp != NULL)
		{
			PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(*AcceptIrp);
			insertportinfo(sa, ConnectionContext);
		}
	}
	return sta;
}
NTSTATUS my_ClientEventReceive(
	_In_  PVOID              TdiEventContext,
	_In_  CONNECTION_CONTEXT ConnectionContext,
	_In_  ULONG              ReceiveFlags,
	_In_  ULONG              BytesIndicated,
	_In_  ULONG              BytesAvailable,
	_Out_ ULONG              *BytesTaken,
	_In_  PVOID              Tsdu,                     //收到数据的指针
	_Out_ PIRP               *IoRequestPacket        //客户端处理回调函数（对收到的数据包进行过滤，转发）
	)
{
	DbgBreakPoint();
	temp_context* context = (temp_context*)TdiEventContext;
	ClientEventReceive func = (ClientEventReceive)context->old_handler;
	PVOID old_context = context->old_context;
	if (context->magic == 0x907690)
	{
		/*ExFreePool(TdiEventContext);*/
	}
	return func(old_context, ConnectionContext, ReceiveFlags, BytesIndicated, BytesAvailable, BytesTaken, Tsdu,
		IoRequestPacket);
}
NTSTATUS my_ClientEventDisconnect(
	_In_ PVOID              TdiEventContext,
	_In_ CONNECTION_CONTEXT ConnectionContext,
	_In_ LONG               DisconnectDataLength,
	_In_ PVOID              DisconnectData,
	_In_ LONG               DisconnectInformationLength,
	_In_ PVOID              DisconnectInformation,
	_In_ ULONG              DisconnectFlags
	)
{
	DbgBreakPoint();
	temp_context* context = (temp_context*)TdiEventContext;
	ClientEventDisconnect func = (ClientEventDisconnect)context->old_handler;
	PVOID old_context = context->old_context;
	deleteportinfo(ConnectionContext);
	if (context->magic == 0x907690)
	{
		/*ExFreePool(TdiEventContext);*/
		return func(old_context, ConnectionContext, DisconnectDataLength, DisconnectData, DisconnectInformationLength, DisconnectInformation, DisconnectFlags);
	}
	else
	{
		return STATUS_UNSUCCESSFUL;
	}
}
BOOLEAN insertportinfo(PIO_STACK_LOCATION sa, CONNECTION_CONTEXT *ConnectionContext)          //获取到的FILE_ONJECT插入全局表
{
	DbgBreakPoint();
	PFILE_OBJECT fo = sa->FileObject;
	if ((int)fo->FsContext2 != 0x2)
	{
		return FALSE;
	}
	for (int i = 0; i < PORT_MAX; i++)
	{
		/**if (globals.canuse[i] == FALSE)*/
		{
			globals.porttable[i] = fo;
			globals.canuse[i] = TRUE;
			globals.connectioncontext[i] = ConnectionContext;
			globals.count++;
		}
	}
	return TRUE;
}
int checkportinfo(CONNECTION_CONTEXT context)                                //返回有效的FILE_OBJECT
{
	for (int i = 0; i < PORT_MAX; i++)
	{
		if (globals.connectioncontext[i] == context)
		{
			return i;
		}
	}
	return -1;
}
VOID deleteportinfo(CONNECTION_CONTEXT ConnectionContext)                     //删除全局表指定的FILE_OBJECT
{
	for (int i = 1; i < PORT_MAX-1; i++)
	{
		if (globals.connectioncontext[i]==ConnectionContext)
		{
			globals.canuse[i] = FALSE;
			globals.count--;
		}
	}
}
NTSTATUS tdi_io_control(PDEVICE_OBJECT dev, PIRP irp)       //调用TDI_SEND从已经获取的FILE_OBJECT选择一个进行发包
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	DbgBreakPoint();
	NTSTATUS sta;
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	switch (sa->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_CONSEND:
	{
		/*PIRP irps = TdiBuildInternalDeviceControlIrp(TDI_SEND, tcpreal, globals.porttable[0], NULL, NULL);*/
		ctx context;
		KeInitializeEvent(&context.eve, NotificationEvent, FALSE);
		PIRP g_reuse_irp = IoAllocateIrp((dev->StackSize) + 1, FALSE);
		PVOID tai = ExAllocatePool(NonPagedPool, 100);
		memcpy(tai, "success", 100);
		PMDL mdl = IoAllocateMdl(tai, 50, FALSE, FALSE, g_reuse_irp);
		MmBuildMdlForNonPagedPool(mdl);
		__try
		{
			MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IoFreeMdl(mdl);
			IoFreeIrp(irp);
			sta = STATUS_UNSUCCESSFUL;
			return sta;
		}
		context.mdl = mdl;
		TdiBuildSend(g_reuse_irp, tcpreal, globals.porttable[0], tdi_send_complete, &context, mdl, 0, 50);
		sta=IoCallDriver(tcpreal, g_reuse_irp);
		if (sta == STATUS_PENDING)
		{
			// returned status pending
			KeWaitForSingleObject(&context.eve, Executive, KernelMode, FALSE, NULL);
		}
		return STATUS_SUCCESS;
	}
	default:
		break;
	}
	return STATUS_SUCCESS;
}
NTSTATUS tdi_send_complete(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp,
	_In_ PVOID Context
	)
{
	DbgBreakPoint();
	UNREFERENCED_PARAMETER(Irp);
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);
	PMDL mdl = NULL;
	PMDL nextMdl = NULL;
	pctx context = Context;
	if (Irp->MdlAddress != NULL)
	{
		for (mdl = Irp->MdlAddress; mdl != NULL; mdl = nextMdl)
		{
			nextMdl = mdl->Next;
			MmUnlockPages(mdl);

			// This function will also unmap pages.
			IoFreeMdl(mdl);
		}

		// set mdl address to null, to prevent iofreeirp to attempt to free it again
		Irp->MdlAddress = NULL;
	}
	KeSetEvent(&context->eve, IO_NO_INCREMENT, FALSE);
	return STATUS_MORE_PROCESSING_REQUIRED;
}