#pragma warning(push)
#pragma warning(disable:4201)
#pragma warning(disable:4214)
#include<ndis.h>
#include<wdm.h>
#include"struct.h"
#pragma warning(pop)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#define MIN_PACKET_POOL_SIZE 200
#define MAX_PACKET_POOL_SIZE 400
#define NIC_MEDIA_TYPE       NdisMedium802_3
#define 	NdisMedium802_3  0  // Ethernet
#define     NdisMedium802_5  1  // Token-ring
#define     NdisMediumFddi   2  // Fddi
#define     NdisMediumWan    3  // NDISWAN
#define     Tranverse16(X)   ((((UINT16)(X) & 0xff00) >> 8) |(((UINT16)(X) & 0x00ff) << 8))
VOID checkcount();
NDIS_STATUS ZlzAnalysePacket(PNDIS_PACKET Packet,enum PacketType type);
VOID ZlzAnalysisIpPacket(UCHAR *buf, pippackinfo info);
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
	pminiportcontext context;
	NdisAllocateMemoryWithTag(&context, sizeof(miniportcontext), 0);
	context->contextno = global.mininum;
	padapercontext relateadaptercontext = NdisIMGetDeviceContext(MiniportAdapterHandle);
	relateadaptercontext->relateminiporthandle = MiniportAdapterHandle;
	context->Miniportadapterhandle = MiniportAdapterHandle;
	context->relateadaptercontext = relateadaptercontext;
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
	if (context->relateadaptercontext->powerstatus)
	{
		return NDIS_STATUS_FAILURE;
	}
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
	NDIS_STATUS ndissta;
	pminiportcontext context = MiniportAdapterContext;
	if (context->relateadaptercontext->powerstatus)
	{
		return NDIS_STATUS_FAILURE;
	}
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
NDIS_STATUS
MPSend(
_In_  NDIS_HANDLE             MiniportAdapterContext,
_In_  PNDIS_PACKET            Packet,
_In_  UINT                    Flags
)
{
	DbgPrint("enter packet send\n");
	PNDIS_PACKET_STACK pps;
	NDIS_STATUS ndissta = STATUS_UNSUCCESSFUL;
	BOOLEAN reuse = TRUE;
	PNDIS_PACKET packet=NULL;
	pminiportcontext context = MiniportAdapterContext;
	UINT mediasize;
	PVOID mediainfo;
	ZlzAnalysePacket(Packet, ZLZ_PACKET_SEND);
	pps = NdisIMGetCurrentPacketStack(Packet, &reuse);
	if (reuse)
	{
		DbgPrint("reuse the packet\n");
		NdisSend(&ndissta, context->relateadapterhandle, Packet);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			if (ndissta == NDIS_STATUS_PENDING)
			{
				DbgPrint("packet send pending\n");
			}
			else
			{
				return NDIS_STATUS_FAILURE;
			}
		}
	}
	else
	{
		DbgPrint("rebulid the packet\n");
		NdisAllocatePacket(&ndissta, &packet, context->relateadaptercontext->sendpacketpool);
		PSEND_PACKET_RSVD SendRsvd;
		SendRsvd = (PSEND_PACKET_RSVD)(packet->ProtocolReserved);
		SendRsvd->pOriginalPacket = Packet;
		packet->Private.Flags = Flags;
		NDIS_PACKET_FIRST_NDIS_BUFFER(packet) = NDIS_PACKET_FIRST_NDIS_BUFFER(Packet);
		NDIS_PACKET_LAST_NDIS_BUFFER(packet) = NDIS_PACKET_LAST_NDIS_BUFFER(Packet);
		NdisIMCopySendPerPacketInfo(packet, Packet);
		NdisMoveMemory(NDIS_OOB_DATA_FROM_PACKET(packet), NDIS_OOB_DATA_FROM_PACKET(Packet), sizeof(NDIS_PACKET_OOB_DATA));
		NDIS_GET_PACKET_MEDIA_SPECIFIC_INFO(Packet, &mediainfo, &mediasize);
		if (mediainfo || mediasize)
		{
			NDIS_SET_PACKET_MEDIA_SPECIFIC_INFO(packet, mediainfo, mediasize);
		}
		NdisSend(&ndissta, context->relateadapterhandle, packet);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			if (ndissta == NDIS_STATUS_PENDING)
			{
				DbgPrint("packet send pending\n");
				return ndissta;
			}
			else
			{
				return NDIS_STATUS_FAILURE;
			}
		}
		NdisFreePacket(packet);
	}
	return ndissta;
}
VOID
MPReturnPacket(
IN NDIS_HANDLE  MiniportAdapterContext,
IN PNDIS_PACKET Packet
)
{
	pminiportcontext context = MiniportAdapterContext;
	DbgPrint("enter return packet\n");
	if (NdisGetPoolFromPacket(Packet) != context->relateadaptercontext->recvpacketpool)
	{
		NdisReturnPackets(&Packet, 1);
	}
	else
	{
		PNDIS_PACKET originalpacket;
		PRECV_PACKET_RSVD recv;
		recv = (PRECV_PACKET_RSVD)Packet->ProtocolReserved;
		originalpacket = recv->pOriginalPacket;
		NdisFreePacket(Packet);
		NdisReturnPackets(&originalpacket, 1);
	}
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
	pminiportcontext context = MiniportAdapterContext;
	DbgPrint("enter transferdata\n");
	NDIS_STATUS ndissta;
	NdisTransferData(&ndissta, context->relateadapterhandle, MiniportReceiveContext, ByteOffset, BytesToTransfer,Packet, BytesTransferred);
	if (ndissta != NDIS_STATUS_PENDING)
	{
		//分析数据包
		ZlzAnalysePacket(Packet, ZLZ_PACKET_RECEIVE);
	}

	return ndissta;
}



