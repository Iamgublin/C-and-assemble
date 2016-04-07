#pragma warning(push)
#pragma warning(disable:4201)
#include<ndis.h>
#include<wdf.h>
#include<wdfminiport.h>
#pragma warning(pop)
#pragma warning(disable:4100)
#define NIC_MEDIA_TYPE                  NdisMedium802_3
#define NIC_ADAPTER_NAME_SIZE           30
#define NIC_MAX_MCAST_LIST              32
#define ETH_HEADER_SIZE                 14
#define ETH_MAX_DATA_SIZE               1500
#define NIC_BUFFER_SIZE                 ETH_MAX_DATA_SIZE+ETH_HEADER_SIZE  
#define PROTOCOL_NDIS_NAME              L"\\??\\WIN NDIS protocol device create by zlz"
#define IOCTL_NDISPROT_BIND_WAIT (ULONG)CTL_CODE(FILE_DEVICE_NETWORK,0x912,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
typedef struct _NDISPROT_INDICATE_STATUS
{
	ULONG            IndicatedStatus;        // NDIS_STATUS
	ULONG            StatusBufferOffset;    // from start of this struct
	ULONG            StatusBufferLength;    // in bytes
} NDISPROT_INDICATE_STATUS, *PNDISPROT_INDICATE_STATUS;
typedef struct _MP_ADAPTER
{
	LIST_ENTRY              List;
	LONG                    RefCount;
	NDIS_SPIN_LOCK          Lock;
	NDIS_EVENT              InitEvent;
	NDIS_EVENT              HaltEvent;

	//
	// Keep track of various device objects.
	//
	PDEVICE_OBJECT          Pdo;
	PDEVICE_OBJECT          Fdo;
	PDEVICE_OBJECT          NextDeviceObject;
	WDFDEVICE               WdfDevice;
	NDIS_HANDLE             AdapterHandle;
	WCHAR                   AdapterName[NIC_ADAPTER_NAME_SIZE];
	WCHAR                   AdapterDesc[NIC_ADAPTER_NAME_SIZE];
	ULONG                   Flags;
	UCHAR                   PermanentAddress[ETH_LENGTH_OF_ADDRESS];
	UCHAR                   CurrentAddress[ETH_LENGTH_OF_ADDRESS];
	WDFWORKITEM             WorkItemForNdisRequest;
	BOOLEAN                 Promiscuous;
	BOOLEAN                 RequestPending;
	BOOLEAN                 ResetPending;
	BOOLEAN                 IsHardwareDevice;
	BOOLEAN                 IsTargetSupportChainedMdls;
	//
	// Variables to track resources for the send operation
	//
	NDIS_HANDLE             SendBufferPoolHandle;
	LIST_ENTRY              SendFreeList;
	LIST_ENTRY              SendWaitList;
	LIST_ENTRY              SendBusyList;
	PUCHAR                  TCBMem;
	LONG                    nBusySend;
	NDIS_SPIN_LOCK          SendLock;

	//
	// Variables to track resources for the Receive operation
	//
	LIST_ENTRY              RecvFreeList;
	LIST_ENTRY              RecvBusyList;
	NDIS_SPIN_LOCK          RecvLock;
	LONG                    nBusyRecv;
	NDIS_HANDLE             RecvPacketPoolHandle;
	NDIS_HANDLE             RecvBufferPoolHandle;
	PUCHAR                  RCBMem;
	WDFWORKITEM             ReadWorkItem;
	WDFWORKITEM             ExecutiveCallbackWorkItem;
	LONG                    IsReadWorkItemQueued;
	//
	// Packet Filter and look ahead size.
	//
	ULONG                   PacketFilter;
	ULONG                   ulLookAhead;
	ULONG                   ulLinkSpeed;

	// multicast list
	ULONG                   ulMCListSize;
	UCHAR                   MCList[NIC_MAX_MCAST_LIST][ETH_LENGTH_OF_ADDRESS];

	// Packet counts
	ULONG64                 GoodTransmits;
	ULONG64                 GoodReceives;
	ULONG                   NumTxSinceLastAdjust;

	// Count of transmit errors
	ULONG                   TxAbortExcessCollisions;
	ULONG                   TxLateCollisions;
	ULONG                   TxDmaUnderrun;
	ULONG                   TxLostCRS;
	ULONG                   TxOKButDeferred;
	ULONG                   OneRetry;
	ULONG                   MoreThanOneRetry;
	ULONG                   TotalRetries;
	ULONG                   TransmitFailuresOther;

	// Count of receive errors
	ULONG                   RcvCrcErrors;
	ULONG                   RcvAlignmentErrors;
	ULONG                   RcvResourceErrors;
	ULONG                   RcvDmaOverrunErrors;
	ULONG                   RcvCdtFrames;
	ULONG                   RcvRuntErrors;

	//
	// Talking to NDISPROT protocol
	//
	HANDLE                  FileHandle;
	PFILE_OBJECT            FileObject;
	WDFIOTARGET             IoTarget;
	UCHAR                   PhyNicMacAddress[ETH_LENGTH_OF_ADDRESS];
	PCALLBACK_OBJECT        CallbackObject;
	PVOID                   CallbackRegisterationHandle;
	WDFREQUEST              StatusIndicationRequest;
	NDISPROT_INDICATE_STATUS NdisProtIndicateStatus;
	WDFMEMORY               WdfStatusIndicationBuffer;
	//
	// Statistic for debugging & validation purposes
	//
	ULONG                   nReadsPosted;
	ULONG                   nReadsCompletedWithError;
	ULONG                   nPacketsIndicated;
	ULONG                   nPacketsReturned;
	ULONG                   nBytesRead;
	ULONG                   nPacketsArrived;
	ULONG                   nWritesPosted;
	ULONG                   nWritesCompletedWithError;
	ULONG                   nBytesArrived;
	ULONG                   nBytesWritten;
	ULONG                   nReadWorkItemScheduled;
	ULONG                   adapternum;
} MP_ADAPTER, *PMP_ADAPTER;
typedef struct _GLOBAL
{
	int size;
	NDIS_HANDLE wraphandle;
	NDIS_SPIN_LOCK lock;
	LIST_ENTRY list;
	int adapternum;
	PMP_ADAPTER adapter[20];
}GLOBAL, *PGLOBAL;
typedef struct _TCB
{
	LIST_ENTRY              List; // This must be the first entry
	LONG                    Ref;
	PVOID                   Adapter;
	WDFREQUEST              Request;
	PMDL                    Mdl;
	PNDIS_BUFFER            Buffer;
	PNDIS_PACKET            OrgSendPacket;
	PUCHAR                  pData;
	ULONG                   ulSize;
	UCHAR                   Data[NIC_BUFFER_SIZE];
} TCB, *PTCB;
typedef struct _RCB
{
	LIST_ENTRY              List; // This must be the first entry
	LONG                    Ref;
	PVOID                   Adapter;
	WDFREQUEST              Request;
	PNDIS_BUFFER            Buffer;
	PNDIS_PACKET            Packet;
	PUCHAR                  pData;
	ULONG                   ulSize;
	UCHAR                   Data[NIC_BUFFER_SIZE];
} RCB, *PRCB;
typedef struct _WDF_DEVICE_INFO{

	PMP_ADAPTER Adapter;

} WDF_DEVICE_INFO, *PWDF_DEVICE_INFO;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDF_DEVICE_INFO, GetWdfDeviceInfo);
GLOBAL global;
NTSTATUS connecttarget(int adapternum);
NTSTATUS ZlzDeviceIoControl(WDFIOTARGET target,ULONG ioctl,    \
	PVOID inputbuffer,ULONG inputbuffersize,PVOID outputbuffer,ULONG outputbuffersize);
