#pragma once
#include"struct.h"
#pragma warning(disable:4100)
#define GETCODE(X)                      X>>2  &  0xFFF
NTSTATUS DevCon(PDEVICE_OBJECT dev, PIRP irp)
{
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	PDEVICE_EXTENSION Ext = dev->DeviceExtension;
	irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
	DbgPrint("code:0x%x\n", GETCODE(sa->Parameters.DeviceIoControl.IoControlCode));
	switch (sa->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_DISK_GET_PARTITION_INFO:
	{
		PVOID output = irp->AssociatedIrp.SystemBuffer;
		PBOOT_SECTOR bootSector = (PBOOT_SECTOR)Ext->DiskImage;
		PPARTITION_INFORMATION Pi = output;
		Pi->PartitionType= (bootSector->bsFileSystemType[4] == '6') ? PARTITION_FAT_16 : PARTITION_FAT_12;
		Pi->BootIndicator = FALSE;
		Pi->RecognizedPartition = TRUE;
		Pi->RewritePartition = FALSE;
		Pi->StartingOffset.QuadPart = 0;
		Pi->PartitionLength.QuadPart = Ext->DiskRegInfo.DiskSize;
		Pi->HiddenSectors = (ULONG)(1L);
		Pi->PartitionNumber = (ULONG)(-1L);
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);
		break;
	}
	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
	{
		PVOID output = irp->AssociatedIrp.SystemBuffer;
		RtlCopyMemory(output, &Ext->DiskGeometry, sizeof(DISK_GEOMETRY));
		irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
		irp->IoStatus.Status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_DISK_GET_LENGTH_INFO:
	{
		PVOID output = irp->AssociatedIrp.SystemBuffer;
		PGET_LENGTH_INFORMATION Gli = output;
		Gli->Length.QuadPart = DEFAULT_DISK_SIZE;
		irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);
		irp->IoStatus.Status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_DISK_CHECK_VERIFY:
	case IOCTL_DISK_IS_WRITABLE:

		//
		// Return status success
		//

		irp->IoStatus.Status = STATUS_SUCCESS;
		break;
	default:
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
	}
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}