VOID
NdisProtCloseAdapterComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN NDIS_STATUS                  Status
)
{
	padapercontext context = ProtocolBindingContext;
	KeSetEvent(&context->unbindeve, IO_NO_INCREMENT, FALSE);
}
VOID
NdisProtSendComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNDIS_PACKET                 pNdisPacket,
IN NDIS_STATUS                  Status
)
{
	DbgPrint("enter send complete\n");
	padapercontext context = ProtocolBindingContext;
	NDIS_HANDLE poolhandle = NdisGetPoolFromPacket(pNdisPacket);
	PSEND_PACKET_RSVD SendRsvd;
	PNDIS_PACKET Packet;
	if (poolhandle != context->sendpacketpool)
	{
		NdisMSendComplete(context->relateminiporthandle, pNdisPacket, Status);
	}
	else
	{
		SendRsvd = (PSEND_PACKET_RSVD)(pNdisPacket->ProtocolReserved);
		Packet = SendRsvd->pOriginalPacket;
		NdisIMCopySendCompletePerPacketInfo(Packet, pNdisPacket);
		NdisDprFreePacket(pNdisPacket);
		NdisMSendComplete(context->relateminiporthandle, Packet, Status);
	}
}
VOID
NdisProtTransferDataComplete(
IN NDIS_HANDLE                  ProtocolBindingContext,
IN PNDIS_PACKET                 pNdisPacket,
IN NDIS_STATUS                  TransferStatus,
IN UINT                         BytesTransferred
)
{
	DbgPrint("enter transfer data complete\n");
	padapercontext context = ProtocolBindingContext;
	//分析数据包
	ZlzAnalysePacket(pNdisPacket, ZLZ_PACKET_RECEIVE);
	NdisMTransferDataComplete(context->relateminiporthandle, pNdisPacket, TransferStatus, BytesTransferred);

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
	padapercontext context = ProtocolBindingContext;
	if (context->powerstatus)
	{
		return;
	}
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
	DbgPrint("enter prot receive\n");
	PNDIS_PACKET packet = NULL;
	PNDIS_PACKET newpacket = NULL;
	NDIS_STATUS ndissta;
	padapercontext context = ProtocolBindingContext;
	packet = NdisGetReceivedPacket(global.bindinghandle[context->contextno], MacReceiveContext);
	if (packet == NULL)
	{
		DbgPrint("enter get packet descriptor failed\n");
		context->isnotgetdescriptor = TRUE;
		switch (context->seletemediumundex)
		{
		case NdisMedium802_3:
		case NdisMediumWan:
			NdisMEthIndicateReceive(context->relateminiporthandle, MacReceiveContext, pHeaderBuffer
				, HeaderBufferSize, pLookaheadBuffer, LookaheadBufferSize, PacketSize);
			break;
		case NdisMedium802_5:
			return NDIS_STATUS_FAILURE;
		case NdisMediumFddi:
			return NDIS_STATUS_FAILURE;
		default:
			return NDIS_STATUS_FAILURE;
		}
	}
	else
	{
		DbgPrint("enter get packet descriptor success\n");
		NdisAllocatePacket(&ndissta, &newpacket, context->recvpacketpool);
		newpacket->Private.Flags = packet->Private.Flags;
		NDIS_PACKET_FIRST_NDIS_BUFFER(newpacket) = NDIS_PACKET_FIRST_NDIS_BUFFER(packet);
		NDIS_PACKET_LAST_NDIS_BUFFER(newpacket) = NDIS_PACKET_LAST_NDIS_BUFFER(packet);
		NDIS_SET_PACKET_HEADER_SIZE(newpacket, HeaderBufferSize);
		NDIS_SET_ORIGINAL_PACKET(newpacket, NDIS_GET_ORIGINAL_PACKET(packet));
		NDIS_SET_PACKET_STATUS(newpacket, NDIS_STATUS_RESOURCES);
		NdisMIndicateReceivePacket(context->relateminiporthandle, &newpacket, 1);
		NdisDprFreePacket(newpacket);
	}
	return NDIS_STATUS_SUCCESS;
}
VOID
NdisProtReceiveComplete(
IN NDIS_HANDLE                  ProtocolBindingContext
)
{
	DbgPrint("enter receive complete\n");
	padapercontext context = ProtocolBindingContext;
	if (context->isnotgetdescriptor)
	{
		switch (context->seletemediumundex)
		{
		case NdisMedium802_3:
		case NdisMediumWan:
			NdisMEthIndicateReceiveComplete(context->relateminiporthandle);
			break;
		case NdisMedium802_5:
		case NdisMediumFddi:
			ASSERT(FALSE);
			break;
		default:
			ASSERT(FALSE);
		}
		context->isnotgetdescriptor = FALSE;
	}
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
	DbgPrint("enter bind adapter\n");
	DbgPrint("%wZ\n", SystemSpecific1);
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
		context->powerstatus = 0;
		InitializeListHead(&context->packrecvlist);
		KeInitializeSpinLock(&context->lock);
		NdisInitializeEvent(&context->ndiseve);
		KeInitializeEvent(&context->recveve, SynchronizationEvent, TRUE);
		KeInitializeEvent(&context->unbindeve, SynchronizationEvent, FALSE);
		context->contextno = global.contextnum;

		NdisAllocateMemoryWithTag(&context->DeviceName.Buffer, devname.Length, 0);
		NdisMoveMemory(context->DeviceName.Buffer, devname.Buffer, devname.Length);
		context->DeviceName.Length = devname.Length;
		NdisFreeMemory(devname.Buffer, devname.Length, 0);

		NdisAllocatePacketPoolEx(&ndissta, &context->sendpacketpool, MIN_PACKET_POOL_SIZE, MAX_PACKET_POOL_SIZE - MIN_PACKET_POOL_SIZE, sizeof(SEND_PACKET_RSVD));
		DbgPrint("0x%x\n", ndissta);
		if (ndissta != NDIS_STATUS_SUCCESS)
		{
			break;
		}
		NdisAllocatePacketPoolEx(&ndissta, &context->recvpacketpool, MIN_PACKET_POOL_SIZE, MAX_PACKET_POOL_SIZE - MIN_PACKET_POOL_SIZE, sizeof(RECV_PACKET_RSVD));
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
		NdisOpenAdapter(&ndissta, &errorcode, &global.bindinghandle[global.contextnum], &seletemediumundex, mediumarray, 4,
			global.protocolhandle, (NDIS_HANDLE)context, pDeviceName, 0, NULL);
		context->seletemediumundex = seletemediumundex;
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
	context->powerstatus = 1;
	NdisCloseAdapter(&sta, global.bindinghandle[num]);
	if (sta == NDIS_STATUS_PENDING)
	{
		DbgPrint("unbind pending\n");
		KeWaitForSingleObject(&context->unbindeve, Executive, KernelMode, FALSE, NULL);
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
	NDIS_STATUS ndissta;
	padapercontext context = ProtocolBindingContext;
	BOOLEAN reuse;
	PNDIS_PACKET packet = NULL;
	NdisIMGetCurrentPacketStack(pNdisPacket, &reuse);
	
	ZlzAnalysePacket(pNdisPacket,ZLZ_PACKET_RECEIVE);
	if (reuse)
	{
		ndissta = NDIS_GET_PACKET_STATUS(pNdisPacket);
		NdisMIndicateReceivePacket(context->relateminiporthandle, &packet, 1);
		return ((ndissta != NDIS_STATUS_RESOURCES) ? 1 : 0);
	}
	else
	{
		NdisAllocatePacket(&ndissta, &packet, context->recvpacketpool);
		PRECV_PACKET_RSVD recv;
		recv = (PRECV_PACKET_RSVD)packet->ProtocolReserved;
		recv->pOriginalPacket = pNdisPacket;
		NDIS_PACKET_FIRST_NDIS_BUFFER(packet) = NDIS_PACKET_FIRST_NDIS_BUFFER(pNdisPacket);
		NDIS_PACKET_LAST_NDIS_BUFFER(packet) = NDIS_PACKET_LAST_NDIS_BUFFER(pNdisPacket);
		ndissta = NDIS_GET_PACKET_STATUS(pNdisPacket);
		NDIS_SET_ORIGINAL_PACKET(packet, NDIS_GET_ORIGINAL_PACKET(pNdisPacket));
		packet->Private.Flags = pNdisPacket->Private.Flags;
		NDIS_SET_PACKET_STATUS(packet, ndissta);
		NdisMIndicateReceivePacket(context->relateminiporthandle, &packet, 1);
		if (ndissta == NDIS_STATUS_RESOURCES)
		{
			NdisFreePacket(packet);
		}
		return((ndissta != NDIS_STATUS_RESOURCES) ? 1 : 0);
	}
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
NDIS_STATUS ZlzAnalysePacket(PNDIS_PACKET Packet, enum PacketType type)
{
	checkcount();
	ippackinfo info;
	PUCHAR buffer;
	NdisAllocateMemoryWithTag(&buffer, 2048, 0);
	NdisZeroMemory(buffer, 2048);
	UINT phybufcount;
	UINT bufcount;
	PNDIS_BUFFER buf;
	UINT totallength;
	NdisQueryPacket(Packet, &phybufcount, &bufcount, &buf, &totallength);
	DbgPrint("\n\n---------------------------------------------\n");
	DbgPrint("\t\t\tPacket Analysis Information\n");
	DbgPrint("---------------------------------------------\n");
	DbgPrint("Packet Total length:%d\n", totallength);
	UINT dataoffset = 0;
	PVOID tempbuffer;
	UINT buffersize;
	while (1)
	{
		NdisQueryBufferSafe(buf, &tempbuffer, &buffersize, NormalPagePriority);
		NdisMoveMemory(buffer + dataoffset, tempbuffer, buffersize);
		dataoffset += buffersize;
		NdisGetNextBuffer(buf, &buf);
		if (buf == NULL)
		{
			break;
		}
	}
	if (type == ZLZ_PACKET_RECEIVE)
	{
		info.issend = FALSE;
	}
	else
	{
		info.issend = TRUE;
	}
	ETHeader *eth = (ETHeader*)buffer;
	switch (Tranverse16(eth->type))
	{
	case 0x0800:
		DbgPrint("Packet type:IP\n");
		break;
	case 0x0806:
		DbgPrint("Packet type:ARP\n");
		break;
	case 0x0835:
		DbgPrint("Packet type:RARP\n");
		break;
	default:
		DbgPrint("Packet type:Unknown\n");
			break;
	}
	info.type = Tranverse16(eth->type);
	DbgPrint("source mac:%x-%x-%x-%x-%x-%x\n", eth->shost[0], eth->shost[1], eth->shost[2],
		eth->shost[3], eth->shost[4], eth->shost[5]);
	DbgPrint("dest mac:%x-%x-%x-%x-%x-%x\n", eth->dhost[0], eth->dhost[1], eth->dhost[2],
		eth->dhost[3], eth->dhost[4], eth->dhost[5]);
	info.sourcemac[0] = eth->shost[0];
	info.sourcemac[1] = eth->shost[1];
	info.sourcemac[2] = eth->shost[2];
	info.sourcemac[3] = eth->shost[3];
	info.sourcemac[4] = eth->shost[4];
	info.sourcemac[5] = eth->shost[5];
	info.destmac[0] = eth->dhost[0];
	info.destmac[1] = eth->dhost[1];
	info.destmac[2] = eth->dhost[2];
	info.destmac[3] = eth->dhost[3];
	info.destmac[4] = eth->dhost[4];
	info.destmac[5] = eth->dhost[5];
	if (Tranverse16(eth->type)==0x0800)
	{
		ZlzAnalysisIpPacket(buffer, &info);
	}
	else
	{

	}
	DbgPrint("\n---------------------------------------------\n");
	DbgPrint("\t\t\tPacket Analysis End\n");
	DbgPrint("---------------------------------------------\n\n");
	NdisFreeMemory(buffer, 2048, 0);
	info.timestamp = KeQuerySystemTime(&info.timestamp);
	globalinfopool.packet[globalinfopool.count] = info;
	globalinfopool.count++;
	return STATUS_SUCCESS;
}
VOID ZlzAnalysisIpPacket(UCHAR *buf, pippackinfo info)
{
	IpPacket *ippack = (pIpPacket)(buf + sizeof(ETHeader));
	switch (ippack->ipProtocol)
	{
		case 0x01:
			DbgPrint("Protocol Type:ICMP\n");
			break;
		case 0x02:
			DbgPrint("Protocol Type:IGMP\n");
			break;
		case 0x06:
			DbgPrint("Protocol Type:TCP\n");
			break;
		case 0x11:
			DbgPrint("Protocol Type:UDP\n");
			break;
	default:
		DbgPrint("Protocol Type:Unknown\n");
		break;
	}
	info->Protocol = ippack->ipProtocol;
	UCHAR *sourceip = (UCHAR *)&ippack->ipSource;
	UCHAR *destip = (UCHAR *)&ippack->ipDestination;
	info->ipSource = ippack->ipSource;
	info->ipDestination = ippack->ipDestination;
	DbgPrint("source ip:%d.%d.%d.%d\n", sourceip[0], sourceip[1], sourceip[2], sourceip[3]);
	DbgPrint("dest ip:%d.%d.%d.%d\n", destip[0], destip[1], destip[2], destip[3]);
	DbgPrint("TTL:%d\n", ippack->ipTTL);
}
VOID checkcount()
{
	if (globalinfopool.count == 100)
	{
		globalinfopool.count = 0;
	}
}
