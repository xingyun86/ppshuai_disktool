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

#ifndef DISK_H
#define DISK_H

#ifndef WINVER
//#define WINVER 0x0601
#endif

//#include <QtWidgets>
//#include <QString>
//#include <cstdio>
//#include <cstdlib>
#include <map>
#include <string>
#include <windows.h>
#include <winioctl.h>

#ifndef FSCTL_IS_VOLUME_MOUNTED
#define FSCTL_IS_VOLUME_MOUNTED  CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif // FSCTL_IS_VOLUME_MOUNTED

typedef struct _DEVICE_NUMBER
{
    DEVICE_TYPE  DeviceType;
    ULONG  DeviceNumber;
    ULONG  PartitionNumber;
} DEVICE_NUMBER, *PDEVICE_NUMBER;

// IOCTL control code
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef enum Status { STATUS_IDLE = 0, STATUS_READING, STATUS_WRITING, STATUS_VERIFYING, STATUS_EXIT, STATUS_CANCELED };

HANDLE GetHandleOnFile(LPCWSTR filelocation, DWORD access);
HANDLE GetHandleOnDevice(int device, DWORD access);
HANDLE GetHandleOnVolume(int volume, DWORD access);
char * GetDriveLabel(char*,long,const char *drv);
DWORD GetDeviceID(HANDLE handle);
BOOL GetLockOnVolume(HANDLE handle);
BOOL RemoveLockOnVolume(HANDLE handle);
BOOL UnmountVolume(HANDLE handle);
BOOL IsVolumeUnmounted(HANDLE handle);
char *ReadSectorDataFromHandle(HANDLE handle, UINT64 startsector, DWORD numsectors, DWORD sectorsize);
BOOL WriteSectorDataToHandle(HANDLE handle, char *data, UINT64 startsector, DWORD numsectors, DWORD sectorsize);
UINT64 GetNumberOfSectors(HANDLE handle, UINT64*sectorsize);
UINT64 GetFileSizeInSectors(HANDLE handle, UINT64 sectorsize);
BOOL SpaceAvailable(const char *location, UINT64 spaceneeded);
BOOL CheckDriveType(char *name, ULONG *pid);
// getLogicalDrives sets cBoxDevice with any logical drives found, as long
// as they indicate that they're either removable, or fixed and on USB bus
void GetAllLogicalDrives(std::map<std::string, ULONG>& smap);
// support routine for winEvent - returns the drive letter for a given mask
//   taken from http://support.microsoft.com/kb/163503
char FirstDriveFromMask(ULONG unitmask);

#endif // DISK_H
