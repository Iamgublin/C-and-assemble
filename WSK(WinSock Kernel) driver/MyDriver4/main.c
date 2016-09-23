#include<wdm.h>
#include<wsk.h>
#pragma warning(disable:4100)
WSK_CLIENT_DISPATCH clientdispatch = {
	MAKE_WSK_VERSION(1, 0), 0, NULL
};
WSK_REGISTRATION wskregistor = { 0, 0, 0 };
typedef struct _Global
{
	WSK_CLIENT_NPI clientnpi;
	WSK_PROVIDER_NPI providernpi;
	WSK_SOCKET rawsocket;
}Global,*pGlobal;
Global global;
VOID unload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	WskReleaseProviderNPI(&wskregistor);
	if (wskregistor.ReservedRegistrationLock)
	{
		WskDeregister(&wskregistor);
	}
}
NTSTATUS ZlzInitRawSocket(WSK_PROVIDER_NPI providernpi, PWSK_SOCKET rawsocket);
NTSTATUS CreateRawSocketComplete(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);
NTSTATUS CreateBasicSocketComplete(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);
NTSTATUS ZlzSetSockOpt(WSK_PROVIDER_NPI providernpi, PWSK_SOCKET rawsocket);
NTSTATUS ZlzReceiveFromEvent(
	_In_opt_ PVOID                    SocketContext,
	_In_     ULONG                    Flags,
	_In_opt_ PWSK_DATAGRAM_INDICATION DataIndication
	)
{
	DbgBreakPoint();
	DbgPrint("recv udp\n");
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	NTSTATUS sta;
	WSK_CLIENT_NPI clientnpi = { 0 };
	WSK_PROVIDER_NPI providernpi = { 0 };
	WSK_SOCKET rawsocket = { 0 };
	driver->DriverUnload = unload;
	clientnpi.ClientContext = NULL;
	clientnpi.Dispatch = &clientdispatch;
	sta = WskRegister(&clientnpi, &wskregistor);
	if (!NT_SUCCESS(sta))
	{
		return STATUS_UNSUCCESSFUL;
	}
	WSK_PROVIDER_CHARACTERISTICS charac;
	WskQueryProviderCharacteristics(&wskregistor, &charac);
	DbgPrint("accept version low:%s,%s\n", WSK_MAJOR_VERSION(charac.LowestVersion), WSK_MINOR_VERSION(charac.LowestVersion));
	DbgPrint("accept version high:%s,%s\n", WSK_MAJOR_VERSION(charac.HighestVersion), WSK_MINOR_VERSION(charac.HighestVersion));
	sta = WskCaptureProviderNPI(&wskregistor, WSK_INFINITE_WAIT, &providernpi);   //proviernpi包含wsk client句柄和一组通用socket函数
	if (!NT_SUCCESS(sta))
	{
		WskDeregister(&wskregistor);
	}
	sta = ZlzInitRawSocket(providernpi, &rawsocket);
	if (!NT_SUCCESS(sta))
	{
		WskReleaseProviderNPI(&wskregistor);
		WskDeregister(&wskregistor);
		return STATUS_UNSUCCESSFUL;
	}
	ZlzSetSockOpt(providernpi, &rawsocket);
	return STATUS_SUCCESS;
}
NTSTATUS ZlzInitRawSocket(WSK_PROVIDER_NPI providernpi,PWSK_SOCKET rawsocket)
{
	PIRP irp;
	NTSTATUS sta;
	irp = IoAllocateIrp(1, FALSE);
	if (!irp)
	{
		return STATUS_UNSUCCESSFUL;
	}
	WSK_CLIENT_DATAGRAM_DISPATCH datagramdispatch;
	datagramdispatch.WskReceiveFromEvent = ZlzReceiveFromEvent;
	IoSetCompletionRoutine(
		irp,
		CreateRawSocketComplete,
		(PVOID)&rawsocket,
		TRUE,
		TRUE,
		TRUE
		);
	sta = providernpi.Dispatch->WskSocket(providernpi.Client, AF_INET, SOCK_RAW, IPPROTO_UDP, WSK_FLAG_DATAGRAM_SOCKET, (PVOID)&rawsocket, &datagramdispatch,
		NULL, NULL, NULL, irp);
	if (NT_SUCCESS(sta))
	{
		return STATUS_SUCCESS;
	}
	return STATUS_UNSUCCESSFUL;
}
NTSTATUS
CreateRawSocketComplete(
PDEVICE_OBJECT DeviceObject,
PIRP Irp,
PVOID Context
)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	PWSK_SOCKET rawsocket = (PWSK_SOCKET)Context;

	// Check the result of the socket creation
	if (Irp->IoStatus.Status == STATUS_SUCCESS)
	{
		// Save the socket object for the new socket
		rawsocket = (PWSK_SOCKET)(Irp->IoStatus.Information);

		// Set any socket options for the new socket(using WskControlSocket)
		
		PWSK_PROVIDER_DATAGRAM_DISPATCH rawdispatch = (PWSK_PROVIDER_DATAGRAM_DISPATCH)rawsocket->Dispatch;
		UNREFERENCED_PARAMETER(rawdispatch);
		
		
		// Enable any event callback functions on the new socket
	}

	// Free the IRP
	IoFreeIrp(Irp);

	// Always return STATUS_MORE_PROCESSING_REQUIRED to
	// terminate the completion processing of the IRP.
	return STATUS_MORE_PROCESSING_REQUIRED;
}
NTSTATUS ZlzSetSockOpt(WSK_PROVIDER_NPI providernpi, PWSK_SOCKET rawsocket)
{
	DbgBreakPoint();
	PWSK_PROVIDER_BASIC_DISPATCH rawdispatch = (PWSK_PROVIDER_BASIC_DISPATCH)rawsocket->Dispatch;
	UNREFERENCED_PARAMETER(rawdispatch);
	/*basicdispatch->WskControlSocket(rawsocket, WskSetOption, IP_HDRINCL, IPPROTO_IP, 0, NULL, 0, NULL, NULL, NULL);*/
	return STATUS_SUCCESS;
}