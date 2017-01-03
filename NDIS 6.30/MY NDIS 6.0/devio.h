#pragma once
#include"struct.h"
extern NTSTATUS ZlzCleanPool(PFILTER_CONTEXT Context);
VOID ZlzInitShowAdapterPacket(PIO_Packet IoPacket)
{
	IoPacket->Packet.ShowAdapter.Num = Global.contextnum;
	IoPacket->Type = PACKET_TYPE_ADAPTERINFO;
	for (int i = 0; i < Global.contextnum; i++)
	{
		RtlCopyMemory(IoPacket->Packet.ShowAdapter.AdapterInfo[i].DevName, Global.context[i]->DevName.Buffer, Global.context[i]->DevName.Length);
		RtlCopyMemory(IoPacket->Packet.ShowAdapter.AdapterInfo[i].DevPathName, Global.context[i]->DevPathName.Buffer, Global.context[i]->DevPathName.Length);
	}
}
/*VOID CopyNetBuffer(PIO_Packet Packet)
{
	int offset = 0;
	Packet->Type = PACKET_TYPE_NETPACKET;
	for (int i = 0; i < Global.contextnum; i++)
	{
		if (Global.context[i]->CurrentRecvNum != 0)
		{
			KIRQL irql;
			KeAcquireSpinLock(&Global.context[i]->NetBufferPoolLock, &irql);
			Packet->Packet.Net_Packet.Num = Global.context[i]->CurrentRecvNum;
			for (int a = 0; a < Global.context[i]->CurrentRecvNum; a++)
			{
				int mdlnum = Global.context[i]->PacketRecvPool[a]->MdlNumber;
				for (int temp = 0; temp < mdlnum; temp++)
				{
					PVOID Buf = MmGetSystemAddressForMdlSafe(Global.context[i]->PacketRecvPool[a]->mdllist[temp], IoPriorityNormal);
					int size = MmGetMdlByteCount(Global.context[i]->PacketRecvPool[a]->mdllist[temp]);
					if (offset + size > 2000)
					{
						break;
					}
					RtlCopyMemory(Packet->Packet.Net_Packet.Buffer[0]+offset, Buf, size);
					offset += size;
				}
				offset = 0;
			}
			ZlzCleanPool(Global.context[i]);
			KeReleaseSpinLock(&Global.context[i]->NetBufferPoolLock, irql);
		}
	}
}*/
NTSTATUS Create(PDEVICE_OBJECT dev, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS Close(PDEVICE_OBJECT dev, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS CleanUp(PDEVICE_OBJECT dev, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS MyDeviceIoControl(PDEVICE_OBJECT dev, PIRP irp)
{
	DbgBreakPoint();
	if (dev == Global.FilterDev)
	{
		PIO_Packet Packet = (PIO_Packet)ExAllocatePool(NonPagedPool, sizeof(IO_Packet));
		if (Packet == NULL)
		{
			return STATUS_UNSUCCESSFUL;
		}
		PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
		switch (sa->Parameters.DeviceIoControl.IoControlCode)
		{
		case IOCTL_SHOWADAPTER:
			irp->IoStatus.Information = sizeof(IO_Packet);
			irp->IoStatus.Status = STATUS_SUCCESS;
			ZlzInitShowAdapterPacket(Packet);
			PVOID buffer = irp->AssociatedIrp.SystemBuffer;
			RtlCopyMemory(buffer, Packet, sizeof(IO_Packet));
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			break;
		case IOCTL_GETRAWDATA:
			/*CopyNetBuffer(Packet);*/
			PVOID buffer1 = irp->AssociatedIrp.SystemBuffer;
			RtlCopyMemory(buffer1, Packet, sizeof(IO_Packet));
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			break;
		default:
			break;
		}
		ExFreePool(Packet);
		return STATUS_SUCCESS;
	}
	else
	{
		return devcon(dev, irp);
	}
}