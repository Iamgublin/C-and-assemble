#include "RawPacketAnalysis.h"
#include"networks.h"
void AnalyseArp(PPacketInfo Info)
{
	memcpy(&Info->protocol.Arp, Info->RawPacket + sizeof(MAC), sizeof(ARPPacket));
}
void AnalyseTcp(PPacketInfo Info)
{
	memcpy(&Info->protocol1.Tcp, Info->RawPacket + sizeof(MAC) + sizeof(IPPacket),sizeof(TCPPacket));
}
void AnalyseIp(PPacketInfo Info)
{
	memcpy(&Info->protocol.Ip, Info->RawPacket + sizeof(MAC),sizeof(IPPacket));
	printf("Protocol:0x%x\n", Info->protocol.Ip.ipProtocol);
	switch (Info->protocol.Ip.ipProtocol)
	{
	case PACKET_TCP:
		SET_INFO_TYPE(Info, INFO_TCP);
		AnalyseTcp(Info);
		break;
	case PACKET_ICMP:
		SET_INFO_TYPE(Info, INFO_ICMP);
		break;
	case PACKET_IGMP:
		SET_INFO_TYPE(Info, INFO_IGMP);
		break;
	case PACKET_UDP:
		SET_INFO_TYPE(Info, INFO_UDP);
		break;
	default:
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
	/*analysis(Info->RawPacket);*/
	switch (Tranverse16(Info->Mac.type))
	{
	case PACKET_IP:
		AnalyseIp(Info);
		break;
	case PACKET_ARP:
		SET_INFO_TYPE(Info, INFO_ARP);
		AnalyseArp(Info);
		break;
	default:
		break;
	}
    return 1;
}