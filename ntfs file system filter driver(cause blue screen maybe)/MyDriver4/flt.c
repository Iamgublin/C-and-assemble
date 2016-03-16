#include<ntifs.h>
#include<ntddk.h>
int status;
typedef struct _ZLZ_EXT
{
	PDEVICE_OBJECT attobj;
	PDEVICE_OBJECT lowerobj;
}ZLZ_EXT, *PZLZ_EXT;
PDRIVER_OBJECT dri;
PDEVICE_OBJECT fildev;
PDEVICE_OBJECT volumedev;
PDEVICE_OBJECT attdev;
PFAST_IO_DISPATCH fd;
VOID unload(PDRIVER_OBJECT driver);
NTSTATUS read(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS write(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS create(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS dispatch(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS attachvolume(PDRIVER_OBJECT driver);
NTSTATUS writecomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
NTSTATUS readcomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
NTSTATUS createcomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
NTSTATUS noti(PDEVICE_OBJECT dev, BOOLEAN registe);
NTSTATUS filesyscon(PDEVICE_OBJECT dev, PIRP irp);
VOID unload(PDRIVER_OBJECT driver)
{
	IoDetachDevice(attdev);
	IoDeleteDevice(driver->DeviceObject);
	UNREFERENCED_PARAMETER(driver);
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	dri = driver;
	for (int a = 0; a < IRP_MJ_MAXIMUM_FUNCTION; a++)
	{
		driver->MajorFunction[a] = dispatch;
	}
	IoRegisterFsRegistrationChange(driver, noti);
	driver->MajorFunction[IRP_MJ_READ] = read;
	driver->MajorFunction[IRP_MJ_CREATE] = create;
	driver->MajorFunction[IRP_MJ_WRITE] = write;
	driver->MajorFunction[IRP_MJ_FILE_SYSTEM_CONTROL] = filesyscon;
	driver->DriverUnload = unload;
	driver->FastIoDispatch = fd;
	attachvolume(driver);
	return STATUS_SUCCESS;
}
NTSTATUS read(PDEVICE_OBJECT dev, PIRP irp)
{
	__try
	{
		PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
		if (sa->MinorFunction == IRP_MN_MDL)
		{
			DbgPrint("MDL!\n");
		}
		if (sa->MinorFunction == IRP_MN_NORMAL)
		{

			NTSTATUS sta;
			UNREFERENCED_PARAMETER(dev);
			ULONG len;
			POBJECT_NAME_INFORMATION info;
			DbgPrint("read\n");
			sta = ObQueryNameString(sa->DeviceObject, NULL, 0, &len);
			info = (POBJECT_NAME_INFORMATION)ExAllocatePool(NonPagedPool, len);
			sta = ObQueryNameString(sa->DeviceObject, info, len, &len);
			DbgPrint("the read device is:%wZ\n", info->Name);
			ExFreePool(info);
			if (dev == fildev)
			{
				DbgPrint("read return\n");
				irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
				irp->IoStatus.Information = 0;
				IoCompleteRequest(irp, IO_NO_INCREMENT);
				return irp->IoStatus.Status;
			}
			IoCopyCurrentIrpStackLocationToNext(irp);
			IoSetCompletionRoutine(irp, readcomplete, 0, TRUE, TRUE, TRUE);
			return IoCallDriver(attdev, irp);
		}
		else
		{
			DbgPrint("error!\n");
			return STATUS_UNSUCCESSFUL;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_UNSUCCESSFUL;
	}
}
NTSTATUS attachvolume(PDRIVER_OBJECT driver)
{
	NTSTATUS sta;
	UNREFERENCED_PARAMETER(driver);
	POBJECT_NAME_INFORMATION info;
	ULONG len;
	PFILE_OBJECT file;
	PDEVICE_OBJECT device1 = 0;
	PDEVICE_OBJECT targetdevice = 0;
	PDEVICE_OBJECT targetdevice1 = 0;
	PDEVICE_OBJECT dev;
	UNICODE_STRING name = RTL_CONSTANT_STRING(L"\\Device\\HarddiskVolume1");
	UNICODE_STRING name1 = RTL_CONSTANT_STRING(L"\\FileSystem\\filters\\zlzguolv");
	sta = IoGetDeviceObjectPointer(&name, SYNCHRONIZE, &file, &device1);
	DbgPrint("0x%x\n", sta);
	DbgPrint("0x%x\n", device1);
	if (NT_SUCCESS(sta))
	{
		ObDereferenceObject(file);
		sta = IoCreateDevice(driver, 0, &name1, device1->DeviceType, 0, FALSE, &dev);
		DbgPrint("10x%x\n", sta);
		if (!NT_SUCCESS(sta))
		{
			status = 1;
			return STATUS_UNSUCCESSFUL;
		}
		dev->Flags &= ~DO_DEVICE_INITIALIZING;
		dev->Flags |= DO_DIRECT_IO;
		dev->Characteristics = FILE_DEVICE_SECURE_OPEN;
		targetdevice = IoGetDeviceAttachmentBaseRef(device1);
		ObDereferenceObject(device1);
		if (targetdevice != NULL)
		{
			sta = ObQueryNameString(targetdevice, NULL, 0, &len);
			info = (POBJECT_NAME_INFORMATION)ExAllocatePool(NonPagedPool, len);
			sta = ObQueryNameString(targetdevice, info, len, &len);
			if (info != NULL)
			{
				DbgPrint("%wZ\n", info->Name);
				ExFreePool(info);
			}
			targetdevice1 = targetdevice->Vpb->DeviceObject;
			if (targetdevice1 != NULL)
			{
				sta = IoAttachDeviceToDeviceStackSafe(dev, targetdevice1, &attdev);
				volumedev = dev;
				DbgPrint("20x%x\n", sta);
				if (!NT_SUCCESS(sta))
				{
					status = 2;
					return STATUS_SUCCESS;
				}
			}
			else
			{
				DbgPrint("error!\n");
			}
			ObReferenceObject(targetdevice);
		}
	}
	return STATUS_SUCCESS;
}
NTSTATUS dispatch(PDEVICE_OBJECT dev, PIRP irp)
{
	__try
	{
		NTSTATUS sta;
		UNREFERENCED_PARAMETER(dev);
		DbgPrint("dispatch\n");
		if (dev == fildev)
		{
			DbgPrint("dispatch return\n");
			irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			irp->IoStatus.Information = 0;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return irp->IoStatus.Status;
		}
		IoSkipCurrentIrpStackLocation(irp);
		sta = IoCallDriver(attdev, irp);
		DbgPrint("dispatch status:0x%x\n", sta);
		return sta;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_UNSUCCESSFUL;
	}
}
NTSTATUS create(PDEVICE_OBJECT dev, PIRP irp)
{
	__try{
		UNREFERENCED_PARAMETER(dev);
		DbgPrint("create\n");
		if (dev == fildev)
		{
			DbgPrint("creat return\n");
			irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			irp->IoStatus.Information = 0;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return irp->IoStatus.Status;
		}
		IoCopyCurrentIrpStackLocationToNext(irp);
		IoSetCompletionRoutine(irp, createcomplete, 0, TRUE, TRUE, TRUE);
		return IoCallDriver(attdev, irp);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_UNSUCCESSFUL;
	}
}
NTSTATUS write(PDEVICE_OBJECT dev, PIRP irp)
{
	__try
	{
		NTSTATUS sta;
		UNREFERENCED_PARAMETER(dev);
		PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
		DbgPrint("write\n");
		if (dev == fildev)
		{
			DbgPrint("write return\n");
			DbgPrint("0x%x", sa->MinorFunction);
			irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			irp->IoStatus.Information = 0;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return irp->IoStatus.Status;
		}
		ULONG len;
		POBJECT_NAME_INFORMATION info;
		sta = ObQueryNameString(sa->DeviceObject, NULL, 0, &len);
		info = (POBJECT_NAME_INFORMATION)ExAllocatePool(NonPagedPool, len);
		sta = ObQueryNameString(sa->DeviceObject, info, len, &len);
		DbgPrint("the write device is:%wZ\n", info->Name);
		ExFreePool(info);
		IoCopyCurrentIrpStackLocationToNext(irp);
		IoSetCompletionRoutine(irp, writecomplete, 0, TRUE, TRUE, TRUE);
		return IoCallDriver(attdev, irp);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("error!\n");
		return STATUS_UNSUCCESSFUL;
	}
}
NTSTATUS writecomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(context);
	ULONG len;
	NTSTATUS sta;
	POBJECT_NAME_INFORMATION info;
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	sta = ObQueryNameString(sa->FileObject, NULL, 0, &len);
	info = (POBJECT_NAME_INFORMATION)ExAllocatePool(NonPagedPool, len);
	if (info != NULL)
	{
		sta = ObQueryNameString(sa->FileObject, info, len, &len);
		DbgPrint("the write file directory is:%wZ\n", info->Name);
		ExFreePool(info);
	}
	if (irp->PendingReturned == TRUE)
	{
		DbgPrint("pending\n");
		IoMarkIrpPending(irp);
	}
	DbgPrint("write status:0x%x\n", irp->IoStatus.Status);
	return irp->IoStatus.Status;
}
NTSTATUS readcomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(context);
	ULONG len;
	NTSTATUS sta;
	POBJECT_NAME_INFORMATION info;
	DbgPrint("1\n");
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	sta = ObQueryNameString(sa->FileObject, NULL, 0, &len);
	info = (POBJECT_NAME_INFORMATION)ExAllocatePool(NonPagedPool, len);
	if (info != NULL)
	{
		DbgPrint("2\n");
		sta = ObQueryNameString(sa->FileObject, info, len, &len);
		DbgPrint("the read file directory is:%wZ\n", info->Name);
		ExFreePool(info);
	}
	if (irp->PendingReturned == TRUE)
	{
		DbgPrint("pending\n");
		IoMarkIrpPending(irp);
	}
	DbgPrint("read status:0x%x\n", irp->IoStatus.Status);
	return irp->IoStatus.Status;
}
NTSTATUS createcomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(context);
	DbgPrint("1\n");
	ULONG len;
	NTSTATUS sta;
	POBJECT_NAME_INFORMATION info;
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	sta = ObQueryNameString(sa->FileObject, NULL, 0, &len);
	info = (POBJECT_NAME_INFORMATION)ExAllocatePool(NonPagedPool, len);
	if (info != NULL)
	{
		DbgPrint("2\n");
		sta = ObQueryNameString(sa->FileObject, info, len, &len);
		DbgPrint("the create file directory is:%wZ\n", info->Name);
		ExFreePool(info);
	}
	if (irp->PendingReturned == TRUE)
	{
		DbgPrint("pending\n");
		IoMarkIrpPending(irp);
	}
	DbgPrint("create status:0x%x\n", irp->IoStatus.Status);
	return irp->IoStatus.Status;
}
NTSTATUS noti(PDEVICE_OBJECT dev, BOOLEAN registe)
{
	ULONG len;
	NTSTATUS sta;
	POBJECT_NAME_INFORMATION info;
	DbgPrint("enter noti\n");
	if (registe)
	{
		UNICODE_STRING A1 = RTL_CONSTANT_STRING(L"\\FileSystem\\Fs_Rec");
		UNICODE_STRING A2 = RTL_CONSTANT_STRING(L"\\FileSystem\\Mup");
		sta = ObQueryNameString(dev, NULL, 0, &len);
		info = ExAllocatePool(NonPagedPool, len);
		sta = ObQueryNameString(dev, info, len, &len);
		DbgPrint("the dev is on use:%wZ\n", info->Name);
		ExFreePool(info);
		DbgPrint("the type:0x%x\n", dev->Type);
		ObQueryNameString(dev->DriverObject, NULL, 0, &len);
		info = ExAllocatePool(NonPagedPool, len);
		ObQueryNameString(dev->DriverObject, info, len, &len);
		DbgPrint("the driver name:%wZ\n", info->Name);
		if ((RtlCompareUnicodeString(&(info->Name), &A1, TRUE) == 0) || (RtlCompareUnicodeString(&(info->Name), &A2, TRUE) == 0))
		{
			DbgPrint("attach passed!\n");
			ExFreePool(info);
			return STATUS_ABANDONED;
		}
		else
		{
			if (info != NULL)
			{
				ExFreePool(info);
			}
			UNICODE_STRING name = RTL_CONSTANT_STRING(L"\\FileSystem\\filters\\zlzguolvsyscon");
			IoCreateDevice(dri, sizeof(ZLZ_EXT), &name, dev->DeviceType, 0, FALSE, &fildev);
			PZLZ_EXT ext = fildev->DeviceExtension;
			fildev->Flags &= ~DO_DEVICE_INITIALIZING;

			if (FlagOn(dev->Flags, DO_BUFFERED_IO)) {
				SetFlag(fildev->Flags, DO_BUFFERED_IO);
			}
			if (FlagOn(dev->Flags, DO_DIRECT_IO)) {
				SetFlag(fildev->Flags, DO_DIRECT_IO);
			}
			fildev->Characteristics = FILE_DEVICE_SECURE_OPEN;
			sta = IoAttachDeviceToDeviceStackSafe(fildev, dev, &(ext->attobj));
			ext->lowerobj=dev;
			DbgPrint("0x%x\n", ext->attobj);
			DbgPrint("0x%x\n", dev);
			sta = ObQueryNameString(ext->attobj->DriverObject, NULL, 0, &len);
			info = ExAllocatePool(NonPagedPool, len);
			sta = ObQueryNameString(ext->attobj->DriverObject, info, len, &len);
			if (info != NULL)
			{
				DbgPrint("%wZ\n", info->Name);
				ExFreePool(info);
			}
		}
	}
	return STATUS_SUCCESS;
}
NTSTATUS filesyscon(PDEVICE_OBJECT dev, PIRP irp)
{
	UNREFERENCED_PARAMETER(dev);
	NTSTATUS sta;
	DbgPrint("enter filesyscon\n");
	PZLZ_EXT ext = fildev->DeviceExtension;
	IoSkipCurrentIrpStackLocation(irp);
	sta = IoCallDriver(ext->attobj, irp);
	DbgPrint("syscon status:0x%x\n", sta);
	return sta;
}