#include "RawPacketAnalysis.h"
#include"networks.h"
VOID analysis(MAC macpacket)
{
	printf("start*********************************\n");
	printf("dest mac:%02x-%02x-%02x-%02x-%02x-%02x\n", macpacket.dst[0], macpacket.dst[1], macpacket.dst[2], macpacket.dst[3], macpacket.dst[4], macpacket.dst[5]);
	printf("source mac:%02x-%02x-%02x-%02x-%02x-%02x\n", macpacket.sou[0], macpacket.sou[1], macpacket.sou[2], macpacket.sou[3], macpacket.sou[4], macpacket.sou[5]);
	printf("type:%04x\n", Tranverse16(macpacket.type));
	printf("end*********************************\n");
}
void AnalyseIgmp(PPacketInfo Info, int IpHeaderLen)
{
	memcpy(&Info->protocol1.Igmp, Info->RawPacket + sizeof(MAC) + IpHeaderLen, sizeof(IGMPPacket));
}
void AnalyseUdp(PPacketInfo Info, int IpHeaderLen)
{
	memcpy(&Info->protocol1.Udp, Info->RawPacket + sizeof(MAC) + IpHeaderLen, sizeof(UDPPacket));
}
void AnalyseIcmp(PPacketInfo Info, int IpHeaderLen)
{
	memcpy(&Info->protocol1.Icmp, Info->RawPacket + sizeof(MAC) + IpHeaderLen, sizeof(ICMPPacket));
}
void AnalyseArp(PPacketInfo Info)
{
	memcpy(&Info->protocol.Arp, Info->RawPacket + sizeof(MAC), sizeof(ARPPacket));
	/*printf("**************ARP**********\n");
	if (Tranverse16(Info->protocol.Arp.opcode) == ARP_REQUEST)
	{
		printf("who has ip %03d.%03d.%03d.%03d? tell %03d.%03d.%03d.%03d\n",Info->protocol.Arp.daddr[0],
			Info->protocol.Arp.daddr[1], Info->protocol.Arp.daddr[2], Info->protocol.Arp.daddr[3],Info->protocol.Arp.saddr[0],
			Info->protocol.Arp.saddr[1], Info->protocol.Arp.saddr[2], Info->protocol.Arp.saddr[3]);
	}
	else if (Tranverse16(Info->protocol.Arp.opcode) == ARP_REPLY)
	{
		printf("%03d.%03d.%03d.%03d is in mac %02x-%02x-%02x-%02x-%02x-%02x\n", Info->protocol.Arp.saddr[0], Info->protocol.Arp.saddr[1],
			Info->protocol.Arp.saddr[2], Info->protocol.Arp.saddr[3], Info->protocol.Arp.smac[0], Info->protocol.Arp.smac[1],
			Info->protocol.Arp.smac[2], Info->protocol.Arp.smac[3], Info->protocol.Arp.smac[4], Info->protocol.Arp.smac[5]);
	}
	printf("****************************\n");*/
}
void AnalyseTcp(PPacketInfo Info,int IpHeaderLen)
{
	memcpy(&Info->protocol1.Tcp, Info->RawPacket + sizeof(MAC) + IpHeaderLen, sizeof(TCPPacket));
}
void AnalyseIp(PPacketInfo Info)
{
	memcpy(&Info->protocol.Ip, Info->RawPacket + sizeof(MAC), sizeof(IPPacket));
	/*printf("Protocol:0x%x\n", Info->protocol.Ip.ipProtocol);*/
	int IpHeaderLen = (Info->protocol.Ip.iphVerLen & 0x0f)*4;
	switch (Info->protocol.Ip.ipProtocol)
	{
	case PACKET_TCP:
		SET_INFO_TYPE(Info, INFO_TCP);
		AnalyseTcp(Info, IpHeaderLen);
		break;
	case PACKET_ICMP:
		SET_INFO_TYPE(Info, INFO_ICMP);
		AnalyseIcmp(Info, IpHeaderLen);
		break;
	case PACKET_IGMP:
		SET_INFO_TYPE(Info, INFO_IGMP);
		AnalyseIgmp(Info, IpHeaderLen);
		break;
	case PACKET_UDP:
		SET_INFO_TYPE(Info, INFO_UDP);
		AnalyseUdp(Info, IpHeaderLen);
		break;
	default:
		SET_INFO_TYPE(Info, INFO_UNKNOWN);
		break;
	}
}
RAWPACKETANALYSIS_API int AnalysePacket(PIO_Packet Packet,PPacketInfo Info)
{
	if (Packet->Type != PACKET_TYPE_NETPACKET)
	{
		return 0;
	}
	memcpy(Info->RawPacket, Packet->Packet.Net_Packet_Output.Buffer, sizeof(Info->RawPacket));
	Info->IsSendPacket = Packet->Packet.Net_Packet_Output.IsSendPacket;
	memcpy(&Info->Mac, Info->RawPacket, sizeof(Info->Mac));
	/*analysis(Info->Mac);*/
	switch (Tranverse16(Info->Mac.type))
	{
	case PACKET_IP:
		AnalyseIp(Info);
		break;
	case PACKET_ARP:
		SET_INFO_TYPE(Info, INFO_ARP);
		AnalyseArp(Info);
		break;
	case PACKET_IPv6:
		SET_INFO_TYPE(Info, INFO_IPv6);              //暂时不解析IPv6数据包
		break;
	default:
		SET_INFO_TYPE(Info, INFO_UNKNOWN);
		analysis(Info->Mac);
		break;
	}
    return 1;
}