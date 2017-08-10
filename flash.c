
#include <stdint.h>
#include <xc.h>
//#include <plib/pps.h>
//#include <plib/spi.h>

#include "os.h"
#include "flash.h"

#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_PAGE_PROGRAM 0x02
#define FLASH_DATA_READ 0x03
#define FLASH_BLOCK_EREASE 0xD8

uint8_t buffer[265];

void flash_init(void)
{
    uint16_t cntr;
    
    PPSUnLock();
    RPINR21 = SPI_MISO_PPS;
    SPI_MOSI_PPS = PPS_FUNCTION_SPI2_MOSI_OUTPUT;
    SPI_SCLK_PPS = PPS_FUNCTION_SPI2_SCLK_OUTPUT;
    PPSLock();
    
    //PPSInput(PPS_SDI2, SPI_MISO_PPS); //MISO
    //PPSOutput(SPI_MOSI_PPS, PPS_SDO2); //MOSI
    //PPSOutput(SPI_SCLK_PPS, PPS_SCK2); //SCLK
    

    //OpenSPI2(SPI_FOSC_4, MODE_00, SMPMID);
    SSP2STATbits.SMP = 1; //Sample at end
    SSP2STATbits.CKE = 1; //Active to idle
    SSP2CON1bits.CKP = 0; //Idle clock is low
    SSP2CON1bits.SSPM =0b0000; //SPI master mode, Fosc/4
    SSP2CON1bits.SSPEN = 1; //Enable SPI module
}

void flash_dummy_write(void)
{
    uint16_t cntr;
    //This code still needs to be ported from PLIB to something else
    /*
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
    */
}

void flash_dummy_read()
{
    DMACON1bits.SSCON1 = 0;
    DMACON1bits.SSCON0 = 0; //Slave Select not controlled by DMA module
    DMACON1bits.TXINC = 1; //Do increment TX address
    DMACON1bits.RXINC = 1; //Do increment RX address
    DMACON1bits.DUPLEX1 = 1;
    DMACON1bits.DUPLEX0 = 0; //Full duplex
    DMACON1bits.DLYINTEN = 0; //Disable delay interrupts
    DMACON2bits.DLYCYC = 0b0000; //1 cycle delay only
    DMACON2bits.INTLVL = 0b0000; //Only interrupt after transfer is completed
    
    //Set TX buffer address
    //TXADDRH =  ((uint16_t) &buffer[0]) >> 8;
    //TXADDRL = ((uint16_t) &buffer[0]) & 0xFF;
    TXADDRH =  HIGH_BYTE((uint16_t) buffer);
    TXADDRL =  LOW_BYTE((uint16_t)buffer);
    
    //Set RX buffer address
    RXADDRH =  ((uint16_t) buffer) >> 8;
    RXADDRL = ((uint16_t) buffer) & 0xFF;
    
    //Set number of bytes to transmit
    DMABCH = (260-1) >> 8;
    DMABCL = (260-1) & 0xFF;
    
    //Prepare data to send
    buffer[0] = FLASH_DATA_READ;
    buffer[1] = 0x00;
    buffer[2] = 0x37;
    buffer[3] = 0x00;
    
    SPI_SS_PORT = 0;
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete
    SPI_SS_PORT = 1;
}

//void flash_send(uint32_t address, uint8_t *dat, uint16_t len){}
