#pragma warning(push)
#pragma warning(disable: 4201)
#pragma warning(disable: 4324)
#define NDIS_SUPPORT_NDIS6 1
#define HTTP_DEFAULT_PORT 80
#define INITGUID
#include<guiddef.h>
#include <ntifs.h>
#include <ntddk.h>
#include <fwpsk.h>
#include <fwpmk.h>
#pragma warning(pop)
// {51E6E6DC-96D5-4AA5-8E76-6F8E892D8B01}
static const GUID FCSGUID =
{ 0x51e6e6dc, 0x96d5, 0x4aa5, { 0x8e, 0x76, 0x6f, 0x8e, 0x89, 0x2d, 0x8b, 0x1 } };
// {37EA3302-CE14-4052-A57B-6BB2971578ED}
static const GUID SUBGUID =
{ 0x37ea3302, 0xce14, 0x4052, { 0xa5, 0x7b, 0x6b, 0xb2, 0x97, 0x15, 0x78, 0xed } };

UINT32 fcmid;
UINT64 ffid;
HANDLE engine = NULL;
UINT32 calloutid;
PDEVICE_OBJECT dev;
VOID NTAPI Wfp_Sample_Established_ClassifyFn_V4(
	IN const FWPS_INCOMING_VALUES  *inFixedValues,
	IN const FWPS_INCOMING_METADATA_VALUES  *inMetaValues,
	IN OUT VOID  *layerData,
	IN OPTIONAL const void  *classifyContext,
	IN const FWPS_FILTER  *filter,
	IN UINT64  flowContext,
	OUT FWPS_CLASSIFY_OUT  *classifyOut
	);
VOID NTAPI Wfp_Sample_Established_FlowDeleteFn_V4(
	IN UINT16  layerId,
	IN UINT32  calloutId,
	IN UINT64  flowContext
	);
NTSTATUS NTAPI Wfp_Sample_Established_NotifyFn_V4(
	IN  FWPS_CALLOUT_NOTIFY_TYPE        notifyType,
	IN  const GUID*             filterKey,
	IN  const FWPS_FILTER*     filter);
