/* 
 * File:   flash.h
 * Author: Luke
 *
 * Created on 8. Januar 2017, 17:13
 */

#ifndef EXTERNAL_FLASH_H
#define	EXTERNAL_FLASH_H

#include "fileio_config.h"
#include <fileio.h>
#include <fileio_media.h>
#include "flash.h"

#define DRV_FILEIO_INTERNAL_FLASH_NUM_RESERVED_SECTORS 1
#define DRV_FILEIO_INTERNAL_FLASH_NUM_VBR_SECTORS 1
#define DRV_FILEIO_INTERNAL_FLASH_NUM_FAT_SECTORS 1
#define DRV_FILEIO_INTERNAL_FLASH_NUM_ROOT_DIRECTORY_SECTORS ((DRV_FILEIO_CONFIG_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT+15)/16) //+15 because the compiler truncates
#define DRV_FILEIO_INTERNAL_FLASH_OVERHEAD_SECTORS (\
            DRV_FILEIO_INTERNAL_FLASH_NUM_RESERVED_SECTORS + \
            DRV_FILEIO_INTERNAL_FLASH_NUM_VBR_SECTORS + \
            DRV_FILEIO_INTERNAL_FLASH_NUM_ROOT_DIRECTORY_SECTORS + \
            DRV_FILEIO_INTERNAL_FLASH_NUM_FAT_SECTORS)
#define DRV_FILEIO_INTERNAL_FLASH_TOTAL_DISK_SIZE (\
            DRV_FILEIO_INTERNAL_FLASH_OVERHEAD_SECTORS + \
            DRV_FILEIO_INTERNAL_FLASH_CONFIG_DRIVE_CAPACITY)
#define DRV_FILEIO_INTERNAL_FLASH_PARTITION_SIZE (uint32_t)(DRV_FILEIO_INTERNAL_FLASH_TOTAL_DISK_SIZE - 1)  //-1 is to exclude the sector used for the MBR

//Functions for mass storage device
FILEIO_MEDIA_INFORMATION * FILEIO_ExternalFlash_MediaInitialize(void* config);
uint16_t FILEIO_ExternalFlash_SectorSizeRead(void* config);
uint32_t FILEIO_ExternalFlash_CapacityRead(void* config);
uint8_t FILEIO_ExternalFlash_MediaDetect(void* config);
uint8_t FILEIO_ExternalFlash_WriteProtectStateGet(void* config);
uint8_t FILEIO_ExternalFlash_SectorRead(void* config, uint32_t sector_addr, uint8_t* buffer);
uint8_t FILEIO_ExternalFlash_SectorWrite(void* config, uint32_t sector_addr, uint8_t* buffer, uint8_t allowWriteToZero);

#endif	/* EXTERNAL_FLASH_H */

