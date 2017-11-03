/* 
 * File:   flash.h
 * Author: Luke
 *
 * Created on 8. Januar 2017, 17:13
 */

#ifndef EXTERNAL_FLASH_H
#define	EXTERNAL_FLASH_H

#include <fileio.h>
#include <fileio_media.h>
#include "flash.h"

//Functions for mass storage device
FILEIO_MEDIA_INFORMATION * ExternalFlash_MediaInitialize(void* config);
uint16_t ExternalFlash_SectorSizeRead(void* config);
uint32_t ExternalFlash_CapacityRead(void* config);
uint8_t ExternalFlash_MediaDetect(void* config);
uint8_t ExternalFlash_WriteProtectStateGet(void* config);
uint8_t ExternalFlash_SectorRead(void* config, uint32_t sector_addr, uint8_t* buffer);
uint8_t ExternalFlash_SectorWrite(void* config, uint32_t sector_addr, uint8_t* buffer, uint8_t allowWriteToZero);

#endif	/* EXTERNAL_FLASH_H */

