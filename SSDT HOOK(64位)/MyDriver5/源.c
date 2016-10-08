#include<ntifs.h>
#include<wdm.h>
#include<windef.h>
#pragma warning(disable:4305)
#pragma warning(disable:4152)
/*cli
cli
mov rax,cr0
mov rcx,rax
and eax,0fffeffffh
mov cr0,rax
mov rax,rcx
ret

*/
UCHAR shellcode[20] =
"\xFA"                 //0
"\x0F\x20\xC0"         //3
"\x48\x8B\xC8"         //6
"\x25\xFF\xFF\xFE\xFF" //11
"\x0F\x22\xC0"         //14
"\x48\x8B\xC1"         //17
"\xC3";                //18
/*
mov cr0,rcx
sti
ret
*/
UCHAR shellcode1[6] =
"\x0F\x22\xC1" //2
"\xFB"         //3
"\xC3";        //4


/*
mov rax,0ffffffffffffffffh
jmp rax
*/
UCHAR shellcode3[13] =
"\x48\xB8\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" //9
"\xFF\xE0";                                //11
typedef ULONG (*CLOSEINTERUPT)();
typedef VOID (*OPENINTERUPT)(ULONG old_cr0);
typedef NTSTATUS(__fastcall *OLDCREATEFILE)(                         //一定要是fastcall，否则会出问题！
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
OLDCREATEFILE  old_func;
typedef struct ServiceDescriptorEntry {
	INT64 *ServiceTableBase;
	INT64 *ServiceCounterTableBase;
	INT64 NumberOfServices;
	INT64 *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
ULONGLONG GetKeServiceDescriptorTable64();
int findserviceid(PVOID address);
ULONG getaddressoffset(PServiceDescriptorTableEntry_t enteraddress,INT64 address);
PVOID changessdt(PServiceDescriptorTableEntry_t address, int service_id);
ULONG closeinterupt();
void startinteript(ULONG old_cr0);
NTSTATUS __fastcall myfunc(
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
VOID resetessdt(PServiceDescriptorTableEntry_t address, int service_id);

PServiceDescriptorTableEntry_t ent;

NTSTATUS unload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	UNICODE_STRING func = RTL_CONSTANT_STRING(L"ZwCreateFile");
	PVOID funcaddress = MmGetSystemRoutineAddress(&func);
	resetessdt(ent, findserviceid(funcaddress));
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	driver->DriverUnload = unload;
	UNICODE_STRING func = RTL_CONSTANT_STRING(L"ZwCreateFile");
	PVOID funcaddress = MmGetSystemRoutineAddress(&func);
	int service_id = findserviceid(funcaddress);
	DbgPrint("serviceid:%x\n", service_id);
	ULONGLONG address = GetKeServiceDescriptorTable64();
	DbgPrint("0x%p\n", address);
	ent = (PServiceDescriptorTableEntry_t)address;
	DbgPrint("SSDT enetry:0x%p\n", ent->ServiceTableBase);
	DbgPrint("number of service:%d\n", ent->NumberOfServices);
	changessdt(ent, service_id);
	return STATUS_SUCCESS;
}
ULONGLONG GetKeServiceDescriptorTable64()
{
	PUCHAR StatrtSearchAddr = (PUCHAR)__readmsr(0xc0000082);
	PUCHAR EndSearchAddr = StatrtSearchAddr + 0x500;
	PUCHAR i = NULL;
	UCHAR b1 = 0, b2 = 0, b3 = 0;
	ULONG templong = 0;
	ULONGLONG addr = 0;
	
	for (i = StatrtSearchAddr; i < EndSearchAddr; i++)
	{
		if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 2))
		{
			b1 = *i;
			b2 = *(i + 1);
			b3 = *(i + 2);

			//fffff800`01ad0772 4c8d15c7202300  lea     r10,[nt!KeServiceDescriptorTable (fffff800`01d02840)]
			if (b1 == 0x4c && b2 == 0x8d && b3 == 0x15)
			{
				memcpy(&templong, i + 3, 4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
	return 0;
}
int findserviceid(PVOID address)
{
	UCHAR i1, i2=0;
	PUCHAR temp = address;
	for (; MmIsAddressValid(temp) && MmIsAddressValid(temp + 1); temp++)
	{
		i1 = *temp;
		i2 = *(temp + 1);
		if (i1 == 0xb8)  //硬编码
		{
			break;
		}
	}
	return i2;
}
PVOID changessdt(PServiceDescriptorTableEntry_t address, int service_id)
{
	ULONG old_cr0=0;
	ULONGLONG add;
	ULONGLONG ssdtentry = (ULONGLONG)address->ServiceTableBase;
	PULONG servicetableaddr = (PULONG)address->ServiceTableBase;
	LONG temp1 = servicetableaddr[service_id];
	temp1 = temp1 >> 4;
	add = ssdtentry + temp1;
	old_func = (OLDCREATEFILE)add;
	ULONGLONG myfuncaddress = (ULONGLONG)myfunc;
	DbgPrint("%p\n", myfuncaddress);
	memcpy(shellcode3 + 2, &myfuncaddress, 8);
	old_cr0 = closeinterupt();
	memcpy(KeBugCheckEx, shellcode3, 13);
	servicetableaddr[service_id] = getaddressoffset(address, (INT64)KeBugCheckEx);
	startinteript(old_cr0);
	return (PVOID)add;
}
VOID resetessdt(PServiceDescriptorTableEntry_t address, int service_id)
{
	ULONG old_cr0 = 0;
	PULONG servicetableaddr = (PULONG)address->ServiceTableBase;
	old_cr0 = closeinterupt();
	memcpy(KeBugCheckEx, shellcode3, 13);
	servicetableaddr[service_id] = (ULONG)((ULONGLONG)old_func - (ULONGLONG)address->ServiceTableBase);
	startinteript(old_cr0);
}
ULONG closeinterupt()
{
	ULONG old_cr0;
	CLOSEINTERUPT func = ExAllocatePool(NonPagedPool, 20);
	memcpy(func, shellcode, 20);
	old_cr0 = func();
	ExFreePool(func);
	return old_cr0;
}
void startinteript(ULONG old_cr0)
{
	OPENINTERUPT func = ExAllocatePool(NonPagedPool, 6);
	memcpy(func, shellcode1, 6);
	func(old_cr0);
	ExFreePool(func);
}
NTSTATUS __fastcall myfunc(
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
	return NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess,
		CreateDisposition, CreateOptions, EaBuffer, EaLength);
}
ULONG getaddressoffset(PServiceDescriptorTableEntry_t enteraddress, INT64 address)
{
	ULONG offset = (ULONG)(address - (INT64)enteraddress->ServiceTableBase);
	return offset << 4;
}