#include<fltKernel.h>
#include<ntddk.h>
PFLT_FILTER pf = NULL;
NTSTATUS unload(PDRIVER_OBJECT driver){ UNREFERENCED_PARAMETER(driver); return STATUS_SUCCESS; }
NTSTATUS fltunload(FLT_FILTER_UNLOAD_FLAGS flag);
FLT_PREOP_CALLBACK_STATUS precreate(PFLT_CALLBACK_DATA Data,PCFLT_RELATED_OBJECTS FltObjects,PVOID *CompletionContext);
FLT_POSTOP_CALLBACK_STATUS postcreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext, FLT_POST_OPERATION_FLAGS flag);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	NTSTATUS sta;
	driver->DriverUnload = unload;
	CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
		{
			IRP_MJ_CREATE,
			0,
			precreate,
			postcreate
		},
		{
			IRP_MJ_OPERATION_END
		}
	};
	FLT_REGISTRATION fr;  
	fr.Size = sizeof(FLT_REGISTRATION);
	fr.ContextRegistration = 0;
	fr.FilterUnloadCallback = fltunload;
	fr.Flags = 0;
	fr.GenerateFileNameCallback = 0;
	fr.InstanceQueryTeardownCallback = 0;
	fr.InstanceSetupCallback = 0;
	fr.InstanceTeardownCompleteCallback = 0;
	fr.InstanceTeardownStartCallback = 0;
	fr.NormalizeContextCleanupCallback = 0;
	fr.NormalizeNameComponentCallback = 0;
	fr.NormalizeNameComponentExCallback = 0;
	fr.OperationRegistration = Callbacks;
	fr.SectionNotificationCallback = 0;
	fr.TransactionNotificationCallback = 0;
	fr.Version = FLT_REGISTRATION_VERSION;
	sta = FltRegisterFilter(driver, &fr, &pf);
	DbgPrint("0x%x\n", sta);
	if (NT_SUCCESS(sta))
	{
		DbgPrint("MINI驱动\t create by:zlz 五邑大学\n");
		DbgPrint("MINI驱动初始化成功，开始过滤\n");
		FltStartFiltering(pf);
	}
	return STATUS_SUCCESS;
}
NTSTATUS fltunload(FLT_FILTER_UNLOAD_FLAGS flag)
{
	UNREFERENCED_PARAMETER(flag);
	if (pf != NULL)
	{
		FltUnregisterFilter(pf);
	}
	return STATUS_SUCCESS;
}
FLT_PREOP_CALLBACK_STATUS precreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	PFLT_FILE_NAME_INFORMATION info;
	PAGED_CODE();
	__try{
		FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &info);
		FltParseFileNameInformation(info);
		DbgPrint("检测到目标打开文件:%wZ\n", info->Name);
		FltReleaseFileNameInformation(info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("error cant execute the try\n");
	}
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}
FLT_POSTOP_CALLBACK_STATUS postcreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext, FLT_POST_OPERATION_FLAGS flag)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(flag);
	DbgPrint("postcreate\n");
	return FLT_POSTOP_FINISHED_PROCESSING;
}