
#include <stdint.h>
#include <xc.h>
#include "string.h"
#include "hardware_config.h"
#include "os.h"
#include "flash.h"
#include "spi.h"

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
#define FLASH_COMMAND_BUFFER1_READ 0xD1
#define FLASH_COMMAND_BUFFER2_READ 0xD3

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
    FLASH_BUFFER_1, //Buffer 1 is reserved for internal use
    FLASH_BUFFER_2  //Buffer 2 is used for external access
} flashBuffer_t;

typedef enum 
{ 
    DATA_DOES_MATCH,
    DATA_DOES_NOT_MATCH
} flashMatchResult_t;

/*****************************************************************************
 * Function prototypes                                                       *
 *****************************************************************************/

static void _flash_set_page_size(flashPageSize_t size);
static uint16_t _flash_get_status(void);
static void _flash_wakeup(void);
static void _flash_copy_page_to_buffer(uint16_t page, flashBuffer_t buffer);
static flashMatchResult_t _flash_compare_page_to_buffer(uint16_t page, flashBuffer_t buffer);
static void _flash_erase_page(uint16_t page);
static void _flash_write_to_buffer(uint16_t start, uint8_t *data, uint16_t data_length, flashBuffer_t buffer);
static void _flash_write_page_from_buffer(uint16_t page, flashBuffer_t buffer);
static uint8_t _flash_is_busy(void);
void _flash_partial_read(uint16_t page, uint16_t start, uint16_t length, uint8_t *data);
void _flash_buffer_read(uint16_t start, uint16_t length, uint8_t *data, flashBuffer_t buffer);


/*****************************************************************************
 * Global Variables                                                          *
 *****************************************************************************/

flashPowerState_t power_state;
const char flash_command_pagesize_512[4] = FLASH_COMMAND_PAGESIZE_512;
const char flash_command_pagesize_528[4] = FLASH_COMMAND_PAGESIZE_528;


/*****************************************************************************
 * Utility functions                                                         *
 * These are for internal use only and are used to implement the actual      *  
 * flash functionality                                                       *
 * These functions may safely assume the configuration to be set correctly   *
 * They may call each other but must not call public functions               *
 *****************************************************************************/

//Configures flash to use page size of 512 or 528
//In this module, it is set to 512 and never changed again
static void _flash_set_page_size(flashPageSize_t size)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(_flash_is_busy());
    
    //Prepare command buffer
    if(size==FLASH_PAGE_SIZE_512)
        memcpy(command, flash_command_pagesize_512, 4);
    if(size==FLASH_PAGE_SIZE_528)
        memcpy(command, flash_command_pagesize_528, 4);
    
    //Transmit command
    spi_tx(command, 4);
}

//Reads and returns the two status bytes from the flash
static uint16_t _flash_get_status(void)
{
    //This command may be called when the flash is busy
    //Most commonly we use it to CHECK if it is busy
    uint8_t command = FLASH_COMMAND_STATUS_READ;
    uint16_t status;
    spi_tx_rx(&command, 1, (uint8_t*) &status, 2);
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
            spi_tx(&command, 1);
            __delay_us(FLASH_DELAY_WAKEUP_DEEP_POWER_DOWN);
            power_state = FLASH_POWER_STATE_NORMAL;
            break;
            
        case FLASH_POWER_STATE_ULTRA_DEEP_POWER_DOWN:
            //Do not need to send a command
            //Just pull ChipSelect low for some minimum amount of time
            SPI_SS1_PIN = 0;
            __delay_us(FLASH_DELAY_WAKEUP_CHIP_SELECT_LOW);
            SPI_SS1_PIN = 1; 
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
    while(_flash_is_busy());
    
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
    spi_tx(command, 4);
}

//Compare the content of a certain page to the content of one of the ram buffers
static flashMatchResult_t _flash_compare_page_to_buffer(uint16_t page, flashBuffer_t buffer)
{
    uint8_t command[4];
    uint16_t status;
    
    //Wait for flash to be ready
    while(_flash_is_busy());
    
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
    spi_tx(command, 4);
    
    //Wait for flash to be ready
    while(_flash_is_busy());
    
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
    while(_flash_is_busy());
    
    //Configure erase command
    command[0] = FLASH_COMMAND_ERASE_PAGE;
    //Configure address
    command[1] = HIGH_BYTE(page<<1); //High address byte
    command[2] = LOW_BYTE(page<<1); //Middle address byte
    command[3] = 0x00; //Low address byte
    
    //Transmit command
    spi_tx(command, 4);
}

//Write data into one of the ram buffers
static void _flash_write_to_buffer(uint16_t start, uint8_t *data, uint16_t data_length, flashBuffer_t buffer)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(_flash_is_busy());
    
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
    spi_tx_tx(command, 4, data, data_length);
}

//Write the content of one of the ram buffers into a certain page
static void _flash_write_page_from_buffer(uint16_t page, flashBuffer_t buffer)
{
    uint8_t command[4];
    
    //Wait for flash to be ready
    while(_flash_is_busy());
    
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
    spi_tx(command, 4);
}

