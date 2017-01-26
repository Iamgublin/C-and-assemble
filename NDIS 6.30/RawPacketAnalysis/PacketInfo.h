#pragma once
#include<Windows.h>
#include"networks.h"
//用到的宏
#define Tranverse16(X)   ((((UINT16)(X) & 0xff00) >> 8) |(((UINT16)(X) & 0x00ff) << 8))
#define SET_INFO_TYPE(A,B) (A->Type=B)

//用于判断包类型
#define PACKET_IP   0x0800
#define PACKET_ARP  0X0806
#define PACKET_RARP 0X0835
#define PACKET_TCP  0X6
#define PACKET_IGMP 0X02
#define PACKET_ICMP 0x01
#define PACKET_UDP  0X11

//PacketInfo->type
#define INFO_ARP    1
#define INFO_RARP   2
#define INFO_TCP    3
#define INFO_UDP    4
#define INFO_ICMP   5
#define INFO_IGMP   6

typedef struct _PacketInfo
{
	int Type;
	BOOLEAN IsSendPacket;
	MAC Mac;
	union 
	{
		IPPacket Ip;
		ARPPacket Arp;
	}protocol;
	union 
	{
		TCPPacket Tcp;
		UDPPacket Udp;
		ICMPPacket Icmp;
		IGMPPacket Igmp;
	}protocol1;
	UCHAR RawPacket[2000];     //原始的包数据（MTU<=1500）
}PacketInfo,*PPacketInfo;