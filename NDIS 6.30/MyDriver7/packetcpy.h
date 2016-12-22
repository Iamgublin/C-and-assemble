#pragma once
#include"struct.h"
/*int ZlzCalcBufSizeOrCopy(PVOID DestBuf, PNET_BUFFER_LIST Nbl, int num)   //调用两次该函数，第一次获取大小，分配内存，第二次拷贝。
{
	PNET_BUFFER_LIST nbltemp = Nbl;
	PNET_BUFFER nb = NULL;
	int size = 0;
	char *buf = (char*)DestBuf;
	do
	{
		nb = NET_BUFFER_LIST_FIRST_NB(nbltemp);
		do
		{
			if (nb != NULL)
			{
				for (PMDL mdl = NET_BUFFER_FIRST_MDL(nb); mdl != NULL; mdl=mdl->Next)
				{
					
					int mdlsize = MmGetMdlByteCount(mdl);
					size += mdlsize;
					if (num)
					{
						PVOID mdladdress = MmGetSystemAddressForMdlSafe(mdl, IoPriorityNormal);
						memcpy(buf, mdladdress, mdlsize);
						buf += mdlsize;
					}
				}
			}
			nb = NET_BUFFER_NEXT_NB(nb);
		} while (nb != NULL);
		nbltemp = NET_BUFFER_LIST_NEXT_NBL(nbltemp);
	} while (nbltemp != NULL);
	return size;
}*/
NTSTATUS ZlzCleanPool(PFILTER_CONTEXT Context)
{
	for (int num = 0; num < Context->CurrentRecvNum; num++)
	{
		if (Context->PacketRecvPool[num] != NULL)
		{
			if (Context->PacketRecvPool[num]->buffer != NULL)
			{
				NdisFreeCloneNetBufferList(Context->PacketRecvPool[num]->buffer, 0);
				ExFreePool(Context->PacketRecvPool[num]);
			}
		}
	}
	NdisZeroMemory(Context->PacketRecvPool, sizeof(Context->PacketRecvPool));
	Context->CurrentRecvNum = 0;
	return STATUS_SUCCESS;
}
NTSTATUS ZlzInsertIntoPool(PS_PACKET Packet, PFILTER_CONTEXT Context)
{
	KIRQL irql;
	if (Context->CurrentRecvNum >= RECV_POOL_MAX)
	{
		ZlzCleanPool(Context);
	}
	KeAcquireSpinLock(&Context->NetBufferPoolLock, &irql);
	Context->PacketRecvPool[Context->CurrentRecvNum] = Packet;
	Context->CurrentRecvNum++;
	KeReleaseSpinLock(&Context->NetBufferPoolLock, irql);
	return STATUS_SUCCESS;
}
NTSTATUS ZlzCopyNdlToBufferAndInsert(PFILTER_CONTEXT Context, PNET_BUFFER_LIST Nbl)
{
	PNET_BUFFER_LIST CloneNbl=NdisAllocateCloneNetBufferList(Nbl, Context->NetBufferPool, NULL, 0);
	PS_PACKET Packet=(PS_PACKET)ExAllocatePool(NonPagedPool, sizeof(S_PACKET));
	Packet->buffer = CloneNbl;
	Packet->size = 0;
	ZlzInsertIntoPool(Packet, Context);
	return STATUS_SUCCESS;
}
