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
	NDIS_STATUS sta;
	DbgPrint("NetEvent:0x%x\n", NetPnPEvent->NetPnPEvent.NetEvent);
	if (NetPnPEvent->NetPnPEvent.NetEvent == NetEventRestart)
	{
		PNDIS_PROTOCOL_RESTART_PARAMETERS para = (PNDIS_PROTOCOL_RESTART_PARAMETERS)NetPnPEvent->NetPnPEvent.Buffer;
		para;
		DbgPrint("Ready to runing\n");
	}
	sta = NdisFNetPnPEvent(FilterModuleContext, NetPnPEvent);
	return sta;
}

_Use_decl_annotations_
VOID FilterReturnNetBufferLists(                               //返还小端口分配的接受NBL的使用权，由小端口并释放
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ ULONG            ReturnFlags
)
{
	/*DbgBreakPoint();*/
	PFILTER_CONTEXT context = FilterModuleContext;
	if (NdisGetPoolFromNetBufferList(NetBufferLists) != context->NetBufferPool)    //是协议层 小端口层分配的，dispatch该消息
	{
		NdisFReturnNetBufferLists(context->FilterHandle, NetBufferLists, ReturnFlags);
	}
}

_Use_decl_annotations_
VOID FilterReceiveNetBufferLists(  
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ NDIS_PORT_NUMBER PortNumber,
	_In_ ULONG            NumberOfNetBufferLists,
	_In_ ULONG            ReceiveFlags
)
//Note  NdisFReturnNetBufferLists should not be called for NBLs indicated with 
//NDIS_RECEIVE_FLAGS_RESOURCES flag set in a corresponding FilterReceiveNetBufferLists call.
//Such NBLs are returned to NDIS synchronously by returning from the FilterReceiveNetBufferLists routine.
{
	/*DbgBreakPoint();*/
	PFILTER_CONTEXT context = FilterModuleContext;
	if (!context->IsRunning)
	{
		ULONG               ReturnFlags;
		ReturnFlags = 0;
		if (NDIS_TEST_RECEIVE_AT_DISPATCH_LEVEL(ReceiveFlags))
		{
			NDIS_SET_RETURN_FLAG(ReturnFlags, NDIS_RETURN_FLAGS_DISPATCH_LEVEL);
		}
		if (!NDIS_TEST_RECEIVE_CANNOT_PEND(ReceiveFlags))     //return 就释放,不需要调用NdisFReturnNetBufferLists
		{
			NdisFReturnNetBufferLists(context->FilterHandle, NetBufferLists, ReturnFlags);
		}
		return;
	}
	NdisFIndicateReceiveNetBufferLists(context->FilterHandle, NetBufferLists, PortNumber, NumberOfNetBufferLists, ReceiveFlags);
}

_Use_decl_annotations_
VOID FilterCancelSendNetBufferLists(
	_In_ NDIS_HANDLE FilterModuleContext,
	_In_ PVOID       CancelId
)
{
	/*DbgBreakPoint();*/
	PFILTER_CONTEXT context = FilterModuleContext;
	NdisFCancelSendNetBufferLists(context->FilterHandle, CancelId);
}

_Use_decl_annotations_
VOID FilterSendNetBufferListsComplete(
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ ULONG            SendCompleteFlags
)
{
	/*DbgBreakPoint();*/
	PFILTER_CONTEXT context = FilterModuleContext;
	NdisFSendNetBufferListsComplete(context->FilterHandle, NetBufferLists, SendCompleteFlags);
}

_Use_decl_annotations_
VOID FilterSendNetBufferLists(
	_In_ NDIS_HANDLE      FilterModuleContext,
	_In_ PNET_BUFFER_LIST NetBufferLists,
	_In_ NDIS_PORT_NUMBER PortNumber,
	_In_ ULONG            SendFlags
)
{
	/*DbgBreakPoint();*/
	PFILTER_CONTEXT context = FilterModuleContext;
	if (!context->IsRunning)
	{
		ULONG SendCompleteFlags = 0;
		NDIS_SET_SEND_COMPLETE_FLAG(SendCompleteFlags, NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL);
		PNET_BUFFER_LIST temp = NetBufferLists;
		do                                           //设置所有包头状态为NDIS_STATUS_PAUSED
		{
			temp->Status = NDIS_STATUS_PAUSED;
			temp = temp->Next;
		} while (temp != NULL);
		NdisFSendNetBufferListsComplete(context->FilterHandle, NetBufferLists, SendCompleteFlags);
		return;
	}
	NdisFSendNetBufferLists(context->FilterHandle, NetBufferLists, PortNumber, SendFlags);
}

_Use_decl_annotations_
NDIS_STATUS FilterPause(
	_In_ NDIS_HANDLE                   FilterModuleContext,
	_In_ PNDIS_FILTER_PAUSE_PARAMETERS FilterPauseParameters
)
{
	DbgBreakPoint();
	PFILTER_CONTEXT context = FilterModuleContext;
	context->IsRunning = FALSE;
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
NDIS_STATUS FilterRestart(
	_In_ NDIS_HANDLE                     FilterModuleContext,
	_In_ PNDIS_FILTER_RESTART_PARAMETERS FilterRestartParameters
)
{
	DbgBreakPoint();
	PFILTER_CONTEXT context = FilterModuleContext;
	context->IsRunning = TRUE;
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
VOID FilterDetach(
	_In_ NDIS_HANDLE FilterModuleContext
)
{
	DbgBreakPoint();
	PFILTER_CONTEXT context = FilterModuleContext;
	NdisFreeNetBufferListPool(context->NetBufferPool);
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

	context->NetBufferPool = PoolHandle;
	context->FilterHandle = NdisFilterHandle;
	context->FliterIndex = Global.contextnum;
	sta = NdisFSetAttributes(NdisFilterHandle,
		context,
		&FilterAttributes);
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

_Use_decl_annotations_               //设置指定绑定设备的函数挂钩
NDIS_STATUS FilterSetModuleOptions(
	_In_ NDIS_HANDLE FilterModuleContext
)
{
	DbgBreakPoint();
	return STATUS_SUCCESS;
}

