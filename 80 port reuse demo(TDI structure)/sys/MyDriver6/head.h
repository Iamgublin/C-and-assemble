#include<ntifs.h>
#include<ntddk.h>
#include<tdikrnl.h>
#pragma warning(disable:4054)
#pragma warning(disable:4055)
#pragma warning(disable:4127)
#define TCP L"\\Device\\Tcp"
#define UDP L"\\Device\\Udp"
#define TCPDRI L"\\Driver\\Tcpip"
#define DEVICE_NAME L"\\Device\\tdi_device"
#define SYN_NAME L"\\??\\tdi_control_device"
#define PORT_MAX 32
#define IOCTL_CONSEND (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x911,METHOD_BUFFERED,FILE_WRITE_DATA|FILE_READ_DATA)  
#define IOCTL_CONRECV (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x912,METHOD_BUFFERED,FILE_WRITE_DATA|FILE_READ_DATA)
typedef struct _CONTEXTI
{
	PDEVICE_OBJECT realdev;
	PIRP query_irp;
	PMDL mdl;
}*PCONTEXTI, CONTEXTI;
typedef struct _global
{
	PFILE_OBJECT porttable[PORT_MAX];
	BOOLEAN canuse[PORT_MAX];
	CONNECTION_CONTEXT* connectioncontext[PORT_MAX];
	int count;
}global, *pglobal;
global globals;
#pragma pack(push,1)
typedef struct _temp_context
{
	int magic;
	PVOID old_handler;
	PVOID old_context;
}temp_context, *ptemp_context;
#pragma pack(pop)
NTSTATUS my_ClientEventConnect(
	_In_  PVOID              TdiEventContext,
	_In_  LONG               RemoteAddressLength,
	_In_  PVOID              RemoteAddress,
	_In_  LONG               UserDataLength,
	_In_  PVOID              UserData,
	_In_  LONG               OptionsLength,
	_In_  PVOID              Options,
	_Out_ CONNECTION_CONTEXT *ConnectionContext,
	_Out_ PIRP               *AcceptIrp
	);
typedef NTSTATUS(__stdcall *ClientEventConnect)(
	_In_  PVOID              TdiEventContext,
	_In_  LONG               RemoteAddressLength,
	_In_  PVOID              RemoteAddress,
	_In_  LONG               UserDataLength,
	_In_  PVOID              UserData,
	_In_  LONG               OptionsLength,
	_In_  PVOID              Options,
	_Out_ CONNECTION_CONTEXT *ConnectionContext,
	_Out_ PIRP               *AcceptIrp
	);
NTSTATUS my_ClientEventReceive(
	_In_  PVOID              TdiEventContext,
	_In_  CONNECTION_CONTEXT ConnectionContext,
	_In_  ULONG              ReceiveFlags,
	_In_  ULONG              BytesIndicated,
	_In_  ULONG              BytesAvailable,
	_Out_ ULONG              *BytesTaken,
	_In_  PVOID              Tsdu,
	_Out_ PIRP               *IoRequestPacket
	);
typedef NTSTATUS(__stdcall *ClientEventReceive)(
	_In_  PVOID              TdiEventContext,
	_In_  CONNECTION_CONTEXT ConnectionContext,
	_In_  ULONG              ReceiveFlags,
	_In_  ULONG              BytesIndicated,
	_In_  ULONG              BytesAvailable,
	_Out_ ULONG              *BytesTaken,
	_In_  PVOID              Tsdu,
	_Out_ PIRP               *IoRequestPacket
	);
NTSTATUS my_ClientEventDisconnect(
	_In_ PVOID              TdiEventContext,
	_In_ CONNECTION_CONTEXT ConnectionContext,
	_In_ LONG               DisconnectDataLength,
	_In_ PVOID              DisconnectData,
	_In_ LONG               DisconnectInformationLength,
	_In_ PVOID              DisconnectInformation,
	_In_ ULONG              DisconnectFlags
	);
typedef NTSTATUS(__stdcall *ClientEventDisconnect)(
	_In_ PVOID              TdiEventContext,
	_In_ CONNECTION_CONTEXT ConnectionContext,
	_In_ LONG               DisconnectDataLength,
	_In_ PVOID              DisconnectData,
	_In_ LONG               DisconnectInformationLength,
	_In_ PVOID              DisconnectInformation,
	_In_ ULONG              DisconnectFlags
	);
/* Native API prototypes */
NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
IN PUNICODE_STRING	ObjectName,
IN ULONG			Attributes,
IN PACCESS_STATE	PassedAccessState OPTIONAL,
IN ACCESS_MASK		DesiredAccess OPTIONAL,
IN POBJECT_TYPE		ObjectType OPTIONAL,
IN KPROCESSOR_MODE	AccessMode,
IN OUT PVOID		ParseContext OPTIONAL,
OUT	PVOID			*Object
);
extern POBJECT_TYPE *IoDriverObjectType;
enum DevType
{
	DEVTYPE_TCP = 0,
	DEVTYPE_UDP,
} devty;
typedef struct  {
	union {
		struct { unsigned char s_b1, s_b2, s_b3, s_b4; } S_un_b;
		struct { unsigned short s_w1, s_w2; } S_un_w;
		unsigned long S_addr;
	} S_un;
}in_addr, *pin_addr;
typedef struct _ctx
{
	PMDL mdl;
	KEVENT eve;
}ctx,*pctx;
PDRIVER_OBJECT dri;
PDEVICE_OBJECT
tcpreal = NULL,
udpreal = NULL,
tcpflt = NULL,
udpflt = NULL,
tcptop = NULL,
udptop = NULL,
condev=NULL;
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str);
NTSTATUS attach(PUNICODE_STRING tardevname, enum DevType type);
NTSTATUS unload(PDRIVER_OBJECT driver);
NTSTATUS dispatch(PDEVICE_OBJECT dev, PIRP irp);
enum DevType check(PDEVICE_OBJECT dev);
NTSTATUS createcomplete(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
NTSTATUS querycom(PDEVICE_OBJECT dev, PIRP irp, PVOID context);
BOOLEAN insertportinfo(PIO_STACK_LOCATION sa, CONNECTION_CONTEXT *ConnectionContext);
int checkportinfo(CONNECTION_CONTEXT context);
VOID deleteportinfo(CONNECTION_CONTEXT ConnectionContext);
NTSTATUS tdi_io_control(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS tdi_send_complete(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp,
	_In_ PVOID Context
	);
USHORT tdi_ntohs(USHORT netshort)
{
	USHORT result = 0;
	((char *)&result)[0] = ((char *)&netshort)[1];
	((char *)&result)[1] = ((char *)&netshort)[0];
	return result;
}