NTSTATUS unload(PDRIVER_OBJECT driver);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(str);
	NTSTATUS sta;
	FWPM_SESSION Session = { 0 };
	driver->DriverUnload = unload;


	sta = FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &Session, &engine);
	DbgPrint("0x%x\n", engine);
	sta = IoCreateDevice(driver, 0, NULL, FILE_DEVICE_UNKNOWN, 0, FALSE, &dev);
	DbgPrint("0x%x\n", sta);
	dev->Flags &= ~DO_DEVICE_INITIALIZING;
	SetFlag(dev->Flags, DO_BUFFERED_IO);
	FWPS_CALLOUT fcs;
	memset(&fcs, 0, sizeof(FWPS_CALLOUT));
	fcs.calloutKey = FCSGUID;
	fcs.flags = 0;
	fcs.classifyFn = Wfp_Sample_Established_ClassifyFn_V4;
	fcs.notifyFn = Wfp_Sample_Established_NotifyFn_V4;
	fcs.flowDeleteFn = Wfp_Sample_Established_FlowDeleteFn_V4;
	sta = FwpsCalloutRegister(dev, &fcs, &calloutid);
	DbgPrint("0x%x\n", sta);


	FWPM_CALLOUT fcm = { 0 };
	fcm.displayData.name = L"callout fli";
	fcm.displayData.description = L"callout";
	fcm.flags = 0;
	fcm.calloutKey = FCSGUID;
	fcm.applicableLayer = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4;
	sta = FwpmCalloutAdd(engine, &fcm, NULL, &fcmid);
	DbgPrint("0x%x\n", sta);

	FWPM_SUBLAYER fs = { 0 };
	fs.flags = 0;
	fs.displayData.description = L"sublayer fliter";
	fs.displayData.name = L"sublayer";
	fs.subLayerKey = SUBGUID;
	fs.weight = 65535;
	sta = FwpmSubLayerAdd(engine, &fs, NULL);
	DbgPrint("0x%x\n", sta);

	FWPM_FILTER ff = { 0 };
	FWPM_FILTER_CONDITION flitercondition[1] = { 0 };
	FWP_V4_ADDR_AND_MASK addrandmask = { 0 };
	ff.displayData.description = L"fliter qi";
	ff.displayData.name = L"fliter";
	ff.flags = 0;
	ff.layerKey = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4;
	ff.subLayerKey = SUBGUID;
	ff.weight.type = FWP_EMPTY;
	ff.numFilterConditions = 1;
	ff.filterCondition = flitercondition;
	ff.action.type = FWP_ACTION_CALLOUT_TERMINATING;
	ff.action.calloutKey = FCSGUID;
	flitercondition->fieldKey = FWPM_CONDITION_IP_REMOTE_ADDRESS;
	flitercondition->matchType = FWP_MATCH_EQUAL;
	flitercondition->conditionValue.type = FWP_V4_ADDR_MASK;
	flitercondition->conditionValue.v4AddrMask = &addrandmask;
	sta = FwpmFilterAdd(engine, &ff, NULL, &ffid);
	DbgPrint("0x%x\n", sta);
	return STATUS_SUCCESS;
}
VOID NTAPI Wfp_Sample_Established_ClassifyFn_V4(
	IN const FWPS_INCOMING_VALUES  *inFixedValues,
	IN const FWPS_INCOMING_METADATA_VALUES  *inMetaValues,
	IN OUT VOID  *layerData,
	IN OPTIONAL const void  *classifyContext,
	IN const FWPS_FILTER  *filter,
	IN UINT64  flowContext,
	OUT FWPS_CLASSIFY_OUT  *classifyOut
	)
{
	UNREFERENCED_PARAMETER(layerData);
	UNREFERENCED_PARAMETER(inMetaValues);
	UNREFERENCED_PARAMETER(flowContext);
	UNREFERENCED_PARAMETER(classifyContext);
	USHORT wDirection = 0;
	WORD wRemotePort = 0;
	WORD wProtocol = 0;
	if (!(classifyOut->rights&FWPS_RIGHT_ACTION_WRITE))
	{
		return;
	}
	wDirection = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_DIRECTION].value.int8;
	wRemotePort = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT].value.uint16;
	wProtocol = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_PROTOCOL].value.uint16;
	classifyOut->actionType = FWP_ACTION_PERMIT;
	DbgPrint("dir:%s\n", (wDirection == 1) ? "inbound" : "outbound");
	DbgPrint("protocol:%d\n", wProtocol);
	DbgPrint("port:%d\n", wRemotePort);
	if ((wProtocol == IPPROTO_TCP) &&
		(wDirection == FWP_DIRECTION_OUTBOUND) &&
		(wRemotePort == HTTP_DEFAULT_PORT))
	{
	}
	if (filter->flags&FWPS_FILTER_FLAG_CLEAR_ACTION_RIGHT)
	{
		classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
	}
}
VOID NTAPI Wfp_Sample_Established_FlowDeleteFn_V4(
	IN UINT16  layerId,
	IN UINT32  calloutId,
	IN UINT64  flowContext
	)
{
	UNREFERENCED_PARAMETER(layerId);
	UNREFERENCED_PARAMETER(calloutId);
	UNREFERENCED_PARAMETER(flowContext);
}
NTSTATUS NTAPI Wfp_Sample_Established_NotifyFn_V4(
	IN  FWPS_CALLOUT_NOTIFY_TYPE        notifyType,
	IN  const GUID*             filterKey,
	IN  const FWPS_FILTER*     filter)
{
	UNREFERENCED_PARAMETER(notifyType);
	UNREFERENCED_PARAMETER(filterKey);
	UNREFERENCED_PARAMETER(filter);
	return STATUS_SUCCESS;
}
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	FwpmFilterDeleteById(engine, ffid);
	FwpmCalloutDeleteById(engine, fcmid);
	FwpmSubLayerDeleteByKey(engine, &SUBGUID);
	FwpsCalloutUnregisterById(calloutid);
	IoDeleteDevice(dev);
	return STATUS_SUCCESS;
}