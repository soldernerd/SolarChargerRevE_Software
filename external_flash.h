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
//This module provides the link between app_device_msd.h and flash.h
//app_device_msd.h is concerned with the USB mass storage and doesn't care about how the data is stored
//flash.h implements the communication with our flash chip and doesn't care about USB
FILEIO_MEDIA_INFORMATION * ExternalFlash_MediaInitialize(void* config);
uint16_t ExternalFlash_SectorSizeRead(void* config);
uint32_t ExternalFlash_CapacityRead(void* config);
uint8_t ExternalFlash_MediaDetect(void* config);
uint8_t ExternalFlash_WriteProtectStateGet(void* config);
uint8_t ExternalFlash_SectorRead(void* config, uint32_t sector_addr, uint8_t* buffer);
uint8_t ExternalFlash_SectorWrite(void* config, uint32_t sector_addr, uint8_t* buffer, uint8_t allowWriteToZero);

#endif	/* EXTERNAL_FLASH_H */

