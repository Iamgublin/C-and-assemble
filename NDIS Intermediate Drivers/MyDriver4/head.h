#pragma warning(push)
#pragma warning(disable:4201)
#include<ndis.h>
#include<wdm.h>
#pragma warning(pop)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#define MIN_PACKET_POOL_SIZE 200
#define MAX_PACKET_POOL_SIZE 400
#define NIC_MEDIA_TYPE       NdisMedium802_3
KEVENT queryeve;
KEVENT seteve;
typedef struct _NPROT_SEND_PACKET_RSVD
{
	PIRP                    pIrp;
} NPROT_SEND_PACKET_RSVD, *PNPROT_SEND_PACKET_RSVD;
typedef struct _NPROT_RECV_PACKET_RSVD
{
	PNDIS_BUFFER            pOriginalBuffer;

} NPROT_RECV_PACKET_RSVD, *PNPROT_RECV_PACKET_RSVD;
typedef struct _packet
{
	LIST_ENTRY list;
	PNDIS_PACKET packet;
}packet, *ppacket;
typedef struct
{
	LIST_ENTRY packrecvlist;
	NDIS_HANDLE sendpacketpool;
	NDIS_HANDLE recvpacketpool;
	NDIS_HANDLE recvbufferpool;
	KSPIN_LOCK lock;
	NDIS_STATUS status;
	int contextno;
	NDIS_STRING DeviceName;
	NDIS_EVENT ndiseve;
	NDIS_EVENT unbindeve;
	NDIS_HANDLE relateminiporthandle;
}adapercontext, *padapercontext;
typedef struct
{
	int contextno;
	NDIS_HANDLE Miniportadapterhandle;
	NDIS_HANDLE relateadapterhandle;
	NDIS_REQUEST queryinformationrequest;
	NDIS_REQUEST setinformationrequest;
}miniportcontext, *pminiportcontext;
typedef struct _GLOBAL
{
	NDIS_HANDLE driverhandle;
	NDIS_HANDLE protocolhandle;
	padapercontext adaptercontext[20];   //最多支持20个设备
	NDIS_HANDLE bindinghandle[20];
	int contextnum;
	pminiportcontext miniportcontext[20];
	int mininum;
}GLOBAL, *PGLOBAL;
GLOBAL global;
PGLOBAL pglobal = &global;
typedef struct _Information
{
	PULONG sebyteread;
	PULONG seBytesNeeded;
	PULONG quBytesWritten;
	PULONG quBytesNeeded;
}Information,*pInformation;
Information requestinfo;
VOID
NdisProtRequestComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNDIS_REQUEST                pNdisRequest,
IN NDIS_STATUS                  Status);
NDIS_STATUS ZlzOpenConfiguration(PNDIS_STRING str, PVOID sys1);
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
	pminiportcontext context;
	NdisAllocateMemoryWithTag(&context, sizeof(miniportcontext), 0);
	context->contextno = global.mininum;
	padapercontext relateadaptercontext = NdisIMGetDeviceContext(MiniportAdapterHandle);
	relateadaptercontext->relateminiporthandle = MiniportAdapterHandle;
	context->Miniportadapterhandle = MiniportAdapterHandle;
	context->relateadapterhandle = global.bindinghandle[relateadaptercontext->contextno];
	KeInitializeEvent(&seteve, SynchronizationEvent, TRUE);
	KeInitializeEvent(&queryeve, SynchronizationEvent, TRUE);
	UINT sum;
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
	*SelectedMediumIndex = sum;
	NdisMSetAttributesEx(MiniportAdapterHandle, context, 0, NDIS_ATTRIBUTE_IGNORE_PACKET_TIMEOUT |
		NDIS_ATTRIBUTE_IGNORE_REQUEST_TIMEOUT |
		NDIS_ATTRIBUTE_INTERMEDIATE_DRIVER |
		NDIS_ATTRIBUTE_DESERIALIZE |
		NDIS_ATTRIBUTE_NO_HALT_ON_SUSPEND,
		0);
	global.mininum++;
	return STATUS_SUCCESS;
}
VOID
MPHalt(
IN  NDIS_HANDLE MiniportAdapterContext
)
{
	DbgBreakPoint();
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
	DbgPrint("enter set information\n");
	DbgPrint("oid number:0x%x\n", Oid);
	NDIS_STATUS ndissta;
	pminiportcontext context = MiniportAdapterContext;
	KeWaitForSingleObject(&seteve, Executive, KernelMode, FALSE, NULL);
	context->setinformationrequest.RequestType = NdisRequestSetInformation;
	context->setinformationrequest.DATA.SET_INFORMATION.Oid = Oid;
	context->setinformationrequest.DATA.SET_INFORMATION.InformationBuffer = InformationBuffer;
	context->setinformationrequest.DATA.SET_INFORMATION.InformationBufferLength = InformationBufferLength;
	requestinfo.seBytesNeeded = BytesNeeded;
	requestinfo.sebyteread = BytesRead;
	NdisRequest(&ndissta, context->relateadapterhandle, &context->setinformationrequest);
	if (ndissta != NDIS_STATUS_PENDING)
	{
		NdisProtRequestComplete(context, &context->setinformationrequest, ndissta);
		ndissta = NDIS_STATUS_PENDING;
	}
	KeSetEvent(&seteve, IO_NO_INCREMENT, FALSE);
	return ndissta;
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
	DbgPrint("enter query information\n");
	DbgPrint("oid number:0x%x\n", Oid);
	NDIS_STATUS ndissta;
	pminiportcontext context = MiniportAdapterContext;
	KeWaitForSingleObject(&queryeve, Executive, KernelMode, FALSE, NULL);
	context->queryinformationrequest.RequestType = NdisRequestQueryInformation;
	context->queryinformationrequest.DATA.QUERY_INFORMATION.Oid = Oid;
	context->queryinformationrequest.DATA.QUERY_INFORMATION.InformationBuffer = InformationBuffer;
	context->queryinformationrequest.DATA.QUERY_INFORMATION.InformationBufferLength = InformationBufferLength;
	requestinfo.quBytesNeeded = BytesNeeded;
	requestinfo.quBytesWritten = BytesWritten;
	NdisRequest(&ndissta, context->relateadapterhandle, &context->queryinformationrequest);
	if (ndissta != NDIS_STATUS_PENDING)
	{
		NdisProtRequestComplete(context, &context->queryinformationrequest, ndissta);
		ndissta = NDIS_STATUS_PENDING;
	}
	return ndissta;
}

