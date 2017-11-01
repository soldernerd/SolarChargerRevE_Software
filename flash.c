
#include <stdint.h>
#include <xc.h>
#include "string.h"
#include "os.h"
#include "flash.h"

#define xDEBUG_FLASH

//Just for debugging, remove later
#ifdef DEBUG_FLASH
#include "external_flash.h"
uint8_t _read_internal_flash(uint16_t sector_addr, uint8_t* buffer);
void _flash_copy_from_internal(uint16_t internal_sector, uint16_t external_page);
uint8_t _flash_buffer[512];
#endif
//End of debug

/*****************************************************************************
 * Flash commands, flags and delays                                          *
 *****************************************************************************/

#define FLASH_COMMAND_PAGESIZE_512 {0x3D, 0x2A, 0x80, 0xA6}
#define FLASH_COMMAND_PAGESIZE_528 {0x3D, 0x2A, 0x80, 0xA7}
#define FLASH_COMMAND_STATUS_READ 0xD7
#define FLASH_COMMAND_COPY_TO_BUFFER1 0x53
#define FLASH_COMMAND_COPY_TO_BUFFER2 0x55
#define FLASH_COMMAND_COMPARE_TO_BUFFER1 0x60
#define FLASH_COMMAND_COMPARE_TO_BUFFER2 0x61
#define FLASH_COMMAND_ERASE_PAGE 0x81
#define FLASH_COMMAND_WRITE_TO_BUFFER1 0x84
#define FLASH_COMMAND_WRITE_TO_BUFFER2 0x87
#define FLASH_COMMAND_WRITE_BUFFER1_TO_PAGE 0x83
#define FLASH_COMMAND_WRITE_BUFFER2_TO_PAGE 0x86
#define FLASH_COMMAND_ENTER_DEEP_POWER_DOWN 0xB9
#define FLASH_COMMAND_EXIT_DEEP_POWER_DOWN 0xAB
#define FLASH_COMMAND_ENTER_ULTRA_DEEP_POWER_DOWN 0x79
#define FLASH_COMMAND_PAGE_PROGRAM 0x82
#define FLASH_COMMAND_DATA_READ 0x03

#define FLASH_STATUS_FLAG_BUSY 0b0000000010000000
#define FLASH_STATUS_FLAG_COMPARE 0b0000000001000000

#define FLASH_DELAY_WAKEUP_DEEP_POWER_DOWN 35
#define FLASH_DELAY_WAKEUP_ULTRA_DEEP_POWER_DOWN 120
#define FLASH_DELAY_WAKEUP_CHIP_SELECT_LOW 20

/*****************************************************************************
 * Type definitions                                                          *
 *****************************************************************************/

typedef enum 
{ 
    FLASH_PAGE_SIZE_512,
    FLASH_PAGE_SIZE_528
} flashPageSize_t;

typedef enum 
{ 
    FLASH_BUFFER_1,
    FLASH_BUFFER_2
} flashBuffer_t;

typedef enum 
{ 
    DATA_DOES_MATCH,
    DATA_DOES_NOT_MATCH
} flashMatchResult_t;

/*****************************************************************************
 * Function prototypes                                                       *
 *****************************************************************************/

static void _flash_spi_tx(uint8_t *data, uint16_t length);
static void _flash_spi_tx_tx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length);
static void _flash_spi_tx_rx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length);
static void _flash_set_page_size(flashPageSize_t size);
static uint16_t _flash_get_status(void);
static void _flash_wakeup(void);
static void _flash_copy_page_to_buffer(uint16_t page, flashBuffer_t buffer);
static flashMatchResult_t _flash_compare_page_to_buffer(uint16_t page, flashBuffer_t buffer);
static void _flash_erase_page(uint16_t page);
static void _flash_write_to_buffer(uint16_t start, uint8_t *data, uint16_t data_length, flashBuffer_t buffer);
static void _flash_write_page_from_buffer(uint16_t page, flashBuffer_t buffer);

/*****************************************************************************
 * Global Variables                                                          *
 *****************************************************************************/

flashPowerState_t power_state;
const char flash_command_pagesize_512[4] = FLASH_COMMAND_PAGESIZE_512;
const char flash_command_pagesize_528[4] = FLASH_COMMAND_PAGESIZE_528;

/*****************************************************************************
 * SPI / DMA Functions                                                       *
 * These are for internal use only and are used to implement the actual      *  
 * flash functionality                                                       *
 *****************************************************************************/

