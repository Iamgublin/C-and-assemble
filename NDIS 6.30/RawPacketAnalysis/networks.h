#pragma once
#include<Windows.h>
typedef struct _MAC
{
	UCHAR dst[6];
	UCHAR sou[6];
	USHORT type;
}MAC, *PMAC;
typedef struct _IPPacket
{
	UCHAR iphVerLen; //版本号和头长度(各占4位)
	UCHAR ipTOS; //服务类型
	USHORT ipLength; //封包总长度，即整个IP报的长度
	USHORT ipID; //封包标识，惟一标识发送的每一个数据报
	USHORT ipFlags; //标志
	UCHAR ipTTL; //生存时间，就是TTL
	UCHAR ipProtocol; //协议，可能是TCP、UDP、ICMP等
	USHORT ipChecksum; //校验和
	ULONG ipSource; //源IP地址
	ULONG ipDestination; //目标IP地址
}IPPacket, *pIPPacket;
typedef struct _TCPPacket //20个字节
{
	USHORT sourcePort; //16位源端口号
	USHORT destinationPort;//16位目的端口号
	ULONG sequenceNumber; //32位序列号
	ULONG acknowledgeNumber;//32位确认号
	UCHAR dataoffset; //4位首部长度/4位保留字
	UCHAR flags; //6位标志位
	USHORT windows; //16位窗口大小
	USHORT checksum; //16位校验和
	USHORT urgentPointer; //16位紧急数据偏移量
}TCPPacket, *PTCPPacket;
typedef struct _UDPPacket
{
	USHORT sourcePort; //源端口号
	USHORT destinationPort;//目的端口号
	USHORT len; //封包长度
	USHORT checksum; //校验和
}UDPPacket, *PUDPPacket;
typedef struct _ICMPPacket
{
	UCHAR icmp_type; //消息类型
	UCHAR icmp_code; //代码
	USHORT icmp_checksum; //校验和
						  //下面是回显头
	USHORT icmp_id; //用来惟一标识此请求的ID号，通常设置为进程ID
	USHORT icmp_sequence; //序列号
	ULONG icmp_timestamp; //时间戳
}ICMPPacket, *PICMPPacket;
typedef struct _IGMPPacket //8字节
{
	UCHAR hVerType; //版本号和类型(各4位)
	UCHAR uReserved; //未用
	USHORT uCheckSum; //校验和
	ULONG dwGroupAddress;//32为组地址(D类IP地址)
}IGMPPacket, *PIGMPPacket;
typedef struct _ARPPacket //28字节的ARP头
{
	USHORT hrd; //硬件地址空间，以太网中为ARPHRD_EHER
	USHORT eth_type; //以太网类型，ETHERTYPE_IP
	UCHAR maclen; //MAC地址的长度，为6
	UCHAR iplen; //IP地址的长度，为4
	USHORT opcode; //操作代码,ARPOP_REWUEST为请求，ARPOP_REPLY为响应
	UCHAR smac[6]; //源MAC地址
	UCHAR saddr[4]; //源IP地址
	UCHAR dmac[6]; //目的MAC地址
	UCHAR daddr[4]; //目的IP地址
}ARPPacket, *PARPPacket;