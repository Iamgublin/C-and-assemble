#pragma once
#include"struct.h"
extern NTSTATUS ZlzRemoveListHead(PFILTER_CONTEXT Context);
VOID ZlzInitShowAdapterPacket(PIO_Packet IoPacket)
{
	IoPacket->Packet.ShowAdapter.Num = Global.contextnum;
	IoPacket->Type = PACKET_TYPE_ADAPTERINFO;
	for (int i = 0; i < Global.contextnum; i++)
	{
		RtlCopyMemory(IoPacket->Packet.ShowAdapter.AdapterInfo[i].DevName, Global.context[i]->DevInfo.DevName.Buffer, Global.context[i]->DevInfo.DevName.Length);
		RtlCopyMemory(IoPacket->Packet.ShowAdapter.AdapterInfo[i].DevPathName, Global.context[i]->DevInfo.DevPathName.Buffer, Global.context[i]->DevInfo.DevPathName.Length);
		RtlCopyMemory(IoPacket->Packet.ShowAdapter.AdapterInfo[i].MacAddress, Global.context[i]->DevInfo.MacAddress, 32);
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
NTSTATUS CopyNetBuffer(PIO_Packet Packet, int i)
{
	Packet->Type = PACKET_TYPE_NETPACKET;
	if (Global.context[i] == NULL)
	{
		return STATUS_UNSUCCESSFUL;
	}
	if (Global.context[i]->CurrentRecvNum != 0)
	{
		KIRQL irql;
		KeAcquireSpinLock(&Global.context[i]->NetBufferListLock, &irql);
		PS_PACKET PacketUsed = (PS_PACKET)Global.context[i]->PacketRecvList.Flink;
		PMDL MdlUsed = PacketUsed->mdllist[PacketUsed->MdlHasCopied];
		PVOID Buf = MmGetSystemAddressForMdlSafe(MdlUsed, IoPriorityNormal);
		int size = MmGetMdlByteCount(MdlUsed);
		Packet->Packet.Net_Packet_Output.Size = size;
		Packet->Packet.Net_Packet_Output.IsSendPacket = PacketUsed->IsSendPacket;
		RtlZeroMemory(Packet->Packet.Net_Packet_Output.Buffer, sizeof(Packet->Packet.Net_Packet_Output.Buffer));
		RtlCopyMemory(Packet->Packet.Net_Packet_Output.Buffer, Buf, size);
		PacketUsed->MdlHasCopied++;
		if (PacketUsed->MdlHasCopied == PacketUsed->MdlNumber)
		{
			ZlzRemoveListHead(Global.context[i]);
			Packet->Packet.Net_Packet_Output.Isthelast = TRUE;
		}
		else
		{
			Packet->Packet.Net_Packet_Output.Isthelast = FALSE;
		}
		KeReleaseSpinLock(&Global.context[i]->NetBufferListLock, irql);
	}
	else
	{
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}
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
	if (dev == Global.FilterDev)
	{
		PIO_Packet Packet = (PIO_Packet)ExAllocatePool(NonPagedPool, sizeof(IO_Packet));
		if (Packet == NULL)
		{
			return STATUS_UNSUCCESSFUL;
		}
		NdisZeroMemory(Packet, sizeof(IO_Packet));
		PIO_STACK_LOCATION sa = IoGetCurrentIrpStackLocation(irp);
		PVOID buffer = irp->AssociatedIrp.SystemBuffer;
		switch (sa->Parameters.DeviceIoControl.IoControlCode)
		{
		case IOCTL_SHOWADAPTER:
			irp->IoStatus.Information = sizeof(IO_Packet);
			irp->IoStatus.Status = STATUS_SUCCESS;
			ZlzInitShowAdapterPacket(Packet);
			RtlCopyMemory(buffer, Packet, sizeof(IO_Packet));
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			break;
		case IOCTL_GETRAWDATA:
			irp->IoStatus.Information = sizeof(IO_Packet);
			irp->IoStatus.Status = STATUS_SUCCESS;
			if (sa->Parameters.DeviceIoControl.InputBufferLength)
			{
				PIO_Packet PacketInput = irp->AssociatedIrp.SystemBuffer;
				int ContextNum = PacketInput->Packet.Net_Packet_InPut.ContextNum;
				irp->IoStatus.Status = CopyNetBuffer(Packet, ContextNum);
				RtlCopyMemory(buffer, Packet, sizeof(IO_Packet));
			}
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