NTSTATUS ZlzOpenProtocolInterface(PMP_ADAPTER adapter);
VOID ZlzPostWriteRequest(PMP_ADAPTER adapter, PTCB ptcb);
VOID ZlzRequestWriteComplete(WDFREQUEST Request, WDFIOTARGET Target, PWDF_REQUEST_COMPLETION_PARAMS Params, WDFCONTEXT Context);
BOOLEAN ZlzCopyPacket(PMP_ADAPTER adapter, PTCB tcb, PNDIS_PACKET packet);
VOID Zlzndissendpacket(PRCB prcb, ULONG bytetoindicate);
VOID ZlzReadRequestComplete(WDFREQUEST Request, WDFIOTARGET Target, PWDF_REQUEST_COMPLETION_PARAMS Params, WDFCONTEXT Context);
NTSTATUS  ZlzPostReadRequest(PMP_ADAPTER adapter, PRCB prcb);
NTSTATUS Zlzinitwork(PMP_ADAPTER adapter);
VOID ZlzWorkItemCallback(WDFWORKITEM WorkItem);
VOID ZlzFreeRcb(PRCB ptcb);
NDIS_STATUS
MPInitialize(
OUT PNDIS_STATUS OpenErrorStatus,
OUT PUINT SelectedMediumIndex,
IN PNDIS_MEDIUM MediumArray,
IN UINT MediumArraySize,                          //介质类型大小
IN NDIS_HANDLE MiniportAdapterHandle,            //适配器句柄
IN NDIS_HANDLE WrapperConfigurationContext)    //注册表获取信息
{
	DbgBreakPoint();
	UINT sum;
	WDF_OBJECT_ATTRIBUTES woa;
	for (sum = 0; sum < MediumArraySize; ++sum)
	{
		if (MediumArray[sum] == NIC_MEDIA_TYPE)
		{
			break;
		}
	}
	if (sum == MediumArraySize)
	{
		DbgPrint("cannon find 802.3");
		return NDIS_STATUS_UNSUPPORTED_MEDIA;
	}
	PMP_ADAPTER adapter;
	NdisAllocateMemoryWithTag(&adapter, sizeof(MP_ADAPTER), 0);
	NdisZeroMemory(adapter, sizeof(MP_ADAPTER));
	global.adapter[global.adapternum] = adapter;
	global.adapter[global.adapternum]->adapternum = global.adapternum;
	global.adapternum++;
	NdisInitializeListHead(&adapter->SendBusyList);
	NdisInitializeListHead(&adapter->SendWaitList);
	NdisAllocateSpinLock(&adapter->Lock);
	NdisAllocateSpinLock(&adapter->SendLock);
	NdisAllocateSpinLock(&adapter->RecvLock);
	*SelectedMediumIndex = sum;
	adapter->AdapterHandle = MiniportAdapterHandle;
	NdisMGetDeviceProperty(MiniportAdapterHandle, &adapter->Pdo, &adapter->Fdo, &adapter->NextDeviceObject, NULL, NULL);
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&woa, WDF_DEVICE_INFO);
	WdfDeviceMiniportCreate(WdfGetDriver(), &woa, adapter->Fdo, adapter->NextDeviceObject, adapter->Pdo, &adapter->WdfDevice);
	NdisMSetAttributesEx(MiniportAdapterHandle, adapter, 0, NDIS_ATTRIBUTE_DESERIALIZE |
		NDIS_ATTRIBUTE_USES_SAFE_BUFFER_APIS, NdisInterfaceInternal);
	ZlzOpenProtocolInterface(adapter);
	Zlzinitwork(adapter);
	return NDIS_STATUS_SUCCESS;
}
VOID
MPHalt(
IN  NDIS_HANDLE MiniportAdapterContext
)
{
	DbgBreakPoint();
	PMP_ADAPTER adapter = (PMP_ADAPTER)MiniportAdapterContext;
	NdisReleaseSpinLock(&adapter->SendLock);
	NdisReleaseSpinLock(&adapter->Lock);
	NdisReleaseSpinLock(&adapter->RecvLock);
	NdisFreeMemory(global.adapter[adapter->adapternum], sizeof(MP_ADAPTER), 0);
	if (adapter->IoTarget != NULL)
	{
		WdfIoTargetStop(adapter->IoTarget, WdfIoTargetCancelSentIo);
	}
}
NDIS_STATUS
MPSetInformation(
IN NDIS_HANDLE MiniportAdapterContext,
IN NDIS_OID Oid,
IN PVOID InformationBuffer,
IN ULONG InformationBufferLength,
OUT PULONG BytesRead,
OUT PULONG BytesNeeded)
{
	return NDIS_STATUS_SUCCESS;
}
NDIS_STATUS
MPQueryInformation(
IN NDIS_HANDLE  MiniportAdapterContext,
IN NDIS_OID     Oid,
IN PVOID        InformationBuffer,
IN ULONG        InformationBufferLength,
OUT PULONG      BytesWritten,
OUT PULONG      BytesNeeded)
{
	return NDIS_STATUS_SUCCESS;
}
VOID
MPSendPackets(
IN  NDIS_HANDLE             MiniportAdapterContext,
IN  PPNDIS_PACKET           PacketArray,
IN  UINT                    NumberOfPackets)
{
	DbgPrint("enter mini send packet\n");
	PMP_ADAPTER adapter = MiniportAdapterContext;
	PNDIS_PACKET packet = NULL;
	PTCB ptcb = NULL;
	for (UINT packetcount = 0; packetcount < NumberOfPackets; packetcount++)
	{
		packet = PacketArray[packetcount];
		if (!packet)
		{
			continue;
		}
		adapter->nPacketsArrived++;
		if (adapter->IoTarget)
		{
			ptcb = (PTCB)NdisInterlockedRemoveHeadList(&adapter->SendFreeList, &adapter->SendLock);
			if (ptcb == NULL)
			{
				NdisInterlockedInsertTailList(&adapter->SendWaitList, (PLIST_ENTRY)&packet->MiniportReserved, &adapter->SendLock);
			}
			else
			{
				NdisInterlockedIncrement(&adapter->nBusySend);
				if (ZlzCopyPacket(adapter, ptcb, packet))
				{
					adapter->nWritesPosted++;
					ZlzPostWriteRequest(adapter, ptcb);
					NDIS_SET_PACKET_STATUS(packet, NDIS_STATUS_SUCCESS);
					NdisMSendComplete(adapter->AdapterHandle, packet, NDIS_STATUS_SUCCESS);
				}
			}
		}
	}
}
VOID
MPReturnPacket(
IN NDIS_HANDLE  MiniportAdapterContext,
IN PNDIS_PACKET Packet
)
{
	DbgPrint("enter packet return");
	PRCB prcb = NULL;
	PMP_ADAPTER adapter;
	prcb = *(PRCB*)Packet->MiniportReserved;
	adapter = prcb->Adapter;
	adapter->nPacketsReturned++;
	ZlzFreeRcb(prcb);
}
BOOLEAN
MPCheckForHang(
IN NDIS_HANDLE MiniportAdapterContext)
{
	return TRUE;
}
NTSTATUS connecttarget(int adapternum)
{
	WDFIOTARGET iotarget;
	NTSTATUS sta = STATUS_SUCCESS;
	WDF_IO_TARGET_OPEN_PARAMS para;
	sta = WdfIoTargetCreate(global.adapter[adapternum]->WdfDevice, WDF_NO_OBJECT_ATTRIBUTES, &iotarget);
	if (!NT_SUCCESS(sta))
	{
		sta = STATUS_UNSUCCESSFUL;
		return sta;
	}
	UNICODE_STRING filename;
	RtlInitUnicodeString(&filename, PROTOCOL_NDIS_NAME);
	WDF_IO_TARGET_OPEN_PARAMS_INIT_CREATE_BY_NAME(&para,&filename,STANDARD_RIGHTS_ALL);
	sta=WdfIoTargetOpen(iotarget, &para);
	if (!NT_SUCCESS(sta))
	{
		sta = STATUS_UNSUCCESSFUL;
		return sta;
	}
	global.adapter[adapternum]->IoTarget = iotarget;
	return sta;
}
NTSTATUS ZlzDeviceIoControl(WDFIOTARGET target, ULONG ioctl, \
	PVOID inputbuffer, ULONG inputbuffersize, PVOID outputbuffer, ULONG outputbuffersize)
{
	NTSTATUS sta;
	WDF_MEMORY_DESCRIPTOR input, output;
	PWDF_MEMORY_DESCRIPTOR pinput=NULL, poutput=NULL;
	ULONG_PTR byteret;
	if (inputbuffer)
	{
		WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&input, inputbuffer, inputbuffersize);
		pinput = &input;
	}
	if (outputbuffer)
	{
		WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&output, outputbuffer, outputbuffersize);
		poutput = &output;
	}
	sta = WdfIoTargetSendIoctlSynchronously(target, WDF_NO_HANDLE, ioctl, pinput, poutput, NULL, &byteret);
	if (!NT_SUCCESS(sta))
	{
		DbgPrint("IOCTL SEND ERROR\n");
		return sta;
	}
	DbgPrint("byteret:%d", byteret);
	return sta;
}
NTSTATUS ZlzOpenProtocolInterface(PMP_ADAPTER adapter)
{
	NTSTATUS sta;
	sta = connecttarget(adapter->adapternum);
	return sta;
}
BOOLEAN ZlzCopyPacket(PMP_ADAPTER adapter, PTCB ptcb, PNDIS_PACKET packet)
{
	NdisInitializeListHead(&ptcb->List);
	PUCHAR dest = ptcb->Data;
	ptcb->OrgSendPacket = packet;
	PNDIS_BUFFER buffer=NULL;
	PVOID firstbufferVA;
	UINT firstbufferlength;
	UINT totalbufferlength;
	BOOLEAN result = TRUE;
	UINT bytecopy = 0; 

	PVOID buffervirtualaddress=NULL;
	UINT currentlength=0;
	UINT sizeremain = NIC_BUFFER_SIZE;
	NdisGetFirstBufferFromPacketSafe(packet, &buffer, &firstbufferVA, &firstbufferlength, &totalbufferlength, NormalPagePriority);
	while (buffer)
	{
		NdisQueryBufferSafe(buffer, &buffervirtualaddress, &currentlength,NormalPagePriority);
		if (!buffervirtualaddress)
		{
			result = FALSE;
			break;
		}
		currentlength = min(currentlength, sizeremain);
		if (currentlength)
		{
			NdisMoveMemory(dest, buffervirtualaddress, currentlength);
			sizeremain -= currentlength;
			bytecopy += currentlength;
			dest += currentlength;
		}
		NdisGetNextBuffer(buffer, &buffer);
	}
	if (result)
	{
		NdisInterlockedInsertTailList(&adapter->SendBusyList, &ptcb->List, &adapter->Lock);
	}
	return result;
}
VOID ZlzPostWriteRequest(PMP_ADAPTER adapter, PTCB ptcb)
{
	PIO_STACK_LOCATION nextsta;
	PIRP irp;
	WdfRequestCreate(NULL, adapter->IoTarget, &ptcb->Request);
	WdfIoTargetFormatRequestForWrite(adapter->IoTarget, ptcb->Request, NULL, NULL, NULL);
	irp=WdfRequestWdmGetIrp(ptcb->Request);
	PMDL mdl = NULL;
	mdl = IoAllocateMdl(ptcb->Data, NIC_BUFFER_SIZE, FALSE, FALSE, irp);
	ptcb->Mdl = mdl;
	nextsta = IoGetNextIrpStackLocation(irp);
	nextsta->Parameters.Write.Length = ptcb->ulSize;
	WdfRequestSetCompletionRoutine(ptcb->Request, ZlzRequestWriteComplete, ptcb);
	if (!WdfRequestSend(ptcb->Request, adapter->IoTarget, WDF_NO_SEND_OPTIONS))
	{
		DbgPrint("Send Error!\n");
	}
}
VOID ZlzRequestWriteComplete(WDFREQUEST Request, WDFIOTARGET Target, PWDF_REQUEST_COMPLETION_PARAMS Params, WDFCONTEXT Context)
{
	PTCB ptcb = (PTCB)Context;
	if (ptcb->Mdl)
	{
		IoFreeMdl(ptcb->Mdl);
	}
}
VOID Zlzndissendpacket(PRCB prcb,ULONG bytetoindicate)
{
	PMP_ADAPTER adapter = prcb->Adapter;
	PNDIS_PACKET packtet = NULL;
	PNDIS_BUFFER firstbuf = NULL;
	UINT totallength;
	NdisAdjustBufferLength(prcb->Buffer, bytetoindicate);
	NdisReinitializePacket(packtet);
	*((PRCB*)packtet->MiniportReserved) = prcb;
	NdisChainBufferAtBack(packtet, prcb->Buffer);
	NdisQueryPacket(packtet, NULL, NULL, &firstbuf, &totallength);
	NDIS_SET_PACKET_STATUS(packtet, STATUS_SUCCESS);
	adapter->nPacketsIndicated++;
	NdisMIndicateReceivePacket(adapter->AdapterHandle, &prcb->Packet, 1);
}
VOID ZlzReadRequestComplete(WDFREQUEST Request, WDFIOTARGET Target, PWDF_REQUEST_COMPLETION_PARAMS Params, WDFCONTEXT Context)
{
	DbgBreakPoint();
	PRCB prcb = (PRCB)Context;
	PMP_ADAPTER Adapter = prcb->Adapter;
	ULONG byteread = 0;
	BOOLEAN bIndicateReceive = FALSE;
	NTSTATUS status;
	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Request);
	status = Params->IoStatus.Status;
	if (!NT_SUCCESS(status))
	{
		Adapter->nReadsCompletedWithError++;
	}
	else
	{
		byteread = (ULONG)Params->IoStatus.Information;
		Adapter->GoodReceives++;
		bIndicateReceive = TRUE;
		Adapter->nBytesRead += byteread;
	}
	if (bIndicateReceive)
	{
		Zlzndissendpacket(prcb, byteread);
	}
	ZlzFreeRcb(prcb);
}
VOID ZlzFreeRcb(PRCB prcb)
{
	IoFreeMdl(prcb->Buffer);
	WdfWorkItemEnqueue(((PMP_ADAPTER)prcb->Adapter)->ReadWorkItem);
}
NTSTATUS ZlzPostReadRequest(PMP_ADAPTER adapter, PRCB prcb)
{
	DbgBreakPoint();
	NTSTATUS status = STATUS_SUCCESS;
	PIRP irp = NULL;
	PIO_STACK_LOCATION nextstack;
	status = WdfIoTargetFormatRequestForRead(adapter->IoTarget, prcb->Request, NULL, NULL, NULL);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	irp = WdfRequestWdmGetIrp(prcb->Request);
	nextstack = IoGetNextIrpStackLocation(irp);
	PMDL mdl = NULL;
	mdl = IoAllocateMdl(prcb->Data, NIC_BUFFER_SIZE, FALSE, FALSE, irp);
	prcb->Buffer = mdl;
	nextstack->Parameters.Read.Length = NIC_BUFFER_SIZE;
	WdfRequestSetCompletionRoutine(prcb->Request, ZlzReadRequestComplete, prcb);
	if (WdfRequestSend(prcb->Request, adapter->IoTarget, WDF_NO_SEND_OPTIONS) == FALSE)
	{
		status = WdfRequestGetStatus(prcb->Request);
	}
	return status;
}
NTSTATUS Zlzinitwork(PMP_ADAPTER adapter)
{
	WDF_WORKITEM_CONFIG workconfig;
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_WORKITEM_CONFIG_INIT(&workconfig, ZlzWorkItemCallback);
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = adapter->WdfDevice;
	WdfWorkItemCreate(&workconfig,&attributes,&adapter->ReadWorkItem);
	WdfWorkItemEnqueue(adapter->ReadWorkItem);
	return STATUS_SUCCESS;
}
VOID ZlzWorkItemCallback(WDFWORKITEM WorkItem)
{
	DbgBreakPoint();
	PMP_ADAPTER adapter;
	NTSTATUS sta;
	PRCB prcb = NULL;
	adapter = GetWdfDeviceInfo(WdfWorkItemGetParentObject(WorkItem))->Adapter;
	NdisAcquireSpinLock(&adapter->RecvLock);
	while (IsListEmpty(&adapter->RecvFreeList))
	{
		prcb = (PRCB)RemoveHeadList(&adapter->RecvFreeList);
		InsertTailList(&adapter->RecvBusyList, &prcb->List);
		NdisReleaseSpinLock(&adapter->RecvLock);
		adapter->nReadsPosted++;
		sta = ZlzPostReadRequest(adapter, prcb);
		NdisAcquireSpinLock(&adapter->RecvLock);
		if (NT_SUCCESS(sta))
		{
			break;
		}
	}
	NdisReleaseSpinLock(&adapter->RecvLock);
}