//Transmits a number of bytes via SPI using the DMA module
//Typically used to send a command
//Caution: this function does NOT check if the flash is busy or in low-power mode
static void _flash_spi_tx(uint8_t *data, uint16_t length)
{
    //Slave Select not controlled by DMA module
    DMACON1bits.SSCON1 = 0;
    DMACON1bits.SSCON0 = 0; 
    //Do increment TX address
    DMACON1bits.TXINC = 1; 
    //Do not increment RX address
    DMACON1bits.RXINC = 0; 
    //Half duplex, transmit only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 1;
    //Disable delay interrupts
    DMACON1bits.DLYINTEN = 0; 
    //1 cycle delay only
    DMACON2bits.DLYCYC = 0b0000; 
    //Only interrupt after transfer is completed
    DMACON2bits.INTLVL = 0b0000; 
    
    //Set TX buffer address
    TXADDRH =  HIGH_BYTE((uint16_t) data);
    TXADDRL =  LOW_BYTE((uint16_t) data);
    
    //Set number of bytes to transmit
    DMABCH = HIGH_BYTE((uint16_t) (length-1));
    DMABCL = LOW_BYTE((uint16_t) (length-1));
    
    //Perform actual transfer
    SPI_SS_PIN = 0; //Enable slave select pin 
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete
    SPI_SS_PIN = 1; //Disable slave select pin 
}

//Transmits a number of bytes via SPI using the DMA module
//Similar to _flash_spi_tx but uses two different data sources
//Typically used to send a command followed by data
//Caution: this function does NOT check if the flash is busy or in low-power mode
static void _flash_spi_tx_tx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length)
{
    //Slave Select not controlled by DMA module
    DMACON1bits.SSCON1 = 0;
    DMACON1bits.SSCON0 = 0; 
    //Do increment TX address
    DMACON1bits.TXINC = 1; 
    //Do not increment RX address
    DMACON1bits.RXINC = 0; 
    //Half duplex, transmit only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 1;
    //Disable delay interrupts
    DMACON1bits.DLYINTEN = 0; 
    //1 cycle delay only
    DMACON2bits.DLYCYC = 0b0000; 
    //Only interrupt after transfer is completed
    DMACON2bits.INTLVL = 0b0000; 
    
    //Set TX buffer address for command
    TXADDRH =  HIGH_BYTE((uint16_t) command);
    TXADDRL =  LOW_BYTE((uint16_t) command);
    
    //Set number of bytes to transmit for command
    DMABCH = HIGH_BYTE((uint16_t) (command_length-1));
    DMABCL = LOW_BYTE((uint16_t) (command_length-1));
    
    //Enable slave select pin 
    SPI_SS_PIN = 0; 
    
    //Perform transfer of command
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete  
    
    //Set TX buffer address for data
    TXADDRH =  HIGH_BYTE((uint16_t) data);
    TXADDRL =  LOW_BYTE((uint16_t) data);
    
    //Set number of bytes to transmit for actual data
    DMABCH = HIGH_BYTE((uint16_t) (data_length-1));
    DMABCL = LOW_BYTE((uint16_t) (data_length-1));
    
    //Perform transfer of data
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete
    
    //Disable slave select pin 
    SPI_SS_PIN = 1;  
}

//Transmits and then receives a number of bytes via SPI using the DMA module
//Typically used to send a command in order to receive data
//Caution: this function does NOT check if the flash is busy or in low-power mode
static void _flash_spi_tx_rx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length)
{
    //Slave Select not controlled by DMA module
    DMACON1bits.SSCON1 = 0;
    DMACON1bits.SSCON0 = 0; 
    //Do increment TX address
    DMACON1bits.TXINC = 1; 
    //Do not increment RX address
    DMACON1bits.RXINC = 0; 
    //Half duplex, transmit only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 1;
    //Disable delay interrupts
    DMACON1bits.DLYINTEN = 0; 
    //1 cycle delay only
    DMACON2bits.DLYCYC = 0b0000; 
    //Only interrupt after transfer is completed
    DMACON2bits.INTLVL = 0b0000; 
    
    //Set TX buffer address
    TXADDRH =  HIGH_BYTE((uint16_t) command);
    TXADDRL =  LOW_BYTE((uint16_t) command);
    
    //Set number of bytes to transmit
    DMABCH = HIGH_BYTE((uint16_t) (command_length-1));
    DMABCL = LOW_BYTE((uint16_t) (command_length-1));
    
    //Enable slave select pin 
    SPI_SS_PIN = 0; 
    
    //Perform transfer of command
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete  
    
    //Do not increment TX address
    DMACON1bits.TXINC = 0; 
    //Do increment RX address
    DMACON1bits.RXINC = 1; 
    //Half duplex, receive only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 0;
    
    //Set RX buffer address
    RXADDRH =  HIGH_BYTE((uint16_t) data);
    RXADDRL =  LOW_BYTE((uint16_t) data);
    
    //Set number of bytes to transmit
    DMABCH = HIGH_BYTE((uint16_t) (data_length-1));
    DMABCL = LOW_BYTE((uint16_t) (data_length-1));
    
    //Perform transfer of data
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete
    
    //Disable slave select pin 
    SPI_SS_PIN = 1; 
}

