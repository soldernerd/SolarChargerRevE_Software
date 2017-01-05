/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

#ifndef _FS_DEF_
#define _FS_DEF_

#include <stdint.h>

// Macro indicating how many drives can be mounted simultaneously.
#define FILEIO_CONFIG_MAX_DRIVES        1

// Delimiter for directories.
#define FILEIO_CONFIG_DELIMITER '/'

// Macro defining the maximum supported sector size for the FILEIO module.  This value should always be 512 , 1024, 2048, or 4096 bytes.
// Most media uses 512-byte sector sizes.
#define FILEIO_CONFIG_MEDIA_SECTOR_SIZE 		512

/* *******************************************************************************************************/
/************** Compiler options to enable/Disable Features based on user's application ******************/
/* *******************************************************************************************************/

// Uncomment FILEIO_CONFIG_FUNCTION_SEARCH to disable the functions used to search for files.
//#define FILEIO_CONFIG_SEARCH_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_WRITE to disable the functions that write to a drive.  Disabling this feature will
// force the file system into read-only mode.
//#define FILEIO_CONFIG_WRITE_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_FORMAT to disable the function used to format drives.
#define FILEIO_CONFIG_FORMAT_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_DIRECTORY to disable use of directories on your drive.  Disabling this feature will
// limit you to performing all file operations in the root directory.
//#define FILEIO_CONFIG_DIRECTORY_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_PROGRAM_MEMORY_STRINGS to disable functions that accept ROM string arguments.
// This is only necessary on PIC18 parts.
#define FILEIO_CONFIG_PROGRAM_MEMORY_STRINGS_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_DRIVE_PROPERTIES to disable the FILEIO_DrivePropertiesGet function.  This function
// will determine the properties of your device, including unused memory.
//#define FILEIO_CONFIG_DRIVE_PROPERTIES_DISABLE

// Uncomment FILEIO_CONFIG_MULTIPLE_BUFFER_MODE_DISABLE to disable multiple buffer mode.  This will force the library to
// use a single instance of the FAT and Data buffer.  Otherwise, it will use one FAT buffer and one data buffer per drive
// (defined by FILEIO_CONFIG_MAX_DRIVES).  If you are only using one drive in your application, this option has no effect.
//#define FILEIO_CONFIG_MULTIPLE_BUFFER_MODE_DISABLE


//---------------------------------------------------------------------------------------
//The size (in number of sectors) of the desired usable data portion of the MSD volume
//---------------------------------------------------------------------------------------
//Note: Windows 7 appears to require a minimum capacity of at least 13 sectors.
//Note2: Windows will not be able to format a drive if it is too small.  The reason
//for this, is that Windows will try to put a "heavyweight" (comparatively) filesystem
//on the drive, which will consume ~18kB of overhead for the filesystem.  If the total
//drive size is too small to fit the filesystem, then Windows will give an error.
//This also means that formatting the drive will "shrink" the usuable data storage
//area, since the default FAT12 filesystem implemented in the Files.c data tables is very
//lightweight, with very low overhead.
//Note3: It is important to make sure that no part of the MSD volume shares a flash
//erase page with the firmware program memory.  This can be done by using a custom
//modified linker script, or by carefully selecting the starting address and the
//total size of the MSD volume.  See also below code comments.
//Note4: It is also important to make sure that no part of the MSD volume shares
//an erase page with the erase page that contains the microcontroller's configuration
//bits (for microcontrollers that use flash for storing the configuration bits,
//see device datasheet). This can be accomplished by using a modified linker script,
//which protects the flash page with the configuration bits (if applicable), or,
//by carefully choosing the DRV_FILEIO_INTERNAL_FLASH_CONFIG_FILES_ADDRESS and DRV_FILEIO_INTERNAL_FLASH_CONFIG_DRIVE_CAPACITY,
//to make sure the MSD volume does extend into the erase page with the configuration
//bits.
#define DRV_FILEIO_INTERNAL_FLASH_CONFIG_DRIVE_CAPACITY 14


//--------------------------------------------------------------------------
//Starting Address of the MSD Volume.
//--------------------------------------------------------------------------
//Note: Make certain that this starting address is aligned with the start
//of a flash erase block.  It is important to make certain that no part of
//the MSD volume overlaps any portion of a flash erase page which is used
//for storing firmware program code.  When the host writes a sector to the
//MSD volume, the firmware must erase an entire page of flash in order to
//do the write.  If the sector being written happened to share a flash erase
//page with this firmware, unpredictable results would occur, since part of
//firmware would also end up getting erased during the write to the MSD volume.
#define DRV_FILEIO_INTERNAL_FLASH_CONFIG_FILES_ADDRESS 0x2000

#define DRV_FILEIO_INTERNAL_FLASH_CONFIG_UNLOCK_VERIFICATION_FUNCTION SYSTEM_UserSelfWriteUnlockVerification

//--------------------------------------------------------------------------
//Maximum files supported
//--------------------------------------------------------------------------
//DRV_FILEIO_CONFIG_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT must be a multiple of 16
//Note: Even if DRV_FILEIO_CONFIG_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT is 16, this does not
//necessarily mean the drive will always work with 16 files.  The drive will
//suppport "up to" 16 files, but other limits could be hit first, even before
//the drive is full.  The RootDirectory0[] sector could get full with less
//files, especially if the files are using long filenames.
#define DRV_FILEIO_CONFIG_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT 16

//name and extern definition for the master boot record
extern const uint8_t MasterBootRecord[FILEIO_CONFIG_MEDIA_SECTOR_SIZE];
#define MASTER_BOOT_RECORD_ADDRESS &MasterBootRecord[0]

//#define INTERNAL_FLASH_WRITE_PROTECT
#endif
