/* 
 * File:   api.h
 * Author: luke
 *
 * Created on 25. Juli 2018, 11:07
 */

#ifndef API_H
#define	API_H

/******************************************************************************
 * API description
 ******************************************************************************
 * 
 * Data requests. These must be sent as the first byte. Any commands may follow
 *  0x10: General status information
 *  0x11: First 2 lines of display content
 *  0x12: Last 2 lines of display content
 *  0x13: Get calibration part 1
 *  0x14: Get calibration part 2
 *  0x15: External communication configuration
 *  0x20: Echo (i.e. send back) all data received. Used to test connection.
 * 
 * Extended data requests. Only parameters may follow
 *  0x80: Get information for a specific file. Parameter: uint8_t FileNumber
 *  0x81: Find file. Parameter: char[8] FileName, char[3] FileExtention
 *  0x82: Read file. Parameters: uint8_t FileNumber, uint32_t StartByte
 *  0x83: Read buffer. Parameters: uint16_t StartByte
 * 
 * Single byte commands
 *  0x20: Reboot
 *  0x30: Turn output 1 off
 *  0x31: Turn output 1 on
 *  0x32: Turn output 2 off
 *  0x33: Turn output 2 on
 *  0x34: Turn output 3 off
 *  0x35: Turn output 3 on
 *  0x36: Turn output 4 off
 *  0x37: Turn output 4 on
 *  0x38: Turn USB output off
 *  0x39: Turn USB output on
 *  0x3A: Turn FAN output off
 *  0x3B: Turn FAN output 1 on
 *  0x3C: Turn encoder CCW
 *  0x3D: Turn encoder CW
 *  0x3E: Press push button
 *  0x3F: Write time and date to eeprom
 *  0x99: Stop parsing (there are no more commands in this buffer)
 * 
 * Multi byte commands (followed by a 16 bit constant to prevent unintended use)
 * Some legacy commands don't require a constant for now
 *  0x40: Set year. Parameters: uint8_t 2-digit year
 *  0x41: Set month. Parameters: uint8_t month
 *  0x42: Set day. Parameters: uint8_t day    
 *  0x43: Set hours. Parameters: uint8_t hours
 *  0x44: Set minutes. Parameters: uint8_t minutes
 *  0x45: Set seconds. Parameters: uint8_t seconds
 *  0x46: Enable remote buck control. Parameters: uint8_t unused   
 *  0x47: Disable remote buck control. Parameters: uint8_t unused      
 *  0x48: Turn buck on. Parameters: uint8_t unused   
 *  0x49: Turn buck off. Parameters: uint8_t unused         
 *  0x4A: Buck asynchronous mode. Parameters: uint8_t unused 
 *  0x4B: Buck synchronous mode. Parameters: uint8_t unused    
 *  0x4C: Buck decrement dutycycle. Parameters: uint8_t unused
 *  0x4D: Buck increment dutycycle. Parameters: uint8_t unused    
 *  0x4E: Buck set dutycycle. Parameters: uint8_t new_dutycycle
 *  0x50: Resize file. Parameters: uint8_t FileNumber, uint32_t newFileSize, 0x4CEA
 *  0x51: Delete file. Parameters: uint8_t FileNumber, 0x66A0
 *  0x52: Create file. Parameters: char[8] FileName, char[3] FileExtention, uint32_t FileSize, 0xBD4F
 *  0x53: Rename file. Parameters: uint8_t FileNumber, char[8] NewFileName, char[3] NewFileExtention, 0x7E18
 *  0x54: Append to file. Parameters: uint8_t FileNumber, uint8_t NumberOfBytes, 0xFE4B, DATA
 *  0x55: Modify file. Parameters: uint8_t FileNumber, uint32_t StartByte, uint8_t NumerOfBytes, 0x0F9B, DATA
 *  0x56: Format drive. Parameters: none, 0xDA22
 *  0x57: Read file sector to buffer. Parameters: uint8_t file_number, uint16_t sector, 0x1B35
 *  0x58: Write buffer to file sector. Parameters: uint8_t file_number, uint16_t sector, 0x6A6D
 *  0x59: Modify buffer. Parameters: uint16_t StartByte, uint8_t NumerOfBytes, 0xE230, DATA
 *  0x5A: Copy file. Parameters: uint8_t FileNumber, char[8] NewFileName, char[3] NewFileExtention, 0x54D9
 *  0x60: Set calibration. Parameters: uint8_t item_to_set, uint16_t offset_or_slope, uint8_t slope_shift
 *  0x70: Change SPI mode. Parameters: uint8_t NewMode, 0x88E2
 *  0x71: Change SPI frequency. Parameters: uint8_t NewFrequency, 0xAEA8
 *  0x72: Change SPI polarity. Parameters: uint8_t NewPolarity, 0x0DBB
 *  0x73: Change I2C mode. Parameters: uint8_t NewMode, 0xB6B9
 *  0x74: Change I2C frequency. Parameters: uint8_t NewFrequency, 0x4E03
 *  0x75: Change I2C slave mode slave address. Parameters: uint8_t NewAddress, 0x88E2
 *  0x76: Change I2C master mode slave address. Parameters: uint8_t NewAddress, 0x540D
 * 
 *  
 ******************************************************************************/

