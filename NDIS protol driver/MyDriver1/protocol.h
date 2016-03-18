#pragma warning(push)
#pragma warning(disable:4201)
#include<basetsd.h>
#include<ntifs.h>
#include<ndis.h>
#include<wdm.h>
#include<wdmsec.h>
#pragma warning(pop)
#pragma warning(disable:4127)
#pragma warning(disable:4100)
typedef struct 
{
	LIST_ENTRY read;
	LIST_ENTRY write;
	LIST_ENTRY pack;
	NDIS_HANDLE sendpacketpool;
	NDIS_HANDLE recvpacketpool;
	NDIS_HANDLE recvbufferpool;
	NDIS_STATUS status;
	NDIS_EVENT eve;
	NDIS_EVENT unbindeve;
	int contextno;
}adapercontext, *padapercontext;
typedef struct _NPROT_SEND_PACKET_RSVD
{
	PIRP                    pIrp;
	ULONG                   RefCount;

} NPROT_SEND_PACKET_RSVD, *PNPROT_SEND_PACKET_RSVD;
typedef struct _NPROT_RECV_PACKET_RSVD
{
	LIST_ENTRY              Link;
	PNDIS_BUFFER            pOriginalBuffer;    

} NPROT_RECV_PACKET_RSVD, *PNPROT_RECV_PACKET_RSVD;
typedef struct _GLOBAL
{
	PDRIVER_OBJECT driver;
	PDEVICE_OBJECT dev;
	NDIS_HANDLE ndisprotocolhandle;
	NDIS_HANDLE bindinghandle[10];
	int bindinghandlenum;
	padapercontext context[10];
	int contextnum;
}*PGLOBAL, GLOBAL;
GLOBAL Globals;
NDIS_STATUS ndisprotDoRequest(
	padapercontext context,
	NDIS_REQUEST_TYPE type,
	NDIS_OID Oid,
	PVOID inforbuffer,
	ULONG bufferlength,
	PULONG pbyteprocessd);
