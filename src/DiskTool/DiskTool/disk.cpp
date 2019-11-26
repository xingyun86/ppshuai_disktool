/**********************************************************************
 *  This program is free software; you can redistribute it and/or     *
 *  modify it under the terms of the GNU General Public License       *
 *  as published by the Free Software Foundation; either version 2    *
 *  of the License, or (at your option) any later version.            *
 *                                                                    *
 *  This program is distributed in the hope that it will be useful,   *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     *
 *  GNU General Public License for more details.                      *
 *                                                                    *
 *  You should have received a copy of the GNU General Public License *
 *  along with this program; if not, see http://gnu.org/licenses/     *
 *  ---                                                               *
 *  Copyright (C) 2009, Justin Davis <tuxdavis@gmail.com>             *
 *  Copyright (C) 2009-2017 ImageWriter developers                    *
 *                 https://sourceforge.net/projects/win32diskimager/  *
 **********************************************************************/

#include "pch.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <winioctl.h>
#include "disk.h"

HANDLE GetHandleOnFile(LPCWSTR filelocation, DWORD access)
{
    HANDLE hFile;
    hFile = CreateFileW(filelocation, access, (access == GENERIC_READ) ? FILE_SHARE_READ : 0, NULL, (access == GENERIC_READ) ? OPEN_EXISTING:CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        wchar_t *errormessage=NULL;
        ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0,
                         (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("File Error"), QObject::tr("An error occurred when attempting to get a handle on the file.\n"
        //                                                      "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return hFile;
}
DWORD GetDeviceID(HANDLE hVolume)
{
    VOLUME_DISK_EXTENTS sd;
    DWORD bytesreturned;
    if (!DeviceIoControl(hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &sd, sizeof(sd), &bytesreturned, NULL))
    {
        wchar_t *errormessage=NULL;
        ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0,
                         (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Volume Error"),
        //                      QObject::tr("An error occurred when attempting to get information on volume.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return sd.Extents[0].DiskNumber;
}

HANDLE GetHandleOnDevice(int device, DWORD access)
{
    HANDLE hDevice;
	CHAR deviceName[MAX_PATH] = { 0 };
	snprintf(deviceName, sizeof(deviceName) / sizeof(*deviceName), "\\\\.\\PhysicalDrive%d", device);
    hDevice = CreateFile(deviceName, access, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Device Error"),
        //                      QObject::tr("An error occurred when attempting to get a handle on the device.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return hDevice;
}

HANDLE GetHandleOnVolume(int volume, DWORD access)
{
    HANDLE hVolume;
    char volumename[] = "\\\\.\\A:";
    volumename[4] += volume;
    hVolume = CreateFile(volumename, access, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hVolume == INVALID_HANDLE_VALUE)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Volume Error"),
        //                      QObject::tr("An error occurred when attempting to get a handle on the volume.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return hVolume;
}

BOOL GetLockOnVolume(HANDLE handle)
{
    DWORD bytesreturned;
    BOOL bResult;
    bResult = DeviceIoControl(handle, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &bytesreturned, NULL);
    if (!bResult)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Lock Error"),
        //                      QObject::tr("An error occurred when attempting to lock the volume.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return (bResult);
}

BOOL RemoveLockOnVolume(HANDLE handle)
{
    DWORD junk;
    BOOL bResult;
    bResult = DeviceIoControl(handle, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &junk, NULL);
    if (!bResult)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Unlock Error"),
        //                      QObject::tr("An error occurred when attempting to unlock the volume.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return (bResult);
}

BOOL UnmountVolume(HANDLE handle)
{
    DWORD junk;
    BOOL bResult;
    bResult = DeviceIoControl(handle, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &junk, NULL);
    if (!bResult)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Dismount Error"),
        //                      QObject::tr("An error occurred when attempting to dismount the volume.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return (bResult);
}

BOOL IsVolumeUnmounted(HANDLE handle)
{
    DWORD junk;
    BOOL bResult;
    bResult = DeviceIoControl(handle, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, &junk, NULL);
    return (bResult);
}

char *ReadSectorDataFromHandle(HANDLE handle, UINT64 startsector, DWORD numsectors, DWORD sectorsize)
{
    DWORD bytesread = 0;
    char *data = new char[sectorsize * numsectors];
    LARGE_INTEGER li;
    li.QuadPart = startsector * sectorsize;
    SetFilePointer(handle, li.LowPart, &li.HighPart, FILE_BEGIN);
    if (!ReadFile(handle, data, sectorsize * numsectors, &bytesread, NULL))
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Read Error"),
        //                      QObject::tr("An error occurred when attempting to read data from handle.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
        delete[] data;
        data = NULL;
    }
    if (data && bytesread < (sectorsize * numsectors))
    {
        memset(data + bytesread,0,(sectorsize * numsectors) - bytesread);
    }
    return data;
}

BOOL WriteSectorDataToHandle(HANDLE handle, char *data, UINT64 startsector, DWORD numsectors, DWORD sectorsize)
{
	DWORD byteswritten = 0;
    BOOL bResult;
    LARGE_INTEGER li;
    li.QuadPart = startsector * sectorsize;
    SetFilePointer(handle, li.LowPart, &li.HighPart, FILE_BEGIN);
    bResult = WriteFile(handle, data, sectorsize * numsectors, &byteswritten, NULL);
    if (!bResult)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Write Error"),
        //                      QObject::tr("An error occurred when attempting to write data to handle.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
    }
    return (bResult);
}

UINT64 GetNumberOfSectors(HANDLE handle, UINT64 * sectorsize)
{
    DWORD junk;
    DISK_GEOMETRY_EX diskgeometry;
    BOOL bResult;
    bResult = DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &diskgeometry, sizeof(diskgeometry), &junk, NULL);
    if (!bResult)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Device Error"),
        //                      QObject::tr("An error occurred when attempting to get the device's geometry.\n"
        //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
        LocalFree(errormessage);
        return 0;
    }
    if (sectorsize != NULL)
    {
        *sectorsize = (unsigned long long)diskgeometry.Geometry.BytesPerSector;
    }
    return (unsigned long long)diskgeometry.DiskSize.QuadPart / (unsigned long long)diskgeometry.Geometry.BytesPerSector;
}

unsigned long long GetFileSizeInSectors(HANDLE handle, UINT64 sectorsize)
{
    unsigned long long retVal = 0;
    if (sectorsize) // avoid divide by 0
    {
        LARGE_INTEGER filesize;
        if(GetFileSizeEx(handle, &filesize) == 0)
        {
            // error
            wchar_t *errormessage=NULL;
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
            //QString errText = QString::fromUtf16((const ushort *)errormessage);
            //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("File Error"),
            //                      QObject::tr("An error occurred while getting the file size.\n"
            //                                  "Error %1: %2").arg(GetLastError()).arg(errText));
            LocalFree(errormessage);
            retVal = 0;
        }
        else
        {
            retVal = ((unsigned long long)filesize.QuadPart / sectorsize ) + (((unsigned long long)filesize.QuadPart % sectorsize )?1:0);
        }
    }
    return(retVal);
}

BOOL SpaceAvailable(const char *location, UINT64 spaceneeded)
{
    ULARGE_INTEGER freespace;
    BOOL bResult;
    bResult = GetDiskFreeSpaceEx(location, NULL, NULL, &freespace);
    if (!bResult)
    {
        wchar_t *errormessage=NULL;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
        //QString errText = QString::fromUtf16((const ushort *)errormessage);
        //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Free Space Error"),
        //                      QObject::tr("Failed to get the free space on drive %1.\n"
        //                                  "Error %2: %3\n"
        //                                  "Checking of free space will be skipped.").arg(location).arg(GetLastError()).arg(errText));
        return TRUE;
    }
    return (spaceneeded <= freespace.QuadPart);
}

// given a drive letter (ending in a slash), return the label for that drive
// TODO make this more robust by adding input verification
char* GetDriveLabel(char * name, long name_size, const char *drv)
{
    //QString retVal;
    int szNameBuf = MAX_PATH + 1;
    char *nameBuf = NULL;
	if (name_size < (szNameBuf - 1))
	{
		return (name);
	}
    if( (nameBuf = (char *)calloc(szNameBuf, sizeof(char))) != 0 )
    {
        ::GetVolumeInformationA(drv, nameBuf, szNameBuf, NULL,
                                        NULL, NULL, NULL, 0);
    }

    // if malloc fails, nameBuf will be NULL.
    // if GetVolumeInfo fails, nameBuf will contain empty string
    // if all succeeds, nameBuf will contain label
    if(nameBuf == NULL)
    {
		memset(name, 0, name_size);
    }
    else
    {
		memcpy(name, nameBuf, strlen(nameBuf));
        free(nameBuf);
    }

    return(name);
}

BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc,
                      DEVICE_NUMBER *devInfo)
{
    STORAGE_PROPERTY_QUERY Query; // input param for query
    DWORD dwOutBytes; // IOCTL output length
    BOOL bResult; // IOCTL return val
    BOOL retVal = TRUE;
    DWORD cbBytesReturned;

    // specify the query type
    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;

    // Query using IOCTL_STORAGE_QUERY_PROPERTY
    bResult = ::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                &Query, sizeof(STORAGE_PROPERTY_QUERY), pDevDesc,
                pDevDesc->Size, &dwOutBytes, (LPOVERLAPPED)NULL);
    if (bResult)
    {
        bResult = ::DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER,
                    NULL, 0, devInfo, sizeof(DEVICE_NUMBER), &dwOutBytes,
                    (LPOVERLAPPED)NULL);
        if (!bResult)
        {
            retVal = FALSE;
            wchar_t *errormessage=NULL;
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
            //QString errText = QString::fromUtf16((const ushort *)errormessage);
            //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("File Error"),
            //                      QObject::tr("An error occurred while getting the device number.\n"
            //                                  "This usually means something is currently accessing the device;"
            //                                  "please close all applications and try again.\n\nError %1: %2").arg(GetLastError()).arg(errText));
            LocalFree(errormessage);
        }
    }
    else
    {
        if (DeviceIoControl(hDevice, IOCTL_STORAGE_CHECK_VERIFY2, NULL, 0, NULL, 0, &cbBytesReturned,
                            (LPOVERLAPPED) NULL))
        {
            wchar_t *errormessage=NULL;
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
            //QString errText = QString::fromUtf16((const ushort *)errormessage);
            //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("File Error"),
            //                      QObject::tr("An error occurred while querying the properties.\n"
            //                                  "This usually means something is currently accessing the device;"
            //                                  " please close all applications and try again.\n\nError %1: %2").arg(GetLastError()).arg(errText));
            LocalFree(errormessage);
        }
            retVal = FALSE;
    }

    return(retVal);
}

// some routines fail if there's no trailing slash in a name,
// 		others fail if there is.  So this routine takes a name (trailing
// 		slash or no), and creates 2 versions - one with the slash, and one w/o
//
// 		CALLER MUST FREE THE 2 RETURNED STRINGS
BOOL Slashify(char *str, char **slash, char **noSlash)
{
	BOOL retVal = FALSE;
    size_t strLen = strlen(str);
    if ( strLen > 0 )
    {
        if ( *(str + strLen - 1) == '\\' )
        {
            // trailing slash exists
            if (( (*slash = (char *)calloc( (strLen + 1), sizeof(char))) != NULL) &&
                    ( (*noSlash = (char *)calloc(strLen, sizeof(char))) != NULL))
            {
                strncpy(*slash, str, strLen);
                strncpy(*noSlash, *slash, (strLen - 1));
                retVal = TRUE;
            }
        }
        else
        {
            // no trailing slash exists
            if ( ((*slash = (char *)calloc( (strLen + 2), sizeof(char))) != NULL) &&
                 ((*noSlash = (char *)calloc( (strLen + 1), sizeof(char))) != NULL) )
            {
                strncpy(*noSlash, str, strLen);
                sprintf(*slash, "%s\\", *noSlash);
                retVal = TRUE;
            }
        }
    }
    return(retVal);
}

BOOL GetMediaType(HANDLE hDevice)
{
    DISK_GEOMETRY diskGeo;
    DWORD cbBytesReturned;
    if (DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL, 0, &diskGeo, sizeof(diskGeo), &cbBytesReturned, NULL))
    {
        if ((diskGeo.MediaType == FixedMedia) || (diskGeo.MediaType == RemovableMedia))
        {
            return TRUE; // Not a floppy
        }
    }
    return FALSE;
}

BOOL CheckDriveType(char *name, ULONG *pid)
{
    HANDLE hDevice;
    PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
    DEVICE_NUMBER deviceInfo;
    BOOL retVal = FALSE;
    char *nameWithSlash;
    char *nameNoSlash;
    int driveType;
    DWORD cbBytesReturned;

    // some calls require no tailing slash, some require a trailing slash...
    if ( !(Slashify(name, &nameWithSlash, &nameNoSlash)) )
    {
        return(retVal);
    }

    driveType = GetDriveType(nameWithSlash);
    switch( driveType )
    {
    case DRIVE_REMOVABLE: // The media can be removed from the drive.
    case DRIVE_FIXED:     // The media cannot be removed from the drive. Some USB drives report as this.
        hDevice = CreateFile(nameNoSlash, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE)
        {
            wchar_t *errormessage=NULL;
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0, (LPWSTR)&errormessage, 0, NULL);
            //QString errText = QString::fromUtf16((const ushort *)errormessage);
            //QMessageBox::critical(MainWindow::getInstance(), QObject::tr("Volume Error"),
            //                      QObject::tr("An error occurred when attempting to get a handle on %3.\n"
            //                                  "Error %1: %2").arg(GetLastError()).arg(errText).arg(nameWithSlash));
            LocalFree(errormessage);
        }
        else
        {
            int arrSz = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
            pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[arrSz];
            pDevDesc->Size = arrSz;

            // get the device number if the drive is
            // removable or (fixed AND on the usb bus, SD, or MMC (undefined in XP/mingw))
            if(GetMediaType(hDevice) && GetDisksProperty(hDevice, pDevDesc, &deviceInfo) &&
                    ( ((driveType == DRIVE_REMOVABLE) && (pDevDesc->BusType != BusTypeSata))
                      || ( (driveType == DRIVE_FIXED) && ((pDevDesc->BusType == BusTypeUsb)
                      || (pDevDesc->BusType == BusTypeSd ) || (pDevDesc->BusType == BusTypeMmc )) ) ) )
            {
                // ensure that the drive is actually accessible
                // multi-card hubs were reporting "removable" even when empty
                if(DeviceIoControl(hDevice, IOCTL_STORAGE_CHECK_VERIFY2, NULL, 0, NULL, 0, &cbBytesReturned, (LPOVERLAPPED) NULL))
                {
                    *pid = deviceInfo.DeviceNumber;
                    retVal = TRUE;
                }
                else
                // IOCTL_STORAGE_CHECK_VERIFY2 fails on some devices under XP/Vista, try the other (slower) method, just in case.
                {
                    CloseHandle(hDevice);
                    hDevice = CreateFile(nameNoSlash, FILE_READ_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                    if(DeviceIoControl(hDevice, IOCTL_STORAGE_CHECK_VERIFY, NULL, 0, NULL, 0, &cbBytesReturned, (LPOVERLAPPED) NULL))
                    {
                        *pid = deviceInfo.DeviceNumber;
                        retVal = TRUE;
                    }
                }
            }

            delete[] pDevDesc;
            CloseHandle(hDevice);
        }

        break;
    default:
        retVal = FALSE;
    }

    // free the strings allocated by slashify
    free(nameWithSlash);
    free(nameNoSlash);

    return(retVal);
}
// getLogicalDrives sets cBoxDevice with any logical drives found, as long
// as they indicate that they're either removable, or fixed and on USB bus
void GetAllLogicalDrives(std::map<std::string, ULONG>& smap)
{
	// GetLogicalDrives returns 0 on failure, or a bitmask representing
	// the drives available on the system (bit 0 = A:, bit 1 = B:, etc)
	DWORD driveMask = GetLogicalDrives();
	int i = 0;
	ULONG pID;

	while (driveMask != 0)
	{
		if (driveMask & 1)
		{
			// the "A" in drivename will get incremented by the # of bits
			// we've shifted
			char drivename[] = "\\\\.\\A:\\";
			drivename[4] += i;
			if (CheckDriveType(drivename, &pID))
			{
				//cboxDevice->addItem(QString("[%1:\\]").arg(drivename[4]), (qulonglong)pID);
				smap.insert(std::map<std::string, ULONG>::value_type("[" + std::string(&drivename[4], sizeof(char)) + ":\\]", pID));
			}
		}
		driveMask >>= 1;
		//cboxDevice->setCurrentIndex(0);
		++i;
	}
}
// support routine for winEvent - returns the drive letter for a given mask
//   taken from http://support.microsoft.com/kb/163503
char FirstDriveFromMask(ULONG unitmask)
{
	char i;

	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
		{
			break;
		}
		unitmask = unitmask >> 1;
	}

	return (i + 'A');
}