//This command wakes up the device if it is in one of the low-power modes
//Do not confuse with: uint8_t flash_is_busy(void)
static uint8_t _flash_is_busy(void)
{
    uint16_t status;
    
    //Wake flash up if it is powered down
    if(power_state!=FLASH_POWER_STATE_NORMAL)
    {
        _flash_wakeup();
    }
    
    //Get and check status bytes
    status = _flash_get_status();
    

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

//Reads a partial page from flash
//Do not confuse with: void flash_partial_read(uint16_t page, uint16_t start, uint16_t length, uint8_t *data)
void _flash_partial_read(uint16_t page, uint16_t start, uint16_t length, uint8_t *data)
{
    uint32_t address;
    uint8_t command[4];

    //Wait for flash to be ready
    while(_flash_is_busy());
    
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
    spi_tx_rx(command, 4, data, length);
}

void _flash_buffer_read(uint16_t start, uint16_t length, uint8_t *data, flashBuffer_t buffer)
{
    uint8_t command[4];

    //Wait for flash to be ready
    while(_flash_is_busy());
    
    //Prepare data to send
    if(buffer==FLASH_BUFFER_1)
        command[0] = FLASH_COMMAND_BUFFER1_READ;
    if(buffer==FLASH_BUFFER_2)
        command[0] = FLASH_COMMAND_BUFFER2_READ;
    command[1] = 0x00; //Not used
    command[2] = HIGH_BYTE(start); //Most significant bit only
    command[3] = LOW_BYTE(start); //Least significant 8 bits
    
    //Transmit command and receive data
    spi_tx_rx(command, 4, data, length);
}

/*****************************************************************************
 * Public functions                                                          *
 * Only these functions are made accessible via flash.h                      *  
 * These functions must set (and then reset) the configuration correctly     *
 * They may call static functions but must not call other public functions   *
 *****************************************************************************/

//Initialize the SPI
//After that, initialize the actual flash chip
void flash_init(void)
{
    //Configure and enable MSSP module
    spi_init(SPI_CONFIGURATION_INTERNAL);
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    //Configure flash to operate in 512byte page size mode
    _flash_set_page_size(FLASH_PAGE_SIZE_512);
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
    
}

//Check if flash is currently busy
//This command wakes up the device if it is in one of the low-power modes
//Do not confuse with: static uint8_t _flash_is_busy(void)
uint8_t flash_is_busy(void)
{
    uint8_t result;
    
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    //Check and save status
    result = _flash_is_busy();
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
    
    //Return status
    return result;
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
    
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
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
            while(_flash_is_busy());
            //Send sleep command
            spi_tx(&command, 1);
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
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

//Reads one full 512byte page from flash
void flash_sector_read(uint16_t page, uint8_t *data)
{
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    //Page read is just a special case of a partial read
    _flash_partial_read(page, 0, 512, data);
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

//Writes one full 512 byte page to flash
//This function is smart enough to only write if the data does not already match
void flash_sector_write(uint16_t page, uint8_t *data)
{
    flashMatchResult_t match;
    
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    //Write data to ram buffer 1
    _flash_write_to_buffer(0, data, 512, FLASH_BUFFER_1);
    
    //Compare buffer 1 to the page we want to write to
    match = _flash_compare_page_to_buffer(page, FLASH_BUFFER_1);
    
    //Copy the data from the ram buffer to flash page if (and only if) necessary
    if(match==DATA_DOES_NOT_MATCH)
    {
        _flash_write_page_from_buffer(page, FLASH_BUFFER_1);
    }
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

//Reads a partial page from flash
void flash_partial_read(uint16_t page, uint16_t start, uint16_t length, uint8_t *data)
{
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    //Do the work
    _flash_partial_read(page, start, length, data);
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

//Writes a partial page to flash
//This function is smart enough to only write if the data does not already match
void flash_partial_write(uint16_t page, uint16_t start, uint16_t length, uint8_t *data)
{
    flashMatchResult_t match;
    
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    //Wait for flash to be ready
    while(_flash_is_busy());
    
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
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

void flash_copy_page_to_buffer(uint16_t page)
{
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    _flash_copy_page_to_buffer(page, FLASH_BUFFER_2);
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

void flash_write_page_from_buffer(uint16_t page)
{
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    _flash_write_page_from_buffer(page, FLASH_BUFFER_2);
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

void flash_read_from_buffer(uint16_t start, uint16_t length, uint8_t *data)
{
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    _flash_buffer_read(start, length, data, FLASH_BUFFER_2);
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}

void flash_write_to_buffer(uint16_t start, uint16_t length, uint8_t *data)
{
    //Set configuration
    spi_set_configuration(SPI_CONFIGURATION_INTERNAL);
    
    _flash_write_to_buffer(start, data, length, FLASH_BUFFER_2);
    
    //Reset configuration
    spi_set_configuration(SPI_CONFIGURATION_EXTERNAL);
}