VOID
MPSendPackets(
IN  NDIS_HANDLE             MiniportAdapterContext,
IN  PPNDIS_PACKET           PacketArray,
IN  UINT                    NumberOfPackets)
{
	DbgPrint("enter send packet\n");
}
VOID
MPReturnPacket(
IN NDIS_HANDLE  MiniportAdapterContext,
IN PNDIS_PACKET Packet
)
{

}
NDIS_STATUS
MPTransferData(
OUT PNDIS_PACKET              Packet,
OUT PUINT                     BytesTransferred,
IN NDIS_HANDLE                MiniportAdapterContext,
IN NDIS_HANDLE                MiniportReceiveContext,
IN UINT                       ByteOffset,
IN UINT                       BytesToTransfer
)
{
	return STATUS_UNSUCCESSFUL;
}



VOID
NdisProtCloseAdapterComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN NDIS_STATUS                  Status
)
{
}
VOID
NdisProtSendComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNDIS_PACKET                 pNdisPacket,
IN NDIS_STATUS                  Status
)
{
	DbgPrint("1\n");
}
VOID
NdisProtTransferDataComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNDIS_PACKET                 pNdisPacket,
IN NDIS_STATUS                  TransferStatus,
IN UINT                         BytesTransferred
)
{
}
VOID
NdisProtResetComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN NDIS_STATUS                  Status
)
{
	DbgPrint("1\n");
}
VOID
NdisProtRequestComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNDIS_REQUEST                pNdisRequest,
IN NDIS_STATUS                  Status
)
{
	DbgPrint("enter request complete\n");
	DbgPrint("status: 0x%x\n", Status);
	padapercontext context = ProtocolBindingContext;
	switch (pNdisRequest->RequestType)
	{
	case NdisRequestSetInformation:
		*requestinfo.seBytesNeeded = pNdisRequest->DATA.SET_INFORMATION.BytesNeeded;
		*requestinfo.sebyteread = pNdisRequest->DATA.SET_INFORMATION.BytesRead;
		NdisMSetInformationComplete(context->relateminiporthandle, Status);
		KeSetEvent(&seteve, IO_NO_INCREMENT, FALSE);
		break;
	case NdisRequestQueryInformation:
		*requestinfo.quBytesNeeded = pNdisRequest->DATA.QUERY_INFORMATION.BytesNeeded;
		*requestinfo.quBytesWritten = pNdisRequest->DATA.QUERY_INFORMATION.BytesWritten;
		NdisMQueryInformationComplete(context->relateminiporthandle, Status);
		KeSetEvent(&queryeve, IO_NO_INCREMENT, FALSE);
		break;
	}
}
NDIS_STATUS
NdisProtReceive(
IN NDIS_HANDLE                              ProtocolBindingContext,
IN NDIS_HANDLE                              MacReceiveContext,
__in_bcount(HeaderBufferSize) IN PVOID      pHeaderBuffer,
IN UINT                                     HeaderBufferSize,
__in_bcount(LookaheadBufferSize) IN PVOID   pLookaheadBuffer,
IN UINT                                     LookaheadBufferSize,
IN UINT                                     PacketSize
)
{
	return NDIS_STATUS_SUCCESS;
}
VOID
NdisProtReceiveComplete(
IN NDIS_HANDLE                  ProtocolBindingContext
)
{
	DbgPrint("enter receive complete\n");
}
VOID
NdisProtStatus(
IN NDIS_HANDLE                          ProtocolBindingContext,
IN NDIS_STATUS                          GeneralStatus,
__in_bcount(StatusBufferSize) IN PVOID  StatusBuffer,
IN UINT                                 StatusBufferSize
)
{
	DbgPrint("enter status\n");
}
VOID
NdisProtStatusComplete(
IN NDIS_HANDLE                  ProtocolBindingContext
)
{
	DbgPrint("enter status complete\n");
}
VOID
NdisProtBindAdapter(
OUT PNDIS_STATUS                pStatus,
IN NDIS_HANDLE                  BindContext,
IN PNDIS_STRING                 pDeviceName,
IN PVOID                        SystemSpecific1,
IN PVOID                        SystemSpecific2
)
{
	DbgBreakPoint();
	DbgPrint("enter bind adapter\n");
	DbgPrint("%wZ\n", SystemSpecific1);
	NDIS_MEDIUM         mediumarray[4] =
	{
		NdisMedium802_3,    // Ethernet
		NdisMedium802_5,    // Token-ring
		NdisMediumFddi,     // Fddi
		NdisMediumWan       // NDISWAN
		/*NdisMediumCoWan*/
	};
	NDIS_STATUS ndissta;
	NDIS_STATUS errorcode;
	NDIS_STRING devname;
	do
	{
		if (ZlzOpenConfiguration(&devname, SystemSpecific1) != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		UINT seletemediumundex = 0;
		padapercontext context;
		NdisAllocateMemoryWithTag(&context, sizeof(adapercontext), 0);
		InitializeListHead(&context->packrecvlist);
		KeInitializeSpinLock(&context->lock);
		NdisInitializeEvent(&context->ndiseve);
		context->contextno = global.contextnum;

		NdisAllocateMemoryWithTag(&context->DeviceName.Buffer, devname.Length, 0);
		NdisMoveMemory(context->DeviceName.Buffer, devname.Buffer, devname.Length);
		context->DeviceName.Length = devname.Length;
		NdisFreeMemory(devname.Buffer, devname.Length, 0);

		NdisAllocatePacketPoolEx(&ndissta, &context->sendpacketpool, MIN_PACKET_POOL_SIZE, MAX_PACKET_POOL_SIZE - MIN_PACKET_POOL_SIZE, sizeof(NPROT_SEND_PACKET_RSVD));
		DbgPrint("0x%x\n", ndissta);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		NdisAllocatePacketPoolEx(&ndissta, &context->recvpacketpool, MIN_PACKET_POOL_SIZE, MAX_PACKET_POOL_SIZE - MIN_PACKET_POOL_SIZE, sizeof(NPROT_RECV_PACKET_RSVD));
		DbgPrint("0x%x\n", ndissta);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		NdisAllocateBufferPool(&ndissta, &context->recvbufferpool, 200);
		DbgPrint("0x%x\n", ndissta);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		NdisOpenAdapter(&ndissta, &errorcode, &global.bindinghandle[global.contextnum], &seletemediumundex, mediumarray, 5,
			global.protocolhandle, (NDIS_HANDLE)context, pDeviceName, 0, NULL);
		DbgPrint("0x%x\n", ndissta);
		DbgPrint("0x%x\n", errorcode);
		DbgPrint("%wZ\n", pDeviceName);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			if (ndissta == NDIS_STATUS_PENDING)
			{
				DbgPrint("PENDING\n");
				NdisWaitEvent(&context->ndiseve, 0);
				ndissta = context->status;
				NdisResetEvent(&context->ndiseve);
			}
			else
			{
				break;
			}
		}
		NdisIMInitializeDeviceInstanceEx(global.driverhandle, &context->DeviceName, context);
		global.adaptercontext[global.contextnum] = context;
		global.contextnum++;
	} while (FALSE);
}
VOID
NdisOpenAdapterComplete(
IN  NDIS_HANDLE             ProtocolBindingContext,
IN  NDIS_STATUS             Status,
IN  NDIS_STATUS             OpenErrorStatus
)
{
	DbgPrint("enter open adapter complete\n");
	padapercontext context = ProtocolBindingContext;
	NdisSetEvent(&context->ndiseve);
	context->status = Status;
}
VOID
NdisProtUnbindAdapter(
OUT PNDIS_STATUS                pStatus,
IN NDIS_HANDLE                  ProtocolBindingContext,
IN NDIS_HANDLE                  UnbindContext
)
{
	DbgBreakPoint();
	DbgPrint("enter unbind\n");
	int num;
	NDIS_STATUS sta;
	padapercontext context = (padapercontext)ProtocolBindingContext;
	num = context->contextno;
	NdisInitializeEvent(&context->unbindeve);
	NdisCloseAdapter(&sta, global.bindinghandle[num]);
	if (sta == NDIS_STATUS_PENDING)
	{
		DbgPrint("unbind pending\n");
		NdisWaitEvent(&context->unbindeve, 0);
		sta = context->status;
	}
	DbgPrint("0x%x\n", sta);
}
INT
NdisProtReceivePacket(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNDIS_PACKET                 pNdisPacket
)
{
	DbgPrint("enter receive packet\n");
	return 0;
}
NDIS_STATUS
NdisProtPnPEventHandler(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNET_PNP_EVENT               pNetPnPEvent
)
{
	DbgPrint("rec pnp\n");
	return NDIS_STATUS_SUCCESS;
}
VOID
NdisProtRegisterAf(
_In_  NDIS_HANDLE             ProtocolBindingContext,
_In_  PCO_ADDRESS_FAMILY      AddressFamily
)
{
	DbgPrint("enter ndis register af\n");
}
NDIS_STATUS ZlzOpenConfiguration(PNDIS_STRING str, PVOID sys1)
{
	NDIS_HANDLE ConfigurationHandle;
	NDIS_STATUS ndissta;
	NDIS_STRING DeviceStr = NDIS_STRING_CONST("UpperBindings");
	PNDIS_CONFIGURATION_PARAMETER   Param=NULL;
	NdisOpenProtocolConfiguration(&ndissta,
		&ConfigurationHandle,
		sys1);
	if (ndissta != NDIS_STATUS_SUCCESS)
	{
		return ndissta;
	}
	NdisReadConfiguration(&ndissta,
		&Param,
		ConfigurationHandle,
		&DeviceStr,
		NdisParameterString);
	DbgPrint("0x%x\n", ndissta);
	if (ndissta != NDIS_STATUS_SUCCESS)
	{
		return STATUS_UNSUCCESSFUL;
	}
	str->Length = Param->ParameterData.StringData.Length;
	str->MaximumLength = str->Length;
	if (str->Buffer == NULL)
	{
		DbgPrint("errror: the buffer is empty!\n");
		return STATUS_UNSUCCESSFUL;
	}
	NdisAllocateMemoryWithTag(&str->Buffer, str->Length, 0);
	NdisMoveMemory(str->Buffer, Param->ParameterData.StringData.Buffer, Param->ParameterData.StringData.Length);
	DbgPrint("%wZ\n", str);
	return NDIS_STATUS_SUCCESS;
}