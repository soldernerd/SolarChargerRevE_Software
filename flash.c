
#include <stdint.h>
#include <xc.h>
#include <plib/pps.h>
#include <plib/spi.h>

#include "os.h"
#include "flash.h"

#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_PAGE_PROGRAM 0x02
#define FLASH_DATA_READ 0x03
#define FLASH_BLOCK_EREASE 0xD8

void flash_init(void)
{
    uint16_t cntr;
    
    PPSUnLock();
    PPSInput(PPS_SDI2, SPI_MISO_PPS); //MISO
    PPSOutput(SPI_MOSI_PPS, PPS_SDO2); //MOSI
    PPSOutput(SPI_SCLK_PPS, PPS_SCK2); //SCLK
    PPSLock();

    OpenSPI2(SPI_FOSC_4, MODE_00, SMPMID);
}

void flash_dummy_write(void)
{
    uint16_t cntr;
    
    //Enable writing
    SPI_SS_PORT = 0;
    WriteSPI2(FLASH_WRITE_ENABLE);
    SPI_SS_PORT = 1;
    
    //Write one page of dummy data
    SPI_SS_PORT = 0;
    WriteSPI2(FLASH_PAGE_PROGRAM);
    WriteSPI2(0x00);
    WriteSPI2(0x37);
    WriteSPI2(0x00);
    for(cntr=0;cntr<256;++cntr)
    {
       WriteSPI2(cntr&0xFF); 
    }
    SPI_SS_PORT = 1;
    
}
void flash_dummy_read()
{
    uint16_t cntr;
    
    //Read some dummy data from the location written to by flash_dummy_write())
    SPI_SS_PORT = 0;
    WriteSPI2(FLASH_DATA_READ);
    WriteSPI2(0x00);
    WriteSPI2(0x37);
    WriteSPI2(0x58);
    for(cntr=0;cntr<4;++cntr)
    {
       ReadSPI2();
    }
    SPI_SS_PORT = 1;
}

//void flash_send(uint32_t address, uint8_t *dat, uint16_t len){}
