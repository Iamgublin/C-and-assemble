// ConsoleApplication3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
UCHAR shellcode[16] = {
	0xB8, 0x11, 0x00, 0x00, 0x00, 0x83, 0xC3, 0x22, 0x83, 0xE9, 0x33, 0xF7, 0xF2, 0x0F, 0xA2, 0xC3
};

typedef void(*CODE)(void);
int _tmain(int argc, _TCHAR* argv[])
{
	__asm
	{
		pushfd;
		mov eax, fs:[0x30];                    ; get PEB 
		mov eax, [eax + 0x0c];                 ; get LDR
		mov eax, [eax + 0x1c];                 ; get modulelistfirst
		mov eax, [eax];                        ; get ntdll.dll
		mov eax, [eax];                        ; get kernel32.dll
		mov eax, [eax + 0x08];                 ; get dll address
		mov ecx, eax;                          ; ecx:PE shart address  
		mov eax, [eax + 0x3c];                 ; PE RVA
		add eax, ecx;
		mov eax, [eax + 0x78];
		add eax, ecx;                          ; eax:exporttable entry
		mov ebx, [eax + 0x20];
		add ebx, ecx;                          ; ebx:address namearray entry
		xor esi, esi;
	find_lib:
		xor edx, edx;
		xor edi, edi;
		mov edx, [ebx + esi * 4];
		mov edi, ecx;
		add edi, edx;
		inc esi;
		mov dx, [edi + 8h + 2h];
		cmp dx, 4179h;
		jne find_lib;
		mov dx, [edi];
		cmp dx, 6f4ch;
		jne find_lib;
	find_lib_end:
		mov ebx, [eax + 0x1c];                 ; ebx:addressarray entry RVA
		mov edx,ecx;
		add ebx, ecx;
        dec esi;
		mov ebx, [ebx + esi * 4];
		mov edx, ecx;
		add edx, ebx;                          ; edx:LoadLibraryA Function Address
		sub esp,0x20;
		mov [esp], 0x31;                       ; char *a = "1.dll"
		mov [esp+1],0x2e;
		mov [esp + 2], 0x64;
		mov[esp + 3], 0x6c;
		mov[esp + 4], 0x6c;
		mov[esp + 5], 0x00;
	    push esp;
		call edx;                              ;LoadLibraryA("1.dll")
		add esp,0x20
		popfd;
	}
	/*printf("%x", (unsigned long)&((IMAGE_NT_HEADERS *)(0))->OptionalHeader.DataDirectory - (unsigned long)&((IMAGE_NT_HEADERS *)(0))->Signature);*/
	return 0;
}
/*
find_lib:
xor r8, r8
mov rax, r9; r9:addressofnames array
mov edx, [rax + r10 * 4]; r10:sum
mov rax, rcx
add rax, rdx; get func name
inc r10
mov r8w, [rax + 8h + 2h]
cmp r8w, 4179h
jne find_lib
mov r8w, [rax]
cmp r8w, 6f4ch
jne find_lib
find_lib_end :
*/
