#pragma once
#include<wdm.h>
#include<ntdddisk.h>
#include<devguid.h>
#define NT_DEVICE_NAME                  L"\\Device\\Ramdisk"
#define DOS_DEVICE_NAME                 L"\\DosDevices\\"

#define BYTE_PER_SECTOR                 512
#define SECTORS_PER_CLUSTER             2               //一个簇的扇区数
#define SECTORS_PER_TRACK               32              //一个磁道的扇区数
#define TRACK_PER_CYLINDER              2				//一个柱面的磁道数
#define DIR_ENTRIES_PER_SECTOR          16
#define RAMDISK_MEDIA_TYPE              0xF8

#define DOS_DEVNAME_LENGTH              (sizeof(DOS_DEVICE_NAME)+sizeof(WCHAR)*10)
#define DRIVE_LETTER_LENGTH             (sizeof(WCHAR)*10)

#define DRIVE_LETTER_BUFFER_SIZE        10
#define DOS_DEVNAME_BUFFER_SIZE         (sizeof(DOS_DEVICE_NAME) / 2) + 10

#define DEFAULT_DISK_SIZE               (1024*1024)     // 1 MB
#define DEFAULT_ROOT_DIR_ENTRIES        512
#define DEFAULT_DRIVE_LETTER            L"Z:"


//目录属性
#define DIR_ATTR_READONLY   0x01
#define DIR_ATTR_HIDDEN     0x02
#define DIR_ATTR_SYSTEM     0x04
#define DIR_ATTR_VOLUME     0x08
#define DIR_ATTR_DIRECTORY  0x10
#define DIR_ATTR_ARCHIVE    0x20

typedef struct _DISK_INFO {
	ULONG   DiskSize;           // 磁盘大小
	ULONG   RootDirEntries;     // 根目录入口
	ULONG   SectorsPerCluster;  // 一个磁道的扇区数
	UNICODE_STRING DriveLetter; // 卷标
} DISK_INFO, *PDISK_INFO;
typedef struct _DEVICE_EXTENSION {
	PUCHAR              DiskImage;                  // Pointer to beginning of disk image
	DISK_GEOMETRY       DiskGeometry;               // Drive parameters built by Ramdisk
	DISK_INFO           DiskRegInfo;                // Disk parameters from the registry
	UNICODE_STRING      SymbolicLink;               // Dos symbolic name; Drive letter
	WCHAR               DriveLetterBuffer[DRIVE_LETTER_BUFFER_SIZE];
	WCHAR               DosDeviceNameBuffer[DOS_DEVNAME_BUFFER_SIZE];
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;
#pragma pack(push,1)
typedef struct  _BOOT_SECTOR
{
	UCHAR       bsJump[3];          // x86 jmp instruction, checked by FS
	CCHAR       bsOemName[8];       // OEM name of formatter
	USHORT      bsBytesPerSec;      // Bytes per Sector
	UCHAR       bsSecPerClus;       // Sectors per Cluster
	USHORT      bsResSectors;       // Reserved Sectors
	UCHAR       bsFATs;             // Number of FATs - we always use 1
	USHORT      bsRootDirEnts;      // Number of Root Dir Entries
	USHORT      bsSectors;          // Number of Sectors
	UCHAR       bsMedia;            // Media type - we use RAMDISK_MEDIA_TYPE
	USHORT      bsFATsecs;          // Number of FAT sectors
	USHORT      bsSecPerTrack;      // Sectors per Track - we use 32
	USHORT      bsHeads;            // Number of Heads - we use 2
	ULONG       bsHiddenSecs;       // Hidden Sectors - we set to 0
	ULONG       bsHugeSectors;      // Number of Sectors if > 32 MB size
	UCHAR       bsDriveNumber;      // Drive Number - not used
	UCHAR       bsReserved1;        // Reserved
	UCHAR       bsBootSignature;    // New Format Boot Signature - 0x29
	ULONG       bsVolumeID;         // VolumeID - set to 0x12345678
	CCHAR       bsLabel[11];        // Label - set to RamDisk
	CCHAR       bsFileSystemType[8];// File System Type - FAT12 or FAT16
	CCHAR       bsReserved2[448];   // Reserved
	UCHAR       bsSig2[2];          // Originial Boot Signature - 0x55, 0xAA
}   BOOT_SECTOR, *PBOOT_SECTOR;
typedef struct  _DIR_ENTRY
{
	UCHAR       deName[8];          // File Name
	UCHAR       deExtension[3];     // File Extension
	UCHAR       deAttributes;       // File Attributes
	UCHAR       deReserved;         // Reserved
	USHORT      deTime;             // File Time
	USHORT      deDate;             // File Date
	USHORT      deStartCluster;     // First Cluster of file
	ULONG       deFileSize;         // File Length
}   DIR_ENTRY, *PDIR_ENTRY;
#pragma pack(pop)