VOID
NdisProtOpenAdapterComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN NDIS_STATUS                  Status,
IN NDIS_STATUS                  OpenErrorCode
)
{
	DbgPrint("1\n");
}
VOID
NdisProtCloseAdapterComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN NDIS_STATUS                  Status
)
{
	padapercontext context = (padapercontext)ProtocolBindingContext;
	DbgPrint("enter close adaper complete\n");
	NdisSetEvent(&context->unbindeve);
	context->status = Status;
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
	DbgPrint("1\n");
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
	padapercontext context = (padapercontext)ProtocolBindingContext;
	context->status = Status;
	NdisSetEvent(&context->eve);
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
	DbgPrint("prot rec\n");
	return NDIS_STATUS_SUCCESS;
}
VOID
NdisProtReceiveComplete(
IN NDIS_HANDLE                  ProtocolBindingContext
)
{
	DbgPrint("1\n");
}
VOID
NdisProtStatus(
IN NDIS_HANDLE                          ProtocolBindingContext,
IN NDIS_STATUS                          GeneralStatus,
__in_bcount(StatusBufferSize) IN PVOID  StatusBuffer,
IN UINT                                 StatusBufferSize
)
{
	DbgPrint("1\n");
}
VOID
NdisProtStatusComplete(
IN NDIS_HANDLE                  ProtocolBindingContext
)
{
	DbgPrint("1\n");
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
	NDIS_MEDIUM mediumarray[1] = { NdisMedium802_3 };
	NDIS_STATUS sta;
	NDIS_STATUS errorcode;
	UINT seletemediumundex;
	padapercontext context=ExAllocatePool(NonPagedPool,sizeof(padapercontext));
	context->contextno = Globals.contextnum;
	ULONG pbyterec;
	do
	{
		/*NdisAllocatePacketPoolEx(&sta, &context.sendpacketpool, 20, 380, sizeof(NPROT_SEND_PACKET_RSVD));
		DbgPrint("0x%x\n", sta);
		if (sta != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		NdisAllocatePacketPoolEx(&sta, &context.recvpacketpool, 4, 20, sizeof(NPROT_RECV_PACKET_RSVD));
		DbgPrint("0x%x\n", sta);
		if (sta != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		NdisAllocateBufferPool(&sta, &context.recvbufferpool, 20);
		DbgPrint("0x%x\n", sta);
		if (sta != NDIS_STATUS_SUCCESS)
		{
			break;
		}*/
		NdisOpenAdapter(&sta, &errorcode, &Globals.bindinghandle[Globals.bindinghandlenum], &seletemediumundex, &mediumarray[0], 1, 
			Globals.ndisprotocolhandle, (NDIS_HANDLE)context, pDeviceName, 0, NULL);
		DbgPrint("0x%x\n", sta);
		DbgPrint("0x%x\n", errorcode);
		DbgPrint("%wZ\n", pDeviceName);
		PVOID address = ExAllocatePool(NonPagedPool, 50);
		ndisprotDoRequest(context, NdisRequestQueryInformation, OID_802_3_CURRENT_ADDRESS, address, sizeof(address), &pbyterec);
		DbgPrint("%x\n", address);
		ExFreePool(address);
		Globals.context[Globals.contextnum] = context;
		Globals.contextnum++;
		Globals.bindinghandlenum++;
	} while (FALSE);
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
	ULONG PacketFilter = 0;
	ULONG Byteread;
	int contextnum;
	NDIS_STATUS sta;
	padapercontext context = (padapercontext)ProtocolBindingContext;
	contextnum = context->contextno;
	NdisInitializeEvent(&context->unbindeve);
	ndisprotDoRequest(context, NdisRequestSetInformation, OID_GEN_CURRENT_PACKET_FILTER,
		&PacketFilter, sizeof(PacketFilter), &Byteread);
	ndisprotDoRequest(context, NdisRequestSetInformation, OID_802_3_MULTICAST_LIST, NULL, 0, &Byteread);
	NdisCloseAdapter(&sta, Globals.bindinghandle[contextnum-1]);
	if (sta == NDIS_STATUS_PENDING)
	{
		DbgPrint("unbind pending");
		NdisWaitEvent(&context->unbindeve,0);
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
	DbgPrint("rec packet\n");
	return 0;
}
NDIS_STATUS
NdisProtPnPEventHandler(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNET_PNP_EVENT               pNetPnPEvent
)
{
	DbgPrint("rec pnp\n");
	return NDIS_STATUS_NOT_SUPPORTED;
}
NDIS_STATUS ndisprotDoRequest(
	padapercontext context,
	NDIS_REQUEST_TYPE type,
	NDIS_OID Oid,
	PVOID inforbuffer,
	ULONG bufferlength,
	PULONG pbyteprocessd)
{
	NDIS_STATUS sta;
	NDIS_REQUEST request;
	NdisInitializeEvent(&context->eve);
	switch (type)
	{
	case NdisRequestQueryInformation:
		request.RequestType = NdisRequestQueryInformation;
		request.DATA.QUERY_INFORMATION.Oid = Oid;
		request.DATA.QUERY_INFORMATION.InformationBuffer = inforbuffer;
		request.DATA.QUERY_INFORMATION.InformationBufferLength = bufferlength;
		break;
	case NdisRequestSetInformation:
		request.RequestType = NdisRequestSetInformation;
		request.DATA.SET_INFORMATION.Oid = Oid;
		request.DATA.SET_INFORMATION.InformationBuffer = inforbuffer;
		request.DATA.SET_INFORMATION.InformationBufferLength = bufferlength;
		break;
	default:
		DbgPrint("error\n");
		return NDIS_STATUS_FAILURE;
		break;
	}
	NdisRequest(&sta, Globals.bindinghandle[context->contextno], &request);
	if (sta == NDIS_STATUS_PENDING)
	{
		DbgPrint("pending...\n");
		NdisWaitEvent(&context->eve, 0);
		sta = context->status;
	}
	if (sta == NDIS_STATUS_SUCCESS)
	{
		*pbyteprocessd = (type == NdisRequestQueryInformation) ?
			request.DATA.QUERY_INFORMATION.BytesWritten :
			request.DATA.SET_INFORMATION.BytesRead;
	}
	DbgPrint("request return:0x%x\n", sta);
	return sta;
}