/*****************************************************************************
 * Utility functions                                                         *
 * These are for internal use only and are used to implement the actual      *  
 * flash functionality                                                       *
 *****************************************************************************/

//Configures flash to use page size of 512 or 528
//In this module, it is set to 512 and never changed again
static void _flash_set_page_size(flashPageSize_t size)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Prepare command buffer
    if(size==FLASH_PAGE_SIZE_512)
        memcpy(command, flash_command_pagesize_512, 4);
    if(size==FLASH_PAGE_SIZE_528)
        memcpy(command, flash_command_pagesize_528, 4);
    
    //Transmit command
    _flash_spi_tx(command, 4);
}

//Reads and returns the two status bytes from the flash
static uint16_t _flash_get_status(void)
{
    //This command may be called when the flash is busy
    //Most commonly we use it to CHECK if it is busy
    uint8_t command = FLASH_COMMAND_STATUS_READ;
    uint16_t status;
    _flash_spi_tx_rx(&command, 1, (uint8_t*) &status, 2);
    return status;
}

//Wakes the device up from one of the low-power states
static void _flash_wakeup(void)
{
    uint8_t command;
    
    switch(power_state)
    {
        case FLASH_POWER_STATE_DEEP_POWER_DOWN:
            command = FLASH_COMMAND_EXIT_DEEP_POWER_DOWN;
            _flash_spi_tx(&command, 1);
            __delay_us(FLASH_DELAY_WAKEUP_DEEP_POWER_DOWN);
            power_state = FLASH_POWER_STATE_NORMAL;
            break;
            
        case FLASH_POWER_STATE_ULTRA_DEEP_POWER_DOWN:
            //Do not need to send a command
            //Just pull ChipSelect low for some minimum amount of time
            SPI_SS_PIN = 0;
            __delay_us(FLASH_DELAY_WAKEUP_CHIP_SELECT_LOW);
            SPI_SS_PIN = 1; 
            __delay_us(FLASH_DELAY_WAKEUP_ULTRA_DEEP_POWER_DOWN);
            power_state = FLASH_POWER_STATE_NORMAL;
            break;
    }
}

//Copy content from a certain page to one of the ram buffers
static void _flash_copy_page_to_buffer(uint16_t page, flashBuffer_t buffer)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Configure copy command
    if(buffer==FLASH_BUFFER_1)
        command[0] = FLASH_COMMAND_COPY_TO_BUFFER1;
    if(buffer==FLASH_BUFFER_2)
        command[0] = FLASH_COMMAND_COPY_TO_BUFFER2;
    //Configure address
    command[1] = HIGH_BYTE(page<<1); //High address byte
    command[2] = LOW_BYTE(page<<1); //Middle address byte
    command[3] = 0x00; //Low address byte
    
    //Transmit command
    _flash_spi_tx(command, 4);
}

//Compare the content of a certain page to the content of one of the ram buffers
static flashMatchResult_t _flash_compare_page_to_buffer(uint16_t page, flashBuffer_t buffer)
{
    uint8_t command[4];
    uint16_t status;
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Configure compare command
    if(buffer==FLASH_BUFFER_1)
        command[0] = FLASH_COMMAND_COMPARE_TO_BUFFER1;
    if(buffer==FLASH_BUFFER_2)
        command[0] = FLASH_COMMAND_COMPARE_TO_BUFFER1;
    //Configure address
    command[1] = HIGH_BYTE(page<<1); //High address byte
    command[2] = LOW_BYTE(page<<1); //Middle address byte
    command[3] = 0x00; //Low address byte
    
    //Transmit command
    _flash_spi_tx(command, 4);
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Get status
    status = _flash_get_status();
    
    //Check compare flag and return accordingly
    if(status & FLASH_STATUS_FLAG_COMPARE)
    {
        //flag=1 indicates that data does not match
        return DATA_DOES_NOT_MATCH;
    }
    else
    {
        //flag=0 indicates that data does match
        return DATA_DOES_MATCH;
    }
}

//Erase a page
static void _flash_erase_page(uint16_t page)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Configure erase command
    command[0] = FLASH_COMMAND_ERASE_PAGE;
    //Configure address
    command[1] = HIGH_BYTE(page<<1); //High address byte
    command[2] = LOW_BYTE(page<<1); //Middle address byte
    command[3] = 0x00; //Low address byte
    
    //Transmit command
    _flash_spi_tx(command, 4);
}

//Write data into one of the ram buffers
static void _flash_write_to_buffer(uint16_t start, uint8_t *data, uint16_t data_length, flashBuffer_t buffer)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Configure write command
    if(buffer==FLASH_BUFFER_1)
        command[0] = FLASH_COMMAND_WRITE_TO_BUFFER1;
    if(buffer==FLASH_BUFFER_2)
        command[0] = FLASH_COMMAND_WRITE_TO_BUFFER2;
    //Configure address
    command[1] = 0x00;
    command[2] =  HIGH_BYTE(start);
    command[3] = LOW_BYTE(start);
    
    //Transmit command
    _flash_spi_tx_tx(command, 4, data, data_length);
}

//Write the content of one of the ram buffers into a certain page
static void _flash_write_page_from_buffer(uint16_t page, flashBuffer_t buffer)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Configure copy/write command
    if(buffer==FLASH_BUFFER_1)
        command[0] = FLASH_COMMAND_WRITE_BUFFER1_TO_PAGE;
    if(buffer==FLASH_BUFFER_2)
        command[0] = FLASH_COMMAND_WRITE_BUFFER2_TO_PAGE;
    //Configure address
    command[1] = HIGH_BYTE(page<<1); //High address byte
    command[2] = LOW_BYTE(page<<1); //Middle address byte
    command[3] = 0x00; //Low address byte
    
    //Transmit command
    _flash_spi_tx(command, 4);
}

/*****************************************************************************
 * Public functions                                                          *
 * Only these functions are made accessible via flash.h                      *  
 *****************************************************************************/

//Set up PPS pin mappings
//Initialize the SPI
//After that, initialize the actual flash chip
void flash_init(void)
{
    #ifdef DEBUG_FLASH
    uint8_t sector;
    #endif
    
    //Associate pins with MSSP module
    PPSUnLock();
    PPS_FUNCTION_SPI2_MISO_INPUT = SPI_MISO_PPS;
    SPI_MOSI_PPS = PPS_FUNCTION_SPI2_MOSI_OUTPUT;
    //Careful: Clock needs to be mapped as an output AND an input
    SPI_SCLK_PPS_OUT = PPS_FUNCTION_SPI2_SCLK_OUTPUT;
    PPS_FUNCTION_SPI2_SCLK_INPUT = SPI_SCLK_PPS_IN;
    //SPI_SS_PPS = PPS_FUNCTION_SPI2_SS_OUTPUT;
    PPSLock();
    
    //Configure and enable MSSP module
    SSP2STATbits.SMP = 1; //Sample at end
    SSP2STATbits.CKE = 1; //Active to idle
    SSP2CON1bits.CKP = 0; //Idle clock is low
    SSP2CON1bits.SSPM =0b0000; //SPI master mode, Fosc/4
    SSP2CON1bits.SSPEN = 1; //Enable SPI module
    
    //Initialize variables
    power_state = FLASH_POWER_STATE_NORMAL;
    
    //Configure flash to operate in 512byte page size mode
    _flash_set_page_size(FLASH_PAGE_SIZE_512);
    
    //Copy data from internal flash at startup
    //DEBUG ONLY
    #ifdef DEBUG_FLASH
    for(sector=0;sector<18;++sector)
    {
        _flash_copy_from_internal(sector, sector);
    }
    #endif
}

//Check if flash is currently busy
//This command wakes up the device if it is in one of the low-power modes
uint8_t flash_is_busy(void)
{
    //Wake flash up if it is powered down
    if(power_state!=FLASH_POWER_STATE_NORMAL)
    {
        _flash_wakeup();
    }
    
    //Get and check status bytes
    uint16_t status = _flash_get_status();
    if(status & FLASH_STATUS_FLAG_BUSY)
    {
        //Flag=1 indicates that the flash is NOT busy
        return 0;
    }
    else
    {
        //Flag=0 indicates that flash IS busy
        return 1;
    }
}

//Get the current power state of the device
flashPowerState_t flash_get_power_state(void)
{
    return power_state;
}

//Change the current power state
//CAUTION: You can not go from DeepPowerDown to UltraDeepPowerDown and vice versa
void flash_set_power_state(flashPowerState_t new_power_state)
{
    uint8_t command;
    switch(power_state)
    {
        case FLASH_POWER_STATE_NORMAL:
            if(new_power_state==FLASH_POWER_STATE_DEEP_POWER_DOWN)
            {
                command = FLASH_COMMAND_ENTER_DEEP_POWER_DOWN;
            }
            else if(new_power_state==FLASH_POWER_STATE_DEEP_POWER_DOWN)
            {
                command = FLASH_COMMAND_ENTER_ULTRA_DEEP_POWER_DOWN;
            }
            //Wait for flash to be ready
            //The device is currently awake so there's no risk of waking it up
            while(flash_is_busy());
            //Send sleep command
            _flash_spi_tx(&command, 1);
            //Keep track of power state
            power_state = new_power_state;
            break;
            
        default:
            if(new_power_state==FLASH_POWER_STATE_NORMAL)
            {
                _flash_wakeup();
            }
            break;
    }
}

//Reads one full 512byte page from flash
void flash_page_read(uint16_t page, uint8_t *data)
{
    //Page read is just a special case of a partial read
    flash_partial_read(page, 0, 512, data);
}

//Writes one full 512 byte page to flash
//This function is smart enough to only write if the data does not already match
void flash_page_write(uint16_t page, uint8_t *data)
{
    flashMatchResult_t match;
    
    //Write data to ram buffer 1
    _flash_write_to_buffer(0, data, 512, FLASH_BUFFER_1);
    
    //Compare buffer 1 to the page we want to write to
    match = _flash_compare_page_to_buffer(page, FLASH_BUFFER_1);
    
    //Copy the data from the ram buffer to flash page if (and only if) necessary
    if(match==DATA_DOES_NOT_MATCH)
    {
        _flash_write_page_from_buffer(page, FLASH_BUFFER_1);
    }
}

//Reads a partial page from flash
void flash_partial_read(uint16_t page, uint16_t start, uint16_t length, uint8_t *data)
{
    uint32_t address;
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(flash_is_busy());
    
    //Calculate address from page and start
    address = page;
    address <<= 9;
    address |= start;
    
    //Prepare data to send
    command[0] = FLASH_COMMAND_DATA_READ; //Command
    command[1] = LOW_BYTE(HIGH_WORD(address)); //High address byte
    command[2] = HIGH_BYTE(LOW_WORD(address)); //Middle address byte
    command[3] = LOW_BYTE(LOW_WORD(address)); //Low address byte
    
    //Transmit command and receive data
    _flash_spi_tx_rx(command, 4, data, length);    
}

//Writes a partial page to flash
//This function is smart enough to only write if the data does not already match
void flash_partial_write(uint16_t page, uint16_t start, uint16_t length, uint8_t *data)
{
    flashMatchResult_t match;
    
    //Copy data from page to ram buffer 1
    _flash_copy_page_to_buffer(page, FLASH_BUFFER_1); 
    
    //Overwrite part of the ram buffer with our new data
    _flash_write_to_buffer(start, data, length, FLASH_BUFFER_1);
    
    //Compare buffer 1 to the page we want to write to
    match = _flash_compare_page_to_buffer(page, FLASH_BUFFER_1);
    
    //Copy the data from the ram buffer to flash page if (and only if) necessary
    if(match==DATA_DOES_NOT_MATCH)
    {
        _flash_write_page_from_buffer(page, FLASH_BUFFER_1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Everything below here is just for debugging and will be deleted later on
////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_FLASH
uint8_t _read_internal_flash(uint16_t sector_addr, uint8_t* buffer)
{
    //Error check.  Make sure the host is trying to read from a legitimate
    //address, which corresponds to the MSD volume (and not some other program
    //memory region beyond the end of the MSD volume).
    if(sector_addr >= DRV_FILEIO_INTERNAL_FLASH_TOTAL_DISK_SIZE)
    {
        return 0;
    }   
    
    //Read a sector worth of data, and copy it to the specified RAM "buffer".
    memcpy
    (
        (void*)buffer,
        (const void*)(MASTER_BOOT_RECORD_ADDRESS + (sector_addr * FILEIO_CONFIG_MEDIA_SECTOR_SIZE)),
        FILEIO_CONFIG_MEDIA_SECTOR_SIZE
    );

	return 1;
}

void _flash_copy_from_internal(uint16_t internal_sector, uint16_t external_page)
{
    _read_internal_flash(internal_sector, _flash_buffer);
    flash_page_write(external_page, _flash_buffer);
}
#endif