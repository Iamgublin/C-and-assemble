// ConsoleTest.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include"..\NdisCoreApi\NdisCoreApi.h"
#include"..\RawPacketAnalysis\RawPacketAnalysis.h"
#pragma comment(lib,"..\\lib\\RawPacketAnalysis.lib")
#pragma comment(lib,"..\\lib\\NdisCoreApi.lib")
#include<locale.h>
char pro[11][8] = {"UNKNOWN","ARP","RARP","TCP","UDP","ICMP","IGMP","HTTP","NAT","DHCP","IPv6"};
void SetConsole()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD size;
	size.X = 2000;
	size.Y = 2000;
	SetConsoleScreenBufferSize(hOut, size);
}
int main()
{
	SetConsole();
	setlocale(LC_ALL, "chs");
	HANDLE hF = Net_OpenFilter();
	printf("%p\n", hF);
	PIO_Packet Output = (PIO_Packet)malloc(sizeof(IO_Packet));
	PacketInfo Info = { 0 };
	Net_StartFilter(hF, NULL);
	Net_ShowAdapter(hF, Output);
	printf("0x%x\n", GetLastError());
	printf("Adapter Num:%d\n", Output->Packet.ShowAdapter.Num);
	for (int i = 0; i < Output->Packet.ShowAdapter.Num; i++)
	{
		wprintf(TEXT("Adapter Name%d:%s\n"),i, Output->Packet.ShowAdapter.AdapterInfo[i].DevName);
		wprintf(TEXT("Adapter Path%d:%s\n"),i, Output->Packet.ShowAdapter.AdapterInfo[i].DevPathName);
		wprintf(TEXT("Is Filtering:%s\n"), Output->Packet.ShowAdapter.AdapterInfo[i].isFiltering ? TEXT("TRUE") : TEXT("FALSE"));
		PUCHAR Mac = Output->Packet.ShowAdapter.AdapterInfo[i].MacAddress;
		printf("Mac Address:%02x-%02x-%02x-%02x-%02x-%02x\n", Mac[0], Mac[1], Mac[2], Mac[3], Mac[4], Mac[5]);
	}
	int a = 2;
	while (1)
	{
		Sleep(100);
		if (Net_GetRawPacket(hF, Output, a))
		{
			AnalysePacket(Output, &Info);
			if (Info.Type == INFO_IPv6 || Info.Type == INFO_UNKNOWN)
			{
				continue;
			}
			if (Info.Type == INFO_ARP)
			{
				UCHAR* saddr = Info.protocol.Arp.saddr;
				UCHAR* daddr = Info.protocol.Arp.daddr;
				printf("%03d.%03d.%03d.%03d\t%03d.%03d.%03d.%03d\t%4d\t%5s\t", saddr[0], saddr[1], saddr[2], saddr[3], daddr[0], daddr[1], daddr[2], daddr[3], Info.Size,pro[Info.Type]);
				if (Tranverse16(Info.protocol.Arp.opcode) == ARP_REQUEST)
				{
					printf("who has ip %03d.%03d.%03d.%03d? tell %03d.%03d.%03d.%03d\n", Info.protocol.Arp.daddr[0],
						Info.protocol.Arp.daddr[1], Info.protocol.Arp.daddr[2], Info.protocol.Arp.daddr[3], Info.protocol.Arp.saddr[0],
						Info.protocol.Arp.saddr[1], Info.protocol.Arp.saddr[2], Info.protocol.Arp.saddr[3]);
				}
				else if (Tranverse16(Info.protocol.Arp.opcode) == ARP_REPLY)
				{
					printf("%03d.%03d.%03d.%03d is in mac %02x-%02x-%02x-%02x-%02x-%02x\n", Info.protocol.Arp.saddr[0], Info.protocol.Arp.saddr[1],
						Info.protocol.Arp.saddr[2], Info.protocol.Arp.saddr[3], Info.protocol.Arp.smac[0], Info.protocol.Arp.smac[1],
						Info.protocol.Arp.smac[2], Info.protocol.Arp.smac[3], Info.protocol.Arp.smac[4], Info.protocol.Arp.smac[5]);
				}
			}
			else
			{
				UCHAR* saddr = Info.protocol.Ip.ipSource;
				UCHAR* daddr = Info.protocol.Ip.ipDestination;
				printf("%03d.%03d.%03d.%03d\t%03d.%03d.%03d.%03d\t%4d\t%5s\t", saddr[0], saddr[1], saddr[2], saddr[3], daddr[0], daddr[1], daddr[2], daddr[3], Info.Size, pro[Info.Type]);
				if (Info.Type == INFO_TCP)
				{
					printf("window:%d port:%d->%d ack:%d syn:%d fin:%d %d\n", Tranverse16(Info.protocol1.Tcp.windows), Tranverse16(Info.protocol1.Tcp.sourcePort), Tranverse16(Info.protocol1.Tcp.destinationPort), Info.protocol1.Tcp.ack,
						Info.protocol1.Tcp.syn, Info.protocol1.Tcp.fin,Info.protocol1.Tcp.urgentPointer);
				}
				else
				{
					printf("\n");
				}
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

