#include<ntifs.h>
#include<wdm.h>
#include<windef.h>
#pragma warning(disable:4305)
#pragma warning(disable:4152)
typedef NTSTATUS(__stdcall *OLDCREATEFILE)(
	_Out_    PHANDLE            FileHandle,
	_In_     ACCESS_MASK        DesiredAccess,
	_In_     POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_    PIO_STATUS_BLOCK   IoStatusBlock,
	_In_opt_ PLARGE_INTEGER     AllocationSize,
	_In_     ULONG              FileAttributes,
	_In_     ULONG              ShareAccess,
	_In_     ULONG              CreateDisposition,
	_In_     ULONG              CreateOptions,
	_In_opt_ PVOID              EaBuffer,
	_In_     ULONG              EaLength
	);
OLDCREATEFILE oldfunction=NULL;
ULONG service_id;
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase;
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;

// µ¼ÈëSSDTµÄ·ûºÅ
__declspec(dllimport)  ServiceDescriptorTableEntry_t KeServiceDescriptorTable;
VOID closeinterrupt();
VOID startinterrupt();
void* hookssdt(void* hookfunc, void* myfunc);
void disablewriteprotect(ULONG* old);
void enablewriteprotect(ULONG old);
NTSTATUS ZlzHookfunction(
	_Out_    PHANDLE            FileHandle,
	_In_     ACCESS_MASK        DesiredAccess,
	_In_     POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_    PIO_STATUS_BLOCK   IoStatusBlock,
	_In_opt_ PLARGE_INTEGER     AllocationSize,
	_In_     ULONG              FileAttributes,
	_In_     ULONG              ShareAccess,
	_In_     ULONG              CreateDisposition,
	_In_     ULONG              CreateOptions,
	_In_opt_ PVOID              EaBuffer,
	_In_     ULONG              EaLength
	);
NTSTATUS unload(PDRIVER_OBJECT driver)
{
	ULONG temp;
	UNREFERENCED_PARAMETER(driver);
	DbgBreakPoint();
	disablewriteprotect(&temp);
	KeServiceDescriptorTable.ServiceTableBase[service_id] = (unsigned int)oldfunction;
	enablewriteprotect(temp);
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	driver->DriverUnload = unload;
	UNICODE_STRING us = RTL_CONSTANT_STRING(L"ZwCreateFile");
	PVOID address=MmGetSystemRoutineAddress(&us);
	DbgPrint("0x%p\n", address);
	DbgPrint("SSDT BASE:0x%p\n", KeServiceDescriptorTable.ServiceTableBase);
	DbgPrint("SSDT function number:%d\n", KeServiceDescriptorTable.NumberOfServices);
	hookssdt(address, ZlzHookfunction);
	return STATUS_SUCCESS;
}
NTSTATUS ZlzHookfunction(
	_Out_    PHANDLE            FileHandle,
	_In_     ACCESS_MASK        DesiredAccess,
	_In_     POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_    PIO_STATUS_BLOCK   IoStatusBlock,
	_In_opt_ PLARGE_INTEGER     AllocationSize,
	_In_     ULONG              FileAttributes,
	_In_     ULONG              ShareAccess,
	_In_     ULONG              CreateDisposition,
	_In_     ULONG              CreateOptions,
	_In_opt_ PVOID              EaBuffer,
	_In_     ULONG              EaLength
	)
{
	DbgPrint("enter the hook\n");
	DbgPrint("name:%wZ\n", ObjectAttributes->ObjectName);
	return oldfunction(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess,
		CreateDisposition, CreateOptions, EaBuffer, EaLength);
}
void* hookssdt(void* hookfunc,void* myfunc)
{
	ULONG temp;
	service_id = *(PULONG)((PUCHAR)(hookfunc)+1);
	oldfunction = (void*)KeServiceDescriptorTable.ServiceTableBase[service_id];
	disablewriteprotect(&temp);
	KeServiceDescriptorTable.ServiceTableBase[service_id] = (unsigned int)myfunc;
	enablewriteprotect(temp);
	return 0;
}
void disablewriteprotect(ULONG* old)
{
	ULONG cr0_old;
	_asm
	{
		cli
			mov eax, cr0
			mov cr0_old, eax
			and eax, 0xfffeffff
			mov cr0, eax
	};
	*old = cr0_old;
}
void enablewriteprotect(ULONG old)
{
	_asm
	{
		mov eax,old
		mov cr0,eax
		sti
	}
}
