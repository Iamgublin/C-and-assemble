#include<ntddk.h>
#include<wdmsec.h>
#define SEND_STR (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x911,METHOD_BUFFERED,FILE_WRITE_DATA)
#define REC_STR (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x912,METHOD_BUFFERED,FILE_READ_DATA)
PDEVICE_OBJECT dev = NULL;
PVOID buf1;
UNICODE_STRING buf;
NTSTATUS dispatch(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS DriverUnload(PDRIVER_OBJECT Driver)
{
	UNREFERENCED_PARAMETER(Driver);
	LONG status;
	IoDeleteDevice(dev);
	UNICODE_STRING str = RTL_CONSTANT_STRING(L"\\??\\asd_zlz");
	status=IoDeleteSymbolicLink(&str);
	DbgPrint("%d", status);
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	buf1 = ExAllocatePoolWithTag(NonPagedPool, 512, 1);
	static const GUID guid =
	{ 0x52a79f7c, 0x5a66, 0x4b3f, { 0xb6, 0x17, 0x3e, 0x24, 0x92, 0xd5, 0x76, 0x62 } };

	UNICODE_STRING name = RTL_CONSTANT_STRING(L"\\Device\\asd_zlz123");
	UNICODE_STRING syb = RTL_CONSTANT_STRING(L"\\??\\asd_zlz");
	UNICODE_STRING sddl = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;WD)");
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = dispatch;
	}
	LONG b=IoCreateDeviceSecure(driver, 0, &name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &sddl, &guid,&dev);
	LONG a=IoCreateSymbolicLink(&syb, &name);
	DbgPrint("%d", a);
	DbgPrint("%d", b);
	driver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
NTSTATUS dispatch(PDEVICE_OBJECT somedev, PIRP irp)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(irp);
	LONG ret_len = 0;
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	if (somedev == dev)
	{
		if (irpsp->MajorFunction == IRP_MJ_CREATE)
		{
			DbgPrint("create success!");
		}
		if (irpsp->MajorFunction == IRP_MJ_CLOSE)
		{
			DbgPrint("close success!");
		}
		if (irpsp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
		{
			PVOID temp = irp->AssociatedIrp.SystemBuffer;
			ULONG inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;
			ULONG outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
			case SEND_STR:
				ASSERT(inlen > 0);
				ASSERT(outlen == 0);
				RtlCopyMemory(buf1, temp, wcslen(temp) * 2 + 2);
				RtlInitUnicodeString(&buf, temp);
				DbgPrint("%wZ", &buf);
				break;
			case REC_STR:
				ret_len = 512;
				DbgPrint("%ws", buf1);
				irp->AssociatedIrp.SystemBuffer = ExAllocatePoolWithTag(NonPagedPool,512, 0);
				RtlCopyMemory(irp->AssociatedIrp.SystemBuffer,buf1, wcslen(buf1) * 2 + 2);
				break;
			default:
				status = STATUS_INVALID_PARAMETER;
				break;
			}
		}
	}
		irp->IoStatus.Information = 512;
		irp->IoStatus.Status = status;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}