/******************************************************************************
 * Type definitions
 ******************************************************************************/

typedef enum
{
    DATAREQUEST_GET_COMMAND_RESPONSE = 0x00,
    DATAREQUEST_GET_STATUS = 0x10,
    DATAREQUEST_GET_DISPLAY_1 = 0x11,
    DATAREQUEST_GET_DISPLAY_2 = 0x12,
    DATAREQUEST_GET_CALIBRATION_1 = 0x13,
    DATAREQUEST_GET_CALIBRATION_2 = 0x14,
    DATAREQUEST_GET_ECHO = 0x20,
    DATAREQUEST_GET_FILE_DETAILS = 0x80,
    DATAREQUEST_FIND_FILE = 0x81,
    DATAREQUEST_READ_FILE = 0x82,
    DATAREQUEST_READ_BUFFER = 0x83
} apiDataRequest_t;


            
typedef enum
{
    //Single byte commands
    COMMAND_REBOOT = 0x20,
    COMMAND_OUTPUT_1_OFF = 0x30,
    COMMAND_OUTPUT_1_ON = 0x31,
    COMMAND_OUTPUT_2_OFF = 0x32,
    COMMAND_OUTPUT_2_ON = 0x33,
    COMMAND_OUTPUT_3_OFF = 0x34,
    COMMAND_OUTPUT_3_ON = 0x35,
    COMMAND_OUTPUT_4_OFF = 0x36,
    COMMAND_OUTPUT_4_ON = 0x37,
    COMMAND_OUTPUT_USB_OFF = 0x38,
    COMMAND_OUTPUT_USB_ON = 0x39,
    COMMAND_OUTPUT_FAN_OFF = 0x3A,
    COMMAND_OUTPUT_FAN_ON = 0x3B,
    COMMAND_ENCODER_CCW = 0x3C,
    COMMAND_ENCODER_CW = 0x3D,
    COMMAND_ENCODER_PUSH = 0x3E,
    COMMAND_WRITE_RTCC_EEPROM = 0x3F,
    COMMAND_STOP_PARSING = 0x99,
    //Multi-byte commands
    COMMAND_SET_YEAR = 0x40,
    COMMAND_SET_MONTH = 0x41,
    COMMAND_SET_DAY = 0x42,  
    COMMAND_SET_HOURS = 0x43,
    COMMAND_SET_MINUTES = 0x44,
    COMMAND_SET_SECONDS = 0x45,
    COMMAND_BUCK_REMOTE_ON = 0x46,  
    COMMAND_BUCK_REMOTE_OFF = 0x47,    
    COMMAND_BUCK_ON = 0x48,
    COMMAND_BUCK_OFF = 0x49,      
    COMMAND_BUCK_ASYNCHRONOUS = 0x4A,
    COMMAND_BUCK_SYNCHRONOUS = 0x4B,  
    COMMAND_BUCK_DECREMENT_DUTYCYCLE = 0x4C,
    COMMAND_BUCK_INCREMENT_DUTYCYCLE = 0x4D,  
    COMMAND_BUCK_SET_DUTYCYCLE = 0x4E,     
    COMMAND_FILE_RESIZE = 0x50,
    COMMAND_FILE_DELETE = 0x51,
    COMMAND_FILE_CREATE = 0x52,
    COMMAND_FILE_RENAME = 0x53,
    COMMAND_FILE_APPEND = 0x54,
    COMMAND_FILE_MODIFY = 0x55,
    COMMAND_FORMAT_DRIVE = 0x56,
    COMMAND_SECTOR_TO_BUFFER = 0x57,
    COMMAND_BUFFER_TO_SECTOR = 0x58,
    COMMAND_WRITE_BUFFER = 0x59,
    COMMAND_FILE_COPY = 0x5A,
    COMMAND_SET_CALIBRATION = 0x60,
    COMMAND_SET_SPI_MODE = 0x70,
    COMMAND_SET_SPI_FREQUENCY = 0x71,
    COMMAND_SET_SPI_POLARITY = 0x72,
    COMMAND_SET_I2C_MODE = 0x73,
    COMMAND_SET_I2C_FREQUENCY = 0x74,
    COMMAND_SET_I2C_SLAVE_MODE_ADDRESS = 0x75,
    COMMAND_SET_I2C_MASTER_MODE_ADDRESS = 0x76 
} apiCommand_t;


/******************************************************************************
 * Function prototypes
 ******************************************************************************/

void api_prepare(uint8_t *inBuffer, uint8_t *outBuffer);
void api_parse(uint8_t *inBuffer, uint8_t receivedDataLength, uint8_t *outBuffer);


#endif	/* API_H */

