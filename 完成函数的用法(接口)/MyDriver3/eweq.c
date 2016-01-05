#include<ntddk.h>
PDEVICE_OBJECT dev1 = NULL;
PDEVICE_OBJECT dev2 = NULL;
NTSTATUS devcreate(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS write(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS unload(PDRIVER_OBJECT driver);
NTSTATUS complete(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
VOID chuli(PVOID context);
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	if (dev1 != NULL)
	{
		IoDeleteDevice(dev1);
	}
	if (dev2 != NULL)
	{
		IoDeleteDevice(dev2);
	}
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(str);
	NTSTATUS status;
	UNICODE_STRING name1 = RTL_CONSTANT_STRING(L"\\Device\\asd_zlz1");
	UNICODE_STRING syb1 = RTL_CONSTANT_STRING(L"\\??\\asd_zlz1");
	UNICODE_STRING name2 = RTL_CONSTANT_STRING(L"\\Device\\asd_zlz2");
	UNICODE_STRING syb2 = RTL_CONSTANT_STRING(L"\\??\\asd_zlz2");
	driver->MajorFunction[IRP_MJ_CREATE] = devcreate;
	driver->MajorFunction[IRP_MJ_WRITE] = write;
	driver->DriverUnload = unload;
	status=IoCreateDevice(driver, 0, &name1, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev1);
	DbgPrint("0x%x\n", status);
	status=IoCreateDevice(driver, 0, &name2, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev2);
	DbgPrint("0x%x\n", status);
	IoCreateSymbolicLink(&syb1, &name1);
	IoCreateSymbolicLink(&syb2, &name2);
	return STATUS_SUCCESS;
}
NTSTATUS devcreate(PDEVICE_OBJECT dev, PIRP irp)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	PIO_STACK_LOCATION sa;
	sa = IoGetCurrentIrpStackLocation(irp);
	if (dev == dev1)
	{
		KEVENT Event;
		LARGE_INTEGER off;
		IO_STATUS_BLOCK bl;
		PIRP newirp;
		NTSTATUS status;
		DbgPrint("dev1 create\n");
		newirp = IoBuildAsynchronousFsdRequest(IRP_MJ_WRITE, dev2, NULL, 0, &off, &bl);
		KeInitializeEvent(&Event, NotificationEvent, FALSE);
		IoSetCompletionRoutine(newirp, complete, &Event, TRUE, TRUE, TRUE);
		status=IoCallDriver(dev2, newirp);   
		if (status == STATUS_PENDING)
		{
			DbgPrint("was waiting...\n");
			KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
		}
		DbgPrint("end\n");
		IofCompleteRequest(newirp, IO_NO_INCREMENT);
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
    }
	if (dev == dev2)
	{
		DbgPrint("dev2 create\n");
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	return STATUS_SUCCESS;
}
NTSTATUS complete(PDEVICE_OBJECT dev, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	UNREFERENCED_PARAMETER(context);
	DbgPrint("complete\n");
	KeSetEvent((PKEVENT)context, IO_NO_INCREMENT, FALSE);
	return STATUS_MORE_PROCESSING_REQUIRED;
}
NTSTATUS write(PDEVICE_OBJECT dev, PIRP irp)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	HANDLE thr;
	if (dev == dev2)
	{
		DbgPrint("receive access\n");
	}
	IoMarkIrpPending(irp);
	PsCreateSystemThread(&thr, 0, NULL, NULL, 0, chuli, irp);
	ZwClose(thr);
	return STATUS_PENDING;
}
VOID chuli(PVOID context)
{
	PIRP irp = (PIRP)context;
	DbgPrint("enter thread\n");
	IoCompleteRequest(irp,IO_NO_INCREMENT);
	PsTerminateSystemThread(STATUS_SUCCESS);
}
