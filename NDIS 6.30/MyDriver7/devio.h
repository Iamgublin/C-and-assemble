#pragma once
#include"struct.h"
NTSTATUS MyDeviceIoControl(PDEVICE_OBJECT dev, PIRP irp)
{
	if (dev == Global.FilterDev)
	{
		PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
		switch (sa->Parameters.DeviceIoControl.IoControlCode)
		{
		default:
			break;
		}
		return STATUS_SUCCESS;
	}
	else
	{
		return devcon(dev, irp);
	}
}