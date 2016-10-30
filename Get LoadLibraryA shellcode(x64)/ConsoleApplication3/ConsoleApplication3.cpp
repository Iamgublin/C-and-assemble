// ConsoleApplication3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include<Windows.h>
/*
cld
push rbx
push rax
push rcx
mov rbx,gs:[60h]            ;get peb   (x86 在FS寄存器，x64在GS寄存器)
mov rbx,[rbx+18h]           ;get ldr
mov rbx,[rbx+30h]           ;get modulelistfirst ntdll.dll
mov rbx,[rbx]               ;get kernelbase.dll
mov rbx,[rbx]               ;get kernel32.dll
mov rax,[rbx+10h]           ;get baseaddress
mov rcx,rax
mov rbx,[rbx+40h]           ;get basedllname
xor rbx,rbx
mov ebx,[rax+3ch]           ;get e_lfanew
add rax,rbx                 ;get PE header
add rax,88h                 ;get export table RVA address
xor rbx,rbx
mov ebx,[rax]               ;get export table RVA
mov rax,rcx
add rax,rbx                 ;locate in the export table
xor rbx,rbx
mov ebx,[rax+1ch]           ;get RVA of AddressOfFunctions
mov rax,rcx
add rax,rbx                 ;get AddressOfFunction array
xor rbx,rbx
mov ebx,[rax+936*4]         ;get LoadLibraryA Function Rva   (No.937)
mov rax,rcx
add rax,rbx                 ;get LoadLibraryA address
xor rbx,rbx
mov rbx,006c6c642e31h         ;1.dll
sub rsp,10h
mov [rsp],rbx               ;char* a="1.dll"
mov rcx,rsp                 ;__fastcall
call rax                    ;LoadLibraryA("1.dll")
add rsp,10h
pop rcx
pop rax
pop rbx
ret
*/
UCHAR shellcode[126] =
"\xFC"                                     //0
"\x53"                                     //1
"\x50"                                     //2
"\x51"                                     //3
"\x65\x48\x8B\x1C\x25\x60\x00\x00\x00"     //12
"\x48\x8B\x5B\x18"                         //16
"\x48\x8B\x5B\x30"                         //20
"\x48\x8B\x1B"                             //23
"\x48\x8B\x1B"                             //26
"\x48\x8B\x43\x10"                         //30
"\x48\x8B\xC8"                             //33
"\x48\x8B\x5B\x40"                         //37
"\x48\x33\xDB"                             //40
"\x8B\x58\x3C"                             //43
"\x48\x03\xC3"                             //46
"\x48\x05\x88\x00\x00\x00"                 //52
"\x48\x33\xDB"                             //55
"\x8B\x18"                                 //57
"\x48\x8B\xC1"                             //60
"\x48\x03\xC3"                             //63
"\x48\x33\xDB"                             //66
"\x8B\x58\x1C"                             //69
"\x48\x8B\xC1"                             //72
"\x48\x03\xC3"                             //75
"\x48\x33\xDB"                             //78
"\x8B\x98\xA0\x0E\x00\x00"                 //84
"\x48\x8B\xC1"                             //87
"\x48\x03\xC3"                             //90
"\x48\x33\xDB"                             //93
"\x48\xBB\x31\x2E\x64\x6C\x6C\x00\x00\x00" //103
"\x48\x83\xEC\x10"                         //107
"\x48\x89\x1C\x24"                         //111
"\x48\x8B\xCC"                             //114
"\xFF\xD0"                                 //116
"\x48\x83\xC4\x10"                         //120
"\x59"                                     //121
"\x58"                                     //122
"\x5B"                                     //123
"\xC3";                                    //124


typedef void(*CODE)(void);
int main()
{
	/*printf("%x", (unsigned long)&((IMAGE_DOS_HEADER*)(0))->e_lfanew - (unsigned long)&((IMAGE_DOS_HEADER*)(0))->e_magic);*/
	/*printf("%x", (unsigned long)&((IMAGE_EXPORT_DIRECTORY *)(0))->AddressOfFunctions - (unsigned long)&((IMAGE_EXPORT_DIRECTORY *)(0))->Characteristics);*/
    CODE code;
	PVOID buf = VirtualAlloc(NULL, 1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(buf, shellcode,sizeof(shellcode));
	code = (CODE)buf;
	code();
}