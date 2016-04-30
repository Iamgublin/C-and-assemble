#include<wdm.h>
#pragma warning(disable:4055)
#pragma warning(disable:4152)
#pragma warning(disable:4214)
UCHAR shellcode[8] =
"\x48\x8B\xC1" //2
"\x0F\x01\x00" //5
"\xC3";        //6
#pragma pack(push,1)
typedef struct
{
	unsigned short table_limit;
	long long table_base;
}IA32_SYS_TR, *PIA32_SYS_TR;
#pragma pack(pop)
#pragma pack(push,1)
typedef struct
{
	unsigned short seglimit1;
	unsigned short baseaddress1;
	unsigned char baseaddress2;
	UCHAR type : 4;
	UCHAR s : 1;
	UCHAR dpl : 2;
	UCHAR p : 1;
	UCHAR seglimit2 : 4;
	UCHAR avl : 1;
	UCHAR l : 1;
	UCHAR DB : 1;
	UCHAR G : 1;
	unsigned char baseaddress3;
}GDT,*PGDT;
#pragma pack(pop)
typedef VOID(*shell)(PIA32_SYS_TR);
NTSTATUS unload(PDRIVER_OBJECT dri)
{
	UNREFERENCED_PARAMETER(dri);
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	driver->DriverUnload = unload;
	IA32_SYS_TR slbk;
	shell function;
	function = (shell)ExAllocatePool(NonPagedPool, 8);
	memcpy(function, shellcode, 8);
	function(&slbk);
	int num = (slbk.table_limit + 1) / 8;
	DbgPrint("table limit:%d\n", slbk.table_limit);
	DbgPrint("table base:0x%p\n", slbk.table_base);
	DbgPrint("GDT descriptor number:%d\n",num);
	PGDT gdt = (PGDT)slbk.table_base;
	for (int i = 0; i < num; i++)
	{
		DbgPrint("DPL:%x\n", gdt->dpl);
		DbgPrint("limit:%x%x\n", gdt->seglimit2, gdt->seglimit1);
		gdt++;
	}
	ExFreePool(function);
	return STATUS_SUCCESS;
}

