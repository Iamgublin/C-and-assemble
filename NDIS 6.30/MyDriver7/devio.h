#pragma once
#include"struct.h"
int ZlzCalcBufSizeOrCopy(PVOID DestBuf, PNET_BUFFER_LIST Nbl, int num)   //调用两次该函数，第一次获取大小，分配内存，第二次拷贝。
{
	PNET_BUFFER_LIST nbltemp = Nbl;
	int size = 0;
	char *buf = DestBuf;
	PNET_BUFFER nb = NET_BUFFER_LIST_FIRST_NB(Nbl);
	do
	{
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
}
NTSTATUS ZlzCopyNdlToBuffer(PVOID DestBuf, PNET_BUFFER_LIST Nbl)
{
	int totalsize = ZlzCalcBufSizeOrCopy(NULL, Nbl, 0);
	PVOID buf = ExAllocatePool(NonPagedPool, totalsize);
	ZlzCalcBufSizeOrCopy(buf, Nbl, 1);
	return STATUS_SUCCESS;
}
