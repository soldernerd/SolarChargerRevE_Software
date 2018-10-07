
#include <stdint.h>
#include <xc.h>
#include "string.h"
#include "os.h"
#include "external_flash.h"

static FILEIO_MEDIA_INFORMATION mediaInformation;

FILEIO_MEDIA_INFORMATION * ExternalFlash_MediaInitialize(void* config)
{
    mediaInformation.validityFlags.bits.sectorSize = true;
    mediaInformation.sectorSize = FLASH_PAGE_SIZE;
    mediaInformation.errorCode = MEDIA_NO_ERROR;
    return &mediaInformation;
}

uint16_t ExternalFlash_SectorSizeRead(void* config)
{
    return FLASH_PAGE_SIZE;
}

uint32_t ExternalFlash_CapacityRead(void* config)
{
    //This is the truthful answer
    return ((uint32_t) FLASH_NUMBER_OF_PAGES - 1);
}

uint8_t ExternalFlash_MediaDetect(void* config)
{
    return true;
}

uint8_t ExternalFlash_WriteProtectStateGet(void* config)
{

    return false;
}

uint8_t ExternalFlash_SectorRead(void* config, uint32_t sector_addr, uint8_t* buffer)
{
    uint16_t page = (uint16_t) sector_addr;
    
    //Error check.  Make sure the host is trying to read from a legitimate address
    if(sector_addr >= FLASH_NUMBER_OF_PAGES)
    {
        return false;
    } 
    
    //Read the data
    flash_sector_read(page, buffer);

	return true;
}

uint8_t ExternalFlash_SectorWrite(void* config, uint32_t sector_addr, uint8_t* buffer, uint8_t allowWriteToZero)
{
    uint16_t page = (uint16_t) sector_addr;

    //First, error check the resulting address
    if(sector_addr >= FLASH_NUMBER_OF_PAGES)
    {
        return false;
    }  
    
    //Write new data to flash
    flash_sector_write(page, buffer);

    return true;
}
