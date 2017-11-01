
#include <stdint.h>
#include <xc.h>
#include "string.h"
#include "os.h"
#include "external_flash.h"

static FILEIO_MEDIA_INFORMATION mediaInformation;

FILEIO_MEDIA_INFORMATION * FILEIO_ExternalFlash_MediaInitialize(void* config)
{
    mediaInformation.validityFlags.bits.sectorSize = true;
    mediaInformation.sectorSize = FLASH_PAGE_SIZE;
    mediaInformation.errorCode = MEDIA_NO_ERROR;
    return &mediaInformation;
}

uint16_t FILEIO_ExternalFlash_SectorSizeRead(void* config)
{
    //return FILEIO_CONFIG_MEDIA_SECTOR_SIZE;
    return FLASH_PAGE_SIZE;
}

uint32_t FILEIO_ExternalFlash_CapacityRead(void* config)
{
    return ((uint32_t) FLASH_NUMBER_OF_PAGES - 1);
}

uint8_t FILEIO_ExternalFlash_MediaDetect(void* config)
{
    return true;
}

uint8_t FILEIO_ExternalFlash_WriteProtectStateGet(void* config)
{

    return false;
}

/******************************************************************************
 * Function:        uint8_t SectorRead(uint32_t sector_addr, uint8_t *buffer)
 *
 * PreCondition:    None
 *
 * Input:           sector_addr - Sector address, each sector contains 512-byte
 *                  buffer      - Buffer where data will be stored, see
 *                                'ram_acs.h' for 'block' definition.
 *                                'Block' is dependent on whether internal or
 *                                external memory is used
 *
 * Output:          Returns true if read successful, false otherwise
 *
 * Side Effects:    None
 *
 * Overview:        SectorRead reads 512 bytes of data from the card starting
 *                  at the sector address specified by sector_addr and stores
 *                  them in the location pointed to by 'buffer'.
 *
 * Note:            The device expects the address field in the command packet
 *                  to be byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
uint8_t FILEIO_ExternalFlash_SectorRead(void* config, uint32_t sector_addr, uint8_t* buffer)
{
    uint16_t page = (uint16_t) sector_addr;
    
    //Error check.  Make sure the host is trying to read from a legitimate address
    //if(sector_addr >= DRV_FILEIO_INTERNAL_FLASH_TOTAL_DISK_SIZE)
    if(sector_addr >= 4096)
    {
        return false;
    } 
    
    //Read the data
    flash_page_read(page, buffer);

	return true;
}


/******************************************************************************
 * Function:        uint8_t SectorWrite(uint32_t sector_addr, uint8_t *buffer, uint8_t allowWriteToZero)
 *
 * PreCondition:    None
 *
 * Input:           sector_addr - Sector address, each sector contains 512-byte
 *                  buffer      - Buffer where data will be read
 *                  allowWriteToZero - If true, writes to the MBR will be valid
 *
 * Output:          Returns true if write successful, false otherwise
 *
 * Side Effects:    None
 *
 * Overview:        SectorWrite sends 512 bytes of data from the location
 *                  pointed to by 'buffer' to the card starting
 *                  at the sector address specified by sector_addr.
 *
 * Note:            The sample device expects the address field in the command packet
 *                  to be byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
uint8_t FILEIO_ExternalFlash_SectorWrite(void* config, uint32_t sector_addr, uint8_t* buffer, uint8_t allowWriteToZero)
{
    uint16_t page = (uint16_t) sector_addr;

    //First, error check the resulting address
    //if(sector_addr >= DRV_FILEIO_INTERNAL_FLASH_TOTAL_DISK_SIZE)
    if(sector_addr >= 4096)
    {
        return false;
    }  
    
    /*
    //Read page first
    flash_page_read(page, _flash_buffer);
    
    //Compare and check if anything needs to be written
    difference_found = 0;
    for(cntr=0; cntr<512; ++cntr)
    {
        if(buffer[cntr]!=_flash_buffer[cntr])
        {
            difference_found = 1;
        }
        if(difference_found)
        {
            break;
        }
    }
    
    //If no difference is found there's nothing to do, so return
    if(!difference_found)
    {
        return true;
    }
     * */
    
    //Write new data to flash
    flash_page_write(page, buffer);

    return true;
}
