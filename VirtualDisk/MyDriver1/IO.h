#pragma once
NTSTATUS VirtualRead(PDEVICE_OBJECT dev, PIRP irp)
{
	int Length;
	LARGE_INTEGER ByteOffset;
	PVOID output = NULL;
	PDEVICE_EXTENSION Ext = dev->DeviceExtension;
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	if (irp->MdlAddress)
	{
		output = MmGetSystemAddressForMdlSafe(irp->MdlAddress, IoPriorityNormal);
	}
	else if(irp->AssociatedIrp.SystemBuffer)
	{
		output = irp->AssociatedIrp.SystemBuffer;
	}
	else
	{
		return STATUS_UNSUCCESSFUL;
	}
	ByteOffset = sa->Parameters.Read.ByteOffset;
	Length = sa->Parameters.Read.Length;
	RtlCopyMemory(output, Ext->DiskImage + ByteOffset.LowPart, Length);
	irp->IoStatus.Information = Length;
	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}
NTSTATUS VirtualWrite(PDEVICE_OBJECT dev, PIRP irp)
{
	int Length;
	LARGE_INTEGER ByteOffset;
	PVOID output = NULL;
	PDEVICE_EXTENSION Ext = dev->DeviceExtension;
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	if (irp->MdlAddress)
	{
		output = MmGetSystemAddressForMdlSafe(irp->MdlAddress, IoPriorityNormal);
	}
	else if (irp->AssociatedIrp.SystemBuffer)
	{
		output = irp->AssociatedIrp.SystemBuffer;
	}
	else
	{
		return STATUS_UNSUCCESSFUL;
	}
	ByteOffset = sa->Parameters.Write.ByteOffset;
	Length = sa->Parameters.Write.Length;
	RtlCopyMemory(Ext->DiskImage + ByteOffset.LowPart, output, Length);
	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}