#include"struct.h"
#include"DevCon.h"
#include"IO.h"
NTSTATUS InitAndFormatSpace(PDEVICE_OBJECT dev)
{
	PBOOT_SECTOR bootSector;
	ULONG        rootDirEntries;
	USHORT       fatType;
	USHORT       fatSectorCnt;
	USHORT       fatEntries;
	PDIR_ENTRY   rootDir;
	PUCHAR       firstFatSector;
	UNICODE_STRING Dl = RTL_CONSTANT_STRING(DEFAULT_DRIVE_LETTER);
	PDEVICE_EXTENSION Ext = dev->DeviceExtension;

	Ext->DiskRegInfo.DiskSize = DEFAULT_DISK_SIZE;
	RtlCopyUnicodeString(&Ext->DiskRegInfo.DriveLetter, &Dl);
	Ext->DiskRegInfo.RootDirEntries = DEFAULT_ROOT_DIR_ENTRIES;
	Ext->DiskRegInfo.SectorsPerCluster = SECTORS_PER_CLUSTER;

	//初始化DiskGeometry
	Ext->DiskGeometry.BytesPerSector = BYTE_PER_SECTOR;
	Ext->DiskGeometry.SectorsPerTrack = SECTORS_PER_TRACK;
	Ext->DiskGeometry.TracksPerCylinder = TRACK_PER_CYLINDER;
	Ext->DiskGeometry.Cylinders.QuadPart = Ext->DiskRegInfo.DiskSize / BYTE_PER_SECTOR / SECTORS_PER_TRACK / TRACK_PER_CYLINDER;
	Ext->DiskGeometry.MediaType = RAMDISK_MEDIA_TYPE;

	Ext->DiskImage = ExAllocatePoolWithTag(PagedPool, DEFAULT_DISK_SIZE, 'zL');

	if (Ext->DiskImage == NULL)
	{
		return STATUS_UNSUCCESSFUL;
	}

	rootDirEntries = Ext->DiskRegInfo.RootDirEntries;
	if (rootDirEntries & (DIR_ENTRIES_PER_SECTOR - 1)) {

		rootDirEntries =
			(rootDirEntries + (DIR_ENTRIES_PER_SECTOR - 1)) &
			~(DIR_ENTRIES_PER_SECTOR - 1);
	}


	//初始化第一个扇区的关键数据
	bootSector = (PBOOT_SECTOR)Ext->DiskImage;

	//初始化引导代码
	bootSector->bsJump[0] = 0xeb;
	bootSector->bsJump[1] = 0x3c;
	bootSector->bsJump[2] = 0x90;

	bootSector->bsOemName[0] = 'R';
	bootSector->bsOemName[1] = 'a';
	bootSector->bsOemName[2] = 'j';
	bootSector->bsOemName[3] = 'u';
	bootSector->bsOemName[4] = 'R';
	bootSector->bsOemName[5] = 'a';
	bootSector->bsOemName[6] = 'm';
	bootSector->bsOemName[7] = ' ';

	bootSector->bsBytesPerSec = (SHORT)Ext->DiskGeometry.BytesPerSector;
	bootSector->bsResSectors = 1;
	bootSector->bsFATs = 1;
	bootSector->bsRootDirEnts = (USHORT)rootDirEntries;

	bootSector->bsSectors = (USHORT)(Ext->DiskRegInfo.DiskSize /
		Ext->DiskGeometry.BytesPerSector);
	bootSector->bsMedia = (UCHAR)Ext->DiskGeometry.MediaType;
	bootSector->bsSecPerClus = (UCHAR)SECTORS_PER_CLUSTER;

	fatEntries =
		(bootSector->bsSectors - bootSector->bsResSectors -
			bootSector->bsRootDirEnts / DIR_ENTRIES_PER_SECTOR) /
		bootSector->bsSecPerClus + 2;

	//
	// Choose between 12 and 16 bit FAT based on number of clusters we
	// need to map
	//

	if (fatEntries > 4087) {
		fatType = 16;
		fatSectorCnt = (fatEntries * 2 + 511) / 512;
		fatEntries = fatEntries + fatSectorCnt;
		fatSectorCnt = (fatEntries * 2 + 511) / 512;
	}
	else {
		fatType = 12;
		fatSectorCnt = (((fatEntries * 3 + 1) / 2) + 511) / 512;
		fatEntries = fatEntries + fatSectorCnt;
		fatSectorCnt = (((fatEntries * 3 + 1) / 2) + 511) / 512;
	}

	bootSector->bsFATsecs = fatSectorCnt;
	bootSector->bsSecPerTrack = (USHORT)Ext->DiskGeometry.SectorsPerTrack;
	bootSector->bsHeads = (USHORT)Ext->DiskGeometry.TracksPerCylinder;
	bootSector->bsBootSignature = 0x29;
	bootSector->bsVolumeID = 0x12345678;

	bootSector->bsLabel[0] = 'R';
	bootSector->bsLabel[1] = 'a';
	bootSector->bsLabel[2] = 'm';
	bootSector->bsLabel[3] = 'D';
	bootSector->bsLabel[4] = 'i';
	bootSector->bsLabel[5] = 's';
	bootSector->bsLabel[6] = 'k';
	bootSector->bsLabel[7] = ' ';
	bootSector->bsLabel[8] = ' ';
	bootSector->bsLabel[9] = ' ';
	bootSector->bsLabel[10] = ' ';

	bootSector->bsFileSystemType[0] = 'F';
	bootSector->bsFileSystemType[1] = 'A';
	bootSector->bsFileSystemType[2] = 'T';
	bootSector->bsFileSystemType[3] = '1';
	bootSector->bsFileSystemType[4] = '?';
	bootSector->bsFileSystemType[5] = ' ';
	bootSector->bsFileSystemType[6] = ' ';
	bootSector->bsFileSystemType[7] = ' ';

	bootSector->bsFileSystemType[4] = (fatType == 16) ? '6' : '2';

	bootSector->bsSig2[0] = 0x55;
	bootSector->bsSig2[1] = 0xAA;


	firstFatSector = (PUCHAR)(bootSector + 1);
	firstFatSector[0] = (UCHAR)Ext->DiskGeometry.MediaType;
	firstFatSector[1] = 0xFF;
	firstFatSector[2] = 0xFF;

	if (fatType == 16) {
		firstFatSector[3] = 0xFF;
	}

	//
	// The Root Directory follows the FAT
	//
	rootDir = (PDIR_ENTRY)(bootSector + 1 + fatSectorCnt);

	//
	// Set device name to "MS-RAMDR"
	// NOTE: Fill all 8 characters, eg. sizeof(rootDir->deName);
	//
	rootDir->deName[0] = 'Z';
	rootDir->deName[1] = 'L';
	rootDir->deName[2] = 'Z';
	rootDir->deName[3] = '-';
	rootDir->deName[4] = 'V';
	rootDir->deName[5] = 'I';
	rootDir->deName[6] = 'R';
	rootDir->deName[7] = 'D';

	//
	// Set device extension name to "IVE"
	// NOTE: Fill all 3 characters, eg. sizeof(rootDir->deExtension);
	//
	rootDir->deExtension[0] = 'I';
	rootDir->deExtension[1] = 'S';
	rootDir->deExtension[2] = 'K';

	rootDir->deAttributes = DIR_ATTR_VOLUME;
	return STATUS_SUCCESS;
}
NTSTATUS ZlzInitDiskName(PDEVICE_OBJECT dev)
{
	NTSTATUS sta;
	UNICODE_STRING deviceName;
	UNICODE_STRING win32Name;
	UNICODE_STRING DriveLetter;
	PDEVICE_EXTENSION Ext = dev->DeviceExtension;

	RtlInitUnicodeString(&DriveLetter, DEFAULT_DRIVE_LETTER);
	RtlInitUnicodeString(&win32Name, DOS_DEVICE_NAME);
	RtlInitUnicodeString(&deviceName, NT_DEVICE_NAME);

	Ext->SymbolicLink.Buffer = (PWSTR)
		&Ext->DosDeviceNameBuffer;
	Ext->SymbolicLink.MaximumLength =
		sizeof(Ext->DosDeviceNameBuffer);
	Ext->SymbolicLink.Length = win32Name.Length;

	RtlCopyUnicodeString(&Ext->SymbolicLink, &win32Name);

	RtlAppendUnicodeStringToString(&Ext->SymbolicLink,
		&DriveLetter);
	sta = IoCreateSymbolicLink(&Ext->SymbolicLink, &deviceName);
	DbgPrint("sta:0x%x\n", sta);
	RtlCopyMemory(((PDEVICE_EXTENSION)dev->DeviceExtension)->DriveLetterBuffer, DriveLetter.Buffer, DriveLetter.Length);
	return STATUS_SUCCESS;
}
NTSTATUS Create(PDEVICE_OBJECT dev, PIRP irp)
{
	return STATUS_SUCCESS;
}
NTSTATUS Close(PDEVICE_OBJECT dev, PIRP irp)
{
	return STATUS_SUCCESS;
}
VOID DevUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	DbgBreakPoint();
	PDEVICE_OBJECT dev = driver->DeviceObject;
	while (dev)
	{
		PDEVICE_EXTENSION Ext = dev->DeviceExtension;
		if (Ext->DiskImage)
		{
			ExFreePool(Ext->DiskImage);
		}
		IoDeleteDevice(dev);
		dev = dev->NextDevice;
	}
}
NTSTATUS
AddDev(
	_In_ struct _DRIVER_OBJECT *DriverObject,
	_In_ struct _DEVICE_OBJECT *PhysicalDeviceObject
)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(PhysicalDeviceObject);
	DbgBreakPoint();
	if (DriverObject->DeviceObject)
	{
		ZlzInitDiskName(DriverObject->DeviceObject);
		InitAndFormatSpace(DriverObject->DeviceObject);
	}
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
	UNREFERENCED_PARAMETER(driver);
	UNREFERENCED_PARAMETER(str);
	DbgBreakPoint();
	NTSTATUS sta = 0;
	PDEVICE_OBJECT dev;
	UNICODE_STRING devname = RTL_CONSTANT_STRING(NT_DEVICE_NAME);

	driver->DriverExtension->AddDevice = AddDev;
	/*driver->MajorFunction[IRP_MJ_CREATE] = Create;*/
	/*driver->MajorFunction[IRP_MJ_CLOSE] = Close;*/
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DevCon;
	driver->MajorFunction[IRP_MJ_READ] = VirtualRead;
	driver->MajorFunction[IRP_MJ_WRITE] = VirtualWrite;

	sta=IoCreateDevice(driver, sizeof(DEVICE_EXTENSION), &devname, FILE_DEVICE_DISK, 0, FALSE, &dev);
	DbgPrint("sta:0x%x\n", sta);
	dev->Flags = DO_DIRECT_IO;
	dev->Flags &= ~DO_DEVICE_INITIALIZING;
	driver->DriverUnload = DevUnload;
	return STATUS_SUCCESS;
}