#pragma once
#include"struct.h"
FILTER_SET_OPTIONS FilterSetOptions;
FILTER_SET_MODULE_OPTIONS FilterSetModuleOptions;
FILTER_ATTACH FilterAttach;
FILTER_DETACH FilterDetach;
FILTER_RESTART FilterRestart;
FILTER_PAUSE FilterPause;
FILTER_SEND_NET_BUFFER_LISTS FilterSendNetBufferLists;
FILTER_SEND_NET_BUFFER_LISTS_COMPLETE FilterSendNetBufferListsComplete;
FILTER_CANCEL_SEND_NET_BUFFER_LISTS FilterCancelSendNetBufferLists;
FILTER_RECEIVE_NET_BUFFER_LISTS FilterReceiveNetBufferLists;
FILTER_RETURN_NET_BUFFER_LISTS FilterReturnNetBufferLists;
FILTER_NET_PNP_EVENT FilterNetPnPEvent;


_Use_decl_annotations_
NDIS_STATUS FilterNetPnPEvent(
	_In_ NDIS_HANDLE                 FilterModuleContext,
	_In_ PNET_PNP_EVENT_NOTIFICATION NetPnPEvent
)
{
	DbgBreakPoint();
	DbgPrint("NetEvent:0x%x\n", NetPnPEvent->NetPnPEvent.NetEvent);
	if (NetPnPEvent->NetPnPEvent.NetEvent == NetEventRestart)
	{
		PNDIS_PROTOCOL_RESTART_PARAMETERS para = (PNDIS_PROTOCOL_RESTART_PARAMETERS)NetPnPEvent->NetPnPEvent.Buffer;
		para;
		DbgPrint("Ready to runing\n");
	}
	return NDIS_STATUS_SUCCESS;
}

_Use_decl_annotations_
VOID FilterReturnNetBufferLists(
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ ULONG            ReturnFlags
)
{
	DbgBreakPoint();
}

_Use_decl_annotations_
VOID FilterReceiveNetBufferLists(
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ NDIS_PORT_NUMBER PortNumber,
	_In_ ULONG            NumberOfNetBufferLists,
	_In_ ULONG            ReceiveFlags
)
{
	DbgBreakPoint();
}

_Use_decl_annotations_
VOID FilterCancelSendNetBufferLists(
	_In_ NDIS_HANDLE FilterModuleContext,
	_In_ PVOID       CancelId
)
{
	DbgBreakPoint();
}

_Use_decl_annotations_
VOID FilterSendNetBufferListsComplete(
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ ULONG            SendCompleteFlags
)
{
	DbgBreakPoint();
}

_Use_decl_annotations_
VOID FilterSendNetBufferLists(
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ NDIS_PORT_NUMBER PortNumber,
	_In_ ULONG            SendFlags
)
{
	DbgBreakPoint();
}

_Use_decl_annotations_
NDIS_STATUS FilterPause(
	_In_ NDIS_HANDLE                   FilterModuleContext,
	_In_ PNDIS_FILTER_PAUSE_PARAMETERS FilterPauseParameters
)
{
	DbgBreakPoint();
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
NDIS_STATUS FilterRestart(
	_In_ NDIS_HANDLE                     FilterModuleContext,
	_In_ PNDIS_FILTER_RESTART_PARAMETERS FilterRestartParameters
)
{
	DbgBreakPoint();
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
VOID FilterDetach(
	_In_ NDIS_HANDLE FilterModuleContext
)
{
	DbgBreakPoint();
}

_Use_decl_annotations_
NDIS_STATUS FilterAttach(
	_In_ NDIS_HANDLE                    NdisFilterHandle,
	_In_ NDIS_HANDLE                    FilterDriverContext,
	_In_ PNDIS_FILTER_ATTACH_PARAMETERS AttachParameters
)
{
	DbgBreakPoint();
	NDIS_STATUS sta;
	NDIS_FILTER_ATTRIBUTES FilterAttributes;
	DbgPrint("BaseMiniportName:%wZ\n", AttachParameters->BaseMiniportName);
	DbgPrint("BaseMiniportInstanceName:%wZ\n", AttachParameters->BaseMiniportInstanceName);
	PFILTER_CONTEXT context;
	NdisAllocateMemoryWithTag(&context, sizeof(PFILTER_CONTEXT), 0);
	memmove(context->magic, "zlzndis",sizeof(context->magic));
	NET_BUFFER_LIST_POOL_PARAMETERS para;
	para.Header.Size = NDIS_SIZEOF_NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1;
	para.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
	para.Header.Revision = NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1;
	para.ProtocolId = 0;
	para.fAllocateNetBuffer = TRUE;
	para.ContextSize = 0;
	para.DataSize = 0;
	para.PoolTag = 0;
	NDIS_HANDLE PoolHandle=NdisAllocateNetBufferListPool(NULL, &para);

	NdisZeroMemory(&FilterAttributes, sizeof(NDIS_FILTER_ATTRIBUTES));
	FilterAttributes.Header.Revision = NDIS_FILTER_ATTRIBUTES_REVISION_1;
	FilterAttributes.Header.Size = sizeof(NDIS_FILTER_ATTRIBUTES);
	FilterAttributes.Header.Type = NDIS_OBJECT_TYPE_FILTER_ATTRIBUTES;
	FilterAttributes.Flags = 0;

	sta = NdisFSetAttributes(NdisFilterHandle,
		context,
		&FilterAttributes);
	context->NetBufferPool = PoolHandle;
	context->FilterHandle = NdisFilterHandle;
	Global.context[Global.contextnum] = context;
	Global.contextnum++;
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
NDIS_STATUS FilterSetOptions(
	_In_ NDIS_HANDLE NdisDriverHandle,
	_In_ NDIS_HANDLE DriverContext
)
{
	DbgBreakPoint();
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
NDIS_STATUS FilterSetModuleOptions(
	_In_ NDIS_HANDLE FilterModuleContext
)
{
	DbgBreakPoint();
	return STATUS_SUCCESS;
}

