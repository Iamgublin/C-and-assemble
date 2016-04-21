typedef struct _ETHeader //以太网数据帧头部结构
{
	UCHAR dhost[6]; //目的MAC地址
	UCHAR shost[6]; //源MAC地址
	USHORT type; //下层协议类型，如IP(ETHERTYPE_IP)，ARP（ETHERTYPE_ARP)等
}ETHeader, *PETHeader;
KEVENT queryeve;
KEVENT seteve;
typedef struct SEND_PACKET_RSVD
{
	PNDIS_PACKET            pOriginalPacket;
} SEND_PACKET_RSVD, *PSEND_PACKET_RSVD;
typedef struct _NPROT_RECV_PACKET_RSVD
{
	PNDIS_PACKET            pOriginalPacket;
} RECV_PACKET_RSVD, *PRECV_PACKET_RSVD;
typedef struct _adapercontext
{
	LIST_ENTRY packrecvlist;
	NDIS_HANDLE sendpacketpool;
	NDIS_HANDLE recvpacketpool;
	NDIS_HANDLE recvbufferpool;
	KSPIN_LOCK lock;
	NDIS_STATUS status;
	int contextno;
	NDIS_STRING DeviceName;
	NDIS_EVENT ndiseve;               //绑定适配器
	KEVENT unbindeve;             //解除绑定
	KEVENT recveve;                   //接收数据包事件
	BOOLEAN isnotgetdescriptor;
	NDIS_HANDLE relateminiporthandle;
	UINT seletemediumundex;
}adapercontext, *padapercontext;
typedef struct _miniportcontext
{
	int contextno;
	NDIS_HANDLE Miniportadapterhandle;
	NDIS_HANDLE relateadapterhandle;
	padapercontext relateadaptercontext;
	NDIS_REQUEST queryinformationrequest;
	NDIS_REQUEST setinformationrequest;
}miniportcontext, *pminiportcontext;
typedef struct _GLOBAL
{
	NDIS_HANDLE driverhandle;
	NDIS_HANDLE protocolhandle;
	padapercontext adaptercontext[20];   //最多支持20个设备
	NDIS_HANDLE bindinghandle[20];
	int contextnum;
	pminiportcontext miniportcontext[20];
	int mininum;
}GLOBAL, *PGLOBAL;
GLOBAL global;
PGLOBAL pglobal = &global;
typedef struct _Information
{
	PULONG sebyteread;
	PULONG seBytesNeeded;
	PULONG quBytesWritten;
	PULONG quBytesNeeded;
}Information, *pInformation;
Information requestinfo;
NDIS_MEDIUM         mediumarray[4] =
{
	NdisMedium802_3,    // Ethernet
	NdisMedium802_5,    // Token-ring
	NdisMediumFddi,     // Fddi
	NdisMediumWan       // NDISWAN
	/*NdisMediumCoWan*/
};
enum PacketType
{
	ZLZ_PACKET_RECEIVE,
	ZLZ_PACKET_SEND,
};
#pragma pack(push,1)
typedef struct _IpPacket
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
}IpPacket, *pIpPacket;
#pragma pack(pop)