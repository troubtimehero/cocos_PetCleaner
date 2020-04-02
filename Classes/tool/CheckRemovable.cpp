#include "CheckRemovable.h"


/************************************************************************/
/*				U≈Ã                                                     */
/************************************************************************/
#include <wtypes.h>
#include <stdio.h>
#include <dbt.h>						// For DeviceChange.
#include <winioctl.h>					// For DeviceIOCtl.

#define MAX_LOADSTRING 100

// Add fro USB Task.

// USB letters' container
TCHAR   szMoveDiskName[33];
TCHAR	szDrvName[33];
BOOL	bInitUSBs;
// Drive type names
#define DRVUNKNOWN		0
#define DRVFIXED		1
#define DRVREMOTE		2
#define DRVRAM			3
#define DRVCD			4
#define DRVREMOVE		5

// IOCTL control code
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)


/****************************************************************************
*
*    FUNCTION: GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
*
*    PURPOSE:  get the info of specified device
*
****************************************************************************/
BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	STORAGE_PROPERTY_QUERY	Query;	// input param for query
	DWORD dwOutBytes;				// IOCTL output length
	BOOL bResult;					// IOCTL return val

	// specify the query type
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	// Query using IOCTL_STORAGE_QUERY_PROPERTY 
	bResult = ::DeviceIoControl(hDevice,			// device handle
		IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
		&Query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
		pDevDesc, pDevDesc->Size,				// output data buffer
		&dwOutBytes,							// out's length
		(LPOVERLAPPED)NULL);					

	return bResult;
}

/****************************************************************************
*
*    FUNCTION: chFirstDriverFrameMask(ULONG unitmask)
*
*    PURPOSE:  get the logic name of driver
*
****************************************************************************/
char chFirstDriveFromMask (ULONG unitmask)
{

	char i;
	for (i = 0; i < 26; ++i)  
	{
		if (unitmask & 0x1) 
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}

/****************************************************************************
*
*    FUNCTION: ReInitUSB_Disk_Letter()
*
*    PURPOSE:  get the usb disks, and filling the 'szMoveDiskName' with them
*
****************************************************************************/
void ReInitUSB_Disk_Letter()
{
	int k = 0;
	DWORD			MaxDriveSet, CurDriveSet;
	DWORD			drive, drivetype;
	TCHAR			szBuf[300];
	HANDLE			hDevice;
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;

	for(k=0; k<26; k++)
		szMoveDiskName[k] = '\0';	
	k = 1;		
	// Get available drives we can monitor
	MaxDriveSet = CurDriveSet = 0;

	MaxDriveSet = GetLogicalDrives();
	CurDriveSet = MaxDriveSet;
	for ( drive = 0; drive < 32; ++drive )  
	{
		if ( MaxDriveSet & (1 << drive) )  
		{
			DWORD temp = 1<<drive;
			sprintf( szDrvName, "%c:\\", 'A'+drive );
			switch ( GetDriveType( szDrvName ) )  
			{
			case 0:					// The drive type cannot be determined.
			case 1:					// The root directory does not exist.
				drivetype = DRVUNKNOWN;
				break;
			case DRIVE_REMOVABLE:	// The drive can be removed from the drive.
				drivetype = DRVREMOVE;
				szMoveDiskName[k] = chFirstDriveFromMask(temp);
				szMoveDiskName[0]=k;
				k++;
				break;
			case DRIVE_CDROM:		// The drive is a CD-ROM drive.
				break;
			case DRIVE_FIXED:		// The disk cannot be removed from the drive.
				drivetype = DRVFIXED;
				sprintf(szBuf, "\\\\?\\%c:", 'A'+drive);
				hDevice = CreateFile(szBuf, GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

				if (hDevice != INVALID_HANDLE_VALUE)
				{

					pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];

					pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

					if(GetDisksProperty(hDevice, pDevDesc))
					{
						if(pDevDesc->BusType == BusTypeUsb)
						{
							szMoveDiskName[k] = chFirstDriveFromMask(temp);
							szMoveDiskName[0]=k;
							k++;
						}
					}

					delete pDevDesc;
					CloseHandle(hDevice);
				}

				break;
			case DRIVE_REMOTE:		// The drive is a remote (network) drive.
				drivetype = DRVREMOTE;
				szMoveDiskName[k] = chFirstDriveFromMask(temp);
				szMoveDiskName[0]=k;
				k++;
				break;
			case DRIVE_RAMDISK:		// The drive is a RAM disk.
				drivetype = DRVRAM;
				break;
			}
		}
	}

}



const char*	System_My_GetRemovableList()
{
	return szMoveDiskName;
}

