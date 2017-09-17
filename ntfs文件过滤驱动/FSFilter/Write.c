#include"Write.h"
//注意，不能在IRP_MJ_WRITE中调用ObQueryNameString，会锁死！！！！！！！！
extern VOID FindFileName(POBJECT_NAME_INFORMATION info, PIO_STACK_LOCATION sa);

NTSTATUS FsFilterDispatchWriteComplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(dev);
	UNREFERENCED_PARAMETER(context);
	PFILE_CONTEXT Ctx = (PFILE_CONTEXT)context;
	if (irp->PendingReturned == TRUE)            //有pending标记一定要mark
	{
		DEBUG_ENTER_FUNCTION("write pending");
		IoMarkIrpPending(irp);
		KeSetEvent(&Ctx->WaitEvent, IO_NO_INCREMENT, FALSE);
		return STATUS_PENDING;
	}
	KeSetEvent(&Ctx->WaitEvent, IO_NO_INCREMENT, FALSE);
	return STATUS_MORE_PROCESSING_REQUIRED;
}
NTSTATUS FsFilterDispatchWrite(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	NTSTATUS sta;
	UNREFERENCED_PARAMETER(DeviceObject);
	PFSFILTER_DEVICE_EXTENSION pDevExt = (PFSFILTER_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
	PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
	if (sa->MinorFunction == IRP_MN_NORMAL)
	{
		if (irp->MdlAddress)
		{
			DEBUG_ENTER_FUNCTION("write output buffer is in mdl");
		}
		else if (irp->AssociatedIrp.SystemBuffer)
		{
			DEBUG_ENTER_FUNCTION("write output buffer is in systembuffer");
		}
		else if (irp->UserBuffer)
		{
			DEBUG_ENTER_FUNCTION("write output buffer is in userbuffer");
		}
		FILE_CONTEXT Ctx = { 0 };
		KeInitializeEvent(&Ctx.WaitEvent, NotificationEvent, FALSE);
		POBJECT_NAME_INFORMATION info = NULL;
		DEBUG_ENTER_FUNCTION("write");
		FindFileName(info, sa);
		IoCopyCurrentIrpStackLocationToNext(irp);
		IoSetCompletionRoutine(irp, FsFilterDispatchWriteComplete, &Ctx, TRUE, TRUE, TRUE);
		sta = IoCallDriver(pDevExt->AttachedToDeviceObject, irp);
		if (sta == STATUS_PENDING)
		{
			DEBUG_ENTER_FUNCTION("write wait......");
			KeWaitForSingleObject(&Ctx.WaitEvent, Executive, KernelMode, FALSE, NULL);
			/*IoCompleteRequest(irp, IO_NO_INCREMENT);*/
			if (irp->IoStatus.Status == STATUS_SUCCESS)
			{
				if (irp->MdlAddress)
				{
					DEBUG_ENTER_FUNCTION("write OK:read output buffer is in mdl");
				}
				else if (irp->AssociatedIrp.SystemBuffer)
				{
					DEBUG_ENTER_FUNCTION("write OK:read output buffer is in systembuffer");
				}
				else if (irp->UserBuffer)
				{
					DEBUG_ENTER_FUNCTION("write OK:read output buffer is in userbuffer");
				}
			}
			DEBUG_ENTER_FUNCTION("pending write status:0x%x", irp->IoStatus.Status);
			return sta;
		}
		else
		{
			//完成函数返回STATUS_MORE_PROCESSING_REQUIRED时需要再次调用IoCompleteRequest
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			DEBUG_ENTER_FUNCTION("write status:0x%x", sta);
			return sta;
		}
	}
	else
	{
		//IRP_MN_MDL较难处理，放过再说
		DEBUG_ENTER_FUNCTION("The request is IRP_MN_MDL!");
		IoSkipCurrentIrpStackLocation(irp);
		return IoCallDriver(pDevExt->AttachedToDeviceObject, irp);
	}
}