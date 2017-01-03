// ConsoleTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"..\NdisCoreApi\NdisCoreApi.h"
#pragma comment(lib,"..\\ReleaseX64\\NdisCoreApi.lib")
int main()
{
	HANDLE hF = Net_OpenFilter();
	printf("%p\n", hF);
	PIO_Packet Output = (PIO_Packet)malloc(sizeof(IO_Packet));
	Net_ShowAdapter(hF, Output);
	printf("0x%x\n", GetLastError());
	printf("Adapter Num:%d\n", Output->Packet.ShowAdapter.Num);
	wprintf(TEXT("Adapter Name:%s\n"), Output->Packet.ShowAdapter.AdapterInfo[0].DevName);
	wprintf(TEXT("Adapter Path:%s\n"), Output->Packet.ShowAdapter.AdapterInfo[0].DevPathName);
	Sleep(INFINITE);
    return 0;
}

