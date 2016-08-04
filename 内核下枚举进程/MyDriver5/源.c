#include<wdm.h>
#pragma warning(disable:4100)
#define EPROCESS_ActiveProcessLinks  0x2e8
#define EPROCESS_ImageFileName  0x438 
NTSTATUS unload(PDRIVER_OBJECT driver);
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	PEPROCESS handprocess = IoGetCurrentProcess();
	PLIST_ENTRY activeprocess = (PLIST_ENTRY)((UINT64)handprocess + EPROCESS_ActiveProcessLinks);
	PLIST_ENTRY temp = activeprocess->Flink;
	for (PLIST_ENTRY activeprocess1 = temp; activeprocess1 != activeprocess; activeprocess1 = activeprocess1->Flink)
	{
		PEPROCESS handpro = (PEPROCESS)((UINT64)activeprocess1 - EPROCESS_ActiveProcessLinks);
		UCHAR *processname = (UCHAR*)((UINT64)handpro + EPROCESS_ImageFileName);
		DbgPrint("%s\n", processname);
	}
	driver->DriverUnload = unload;
	return STATUS_SUCCESS;
}
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	return STATUS_SUCCESS;
}