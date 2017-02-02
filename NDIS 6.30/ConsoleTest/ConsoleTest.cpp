// ConsoleTest.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include"..\NdisCoreApi\NdisCoreApi.h"
#include"..\RawPacketAnalysis\RawPacketAnalysis.h"
#pragma comment(lib,"..\\ReleaseX64\\RawPacketAnalysis.lib")
#pragma comment(lib,"..\\ReleaseX64\\NdisCoreApi.lib")
#include<locale.h>
char pro[11][8] = {"UNKNOWN","ARP","RARP","TCP","UDP","ICMP","IGMP","HTTP","NAT","DHCP","IPv6"};
int main()
{
	setlocale(LC_ALL, "chs");
	HANDLE hF = Net_OpenFilter();
	printf("%p\n", hF);
	PIO_Packet Output = (PIO_Packet)malloc(sizeof(IO_Packet));
	PacketInfo Info = { 0 };
	/*Net_ShowAdapter(hF, Output);
	printf("0x%x\n", GetLastError());
	printf("Adapter Num:%d\n", Output->Packet.ShowAdapter.Num);
	for (int i = 0; i < Output->Packet.ShowAdapter.Num; i++)
	{
		wprintf(TEXT("Adapter Name%d:%s\n"),i, Output->Packet.ShowAdapter.AdapterInfo[i].DevName);
		wprintf(TEXT("Adapter Path%d:%s\n"),i, Output->Packet.ShowAdapter.AdapterInfo[i].DevPathName);
		PUCHAR Mac = Output->Packet.ShowAdapter.AdapterInfo[i].MacAddress;
		printf("Mac Address:%02x-%02x-%02x-%02x-%02x-%02x\n", Mac[0], Mac[1], Mac[2], Mac[3], Mac[4], Mac[5]);
	}*/
	int a = 2;
	while (1)
	{
		Sleep(100);
		if (Net_GetRawPacket(hF, Output, a))
		{
			AnalysePacket(Output, &Info);
			printf("TYPE:%s\n", pro[Info.Type]);
			if (Info.Type == INFO_TCP)
			{
				printf("Number:%d\n", Tranverse16(Info.protocol1.Tcp.destinationPort));
			}
		}
	}
	/*printf("size:%d\n", Output->Packet.Net_Packet_Output.Size);
	for (int b = 0; b < 2000; b++)
	{
		printf("%02x ", Output->Packet.Net_Packet_Output.Buffer[b]);
	}*/
	Sleep(INFINITE);
	return 0;
}

