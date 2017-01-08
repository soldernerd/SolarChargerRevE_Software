
#include <stdint.h>
#include <xc.h>
#include <plib/pps.h>
#include <plib/spi.h>

#include "os.h"
#include "flash.h"

void flash_init(void)
{
    PPSUnLock();
    PPSInput(PPS_SDI2, SPI_MISO_PPS); //MISO
    PPSOutput(SPI_MOSI_PPS, PPS_SDO2); //MOSI
    PPSOutput(SPI_SCLK_PPS, PPS_SCK2); //SCLK
    //PPSOutput(SPI_SS_PPS, PPS_SS2); //SS
    PPSLock();
    
    //MODE_00: SSPSTAT register CKE & CKP
    //#define   SMPEND        0b10000000           // Input data sample at end of data out             
    //#define   SMPMID        0b00000000           // Input data sample at middle of data out
    
    OpenSPI2(SPI_FOSC_4, MODE_00, SMPMID);
    //WriteSPI2(0x77);
    //ReadSPI2();
}

/*
Serial Data Out (SDOx) ?
RC7/RX1/DT1/SDO1/RP18 or
SDO2/Remappable
? Serial Data In (SDIx) ?
RB5/PMA0/KBI1/SDI1/SDA1/RP8 or
SDI2/Remappable
? Serial Clock (SCKx) ?
RB4/PMA1/KBI0/SCK1/SCL1/RP7 or
SCK2/Remappable
Additionally, a fourth pin may be used when in a Slave
mode of operation:
? Slave Select (SSx) ? RA5/AN4/SS1/
HLVDIN/RCV/RP2 or SS2/Remappable
 * */