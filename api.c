
#include <string.h>

#include "system.h"
#include "api.h"
#include "display.h"
#include "os.h"
#include "ui.h"
#include "i2c.h"
#include "fat16.h"
#include "flash.h"
#include "rtcc.h"
#include "buck.h"

/******************************************************************************
 * External variables
 ******************************************************************************/

extern calibration_t calibrationParameters[7];

/******************************************************************************
 * Static function prototypes
 ******************************************************************************/

static void _fill_buffer_get_status(uint8_t *outBuffer);
static void _fill_buffer_get_display(uint8_t *outBuffer, uint8_t secondHalf);
static void _fill_buffer_get_configuration(uint8_t *outBuffer);
static void _fill_buffer_get_calibration1(uint8_t *outBuffer);
static void _fill_buffer_get_calibration2(uint8_t *outBuffer);

static void _fill_buffer_get_file_details(uint8_t *inBuffer, uint8_t *outBuffer);
static void _fill_buffer_find_file(uint8_t *inBuffer, uint8_t *outBuffer);
static void _fill_buffer_read_file(uint8_t *inBuffer, uint8_t *outBuffer);
static void _fill_buffer_read_buffer(uint8_t *inBuffer, uint8_t *outBuffer);

static void _parse_command_short(uint8_t cmd);
static uint8_t _parse_command_long(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);

static uint8_t _parse_file_resize(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_file_delete(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_file_create(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_file_rename(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_file_append(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_file_modify(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_format_drive(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_sector_to_buffer(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_buffer_to_sector(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_write_buffer(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_file_copy(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);

static uint8_t _parse_settings_spi_mode(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_settings_spi_frequency(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_settings_spi_polarity(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_settings_i2c_mode(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_settings_i2c_frequency(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_settings_i2c_slaveModeSlaveAddress(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);
static uint8_t _parse_settings_i2c_masterModeSlaveAddress(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);

static uint8_t _parse_calibration(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr);

static void _fill_buffer_get_display1(uint8_t *outBuffer);

/******************************************************************************
 * Public functions implementation
 ******************************************************************************/


void api_prepare(uint8_t *inBuffer, uint8_t *outBuffer)
{
    apiDataRequest_t command = (apiDataRequest_t) inBuffer[0];
    
    if(command>0x7F)
    {
        //Extended data request, may be followed by parameters (no commands allowed to follow)

        switch(command)
        {
            case DATAREQUEST_GET_FILE_DETAILS:
                //Get file details
                _fill_buffer_get_file_details(inBuffer, outBuffer);
                break;
                
            case DATAREQUEST_FIND_FILE:
                //Find file
                _fill_buffer_find_file(inBuffer, outBuffer);
                break;
                
            case DATAREQUEST_READ_FILE:
                //Read file
                _fill_buffer_read_file(inBuffer, outBuffer);
                break;
                
            case DATAREQUEST_READ_BUFFER:
                //Read file
                _fill_buffer_read_buffer(inBuffer, outBuffer);
                break;
                
            default:
                outBuffer[0] = 0x99;
                outBuffer[1] = 0x99;
        }
    }
    else
    {
        //Normal data request, need no parameters, may be followed by commands
        switch(command)				
        {
            case DATAREQUEST_GET_COMMAND_RESPONSE:
                //nothing to do
                break;
            
            case DATAREQUEST_GET_STATUS:
                //Call function to fill the buffer with general information
                _fill_buffer_get_status(outBuffer);
                break;
                
            case DATAREQUEST_GET_DISPLAY_1:
                //Call function to fill the buffer with general information
                //_fill_buffer_get_display(outBuffer, 0);
                _fill_buffer_get_display1(outBuffer);
                break;

            case DATAREQUEST_GET_DISPLAY_2:
                //Call function to fill the buffer with general information
                _fill_buffer_get_display(outBuffer, 1);
                break;
                
            case DATAREQUEST_GET_CALIBRATION_1:
                //Call function to fill the buffer with general information
                _fill_buffer_get_calibration1(outBuffer);
                break;

            case DATAREQUEST_GET_CALIBRATION_2:
                //Call function to fill the buffer with general information
                _fill_buffer_get_calibration2(outBuffer);
                break;
                
            case DATAREQUEST_GET_ECHO:
                //Copy received data to outBuffer
                memcpy(outBuffer, inBuffer, 64);
                break;                
                
            default:
                outBuffer[0] = 0x99;
                outBuffer[1] = 0x99;
        }
    }
}

void api_parse(uint8_t *inBuffer, uint8_t receivedDataLength, uint8_t *outBuffer)
{
    //Check if the host expects us to do anything else

    uint8_t in_idx;
    uint8_t out_idx;
    uint8_t *out_idx_ptr;
  
    out_idx = 0;
    out_idx_ptr = &out_idx;
    
    if(inBuffer[0]>0x7F)
    {
        //Extended data request. May not be followed by commands.
        //Nothing for us to do here
        return;
    }
    
    if(inBuffer[0]==DATAREQUEST_GET_ECHO)
    {
        //Connectivity is beeing tested. Just echo back whatever we've received
        //Do not try to interpret any of the following bytes as commands
        return;
    }
    
    if(inBuffer[0]==DATAREQUEST_GET_COMMAND_RESPONSE)
    {
        //Echo back to the host PC the command we are fulfilling in the first uint8_t
        outBuffer[0] = DATAREQUEST_GET_COMMAND_RESPONSE;
        
        //Set out_idx, also indicating that we want to provide feedback on the command's execution
        out_idx = 3;
    }
    
    in_idx = 1;
    while(in_idx<receivedDataLength)
    {
        //Check if there is anything more to parse
        if(inBuffer[in_idx]==COMMAND_STOP_PARSING)
        {
            return;
        }
        
        switch(inBuffer[in_idx] & 0xF0)
        {
            case 0x20:
                _parse_command_short(inBuffer[in_idx]);
                ++in_idx;
                break;
                
            case 0x30:
                _parse_command_short(inBuffer[in_idx]);
                ++in_idx;
                break;
                
            case 0x40:
                in_idx += _parse_command_long(&inBuffer[in_idx], outBuffer, out_idx_ptr);
                break;
                
            case 0x50:
                in_idx += _parse_command_long(&inBuffer[in_idx], outBuffer, out_idx_ptr);
                break;
                
            case 0x60:
                in_idx += _parse_command_long(&inBuffer[in_idx], outBuffer, out_idx_ptr);
                break;
                
            default:
                //We should never end up here
                //If we still do, stop parsing this buffer
                return;
        }
    }
}


/******************************************************************************
 * Static functions implementation
 ******************************************************************************/

//Fill buffer with general status information
static void _fill_buffer_get_status(uint8_t *outBuffer)
{
    //Echo back to the host PC the command we are fulfilling in the first uint8_t
    outBuffer[0] = DATAREQUEST_GET_STATUS;
    //Bytes 1-2: input voltage
    outBuffer[1] = (uint8_t) os.input_voltage; //LSB
    outBuffer[2] = os.input_voltage >> 8; //MSB
    //Bytes 3-4: output voltage
    outBuffer[3] = (uint8_t) os.output_voltage; //LSB
    outBuffer[4] = os.output_voltage >> 8; //MSB
    //Bytes 5-6: input current
    outBuffer[5] = (uint8_t) os.input_current; //LSB
    outBuffer[6] = os.input_current >> 8; //MSB
    //Bytes 7-8: output current
    outBuffer[7] = (uint8_t) os.output_current; //LSB
    outBuffer[8] = os.output_current >> 8; //MSB
    //Bytes 9-10: on-board temperature
    outBuffer[9] = (uint8_t) os.temperature_onboard; //LSB
    outBuffer[10] = os.temperature_onboard >> 8; //MSB
    //Bytes 11-12: external temperature 1
    outBuffer[11] = (uint8_t) os.temperature_external_1; //LSB
    outBuffer[12] = os.temperature_external_1 >> 8; //MSB
    //Bytes 13-14: external temperature 2
    outBuffer[13] = (uint8_t) os.temperature_external_2; //LSB
    outBuffer[14] = os.temperature_external_2 >> 8; //MSB
    //Byte 15: Outputs
    outBuffer[15] = os.outputs;
    //Byte 16: Display mode
    outBuffer[16] = os.display_mode;
    //Byte 17-22: Date and time, all in 2-digit DCB
    outBuffer[17] = rtcc_get_year();
    outBuffer[18] = rtcc_get_month();
    outBuffer[19] = rtcc_get_day();
    outBuffer[20] = rtcc_get_hours();
    outBuffer[21] = rtcc_get_minutes();
    outBuffer[22] = rtcc_get_seconds();
    //Charger details
    outBuffer[23] = buck_get_mode();
    outBuffer[24] = buck_get_dutycycle();
    outBuffer[25] = buck_remote_get_status();
    outBuffer[26] = buck_remote_get_dutycycle();
    //Raw ADC data
    outBuffer[27] = (uint8_t) os.temperature_onboard_adc; //LSB
    outBuffer[28] = os.temperature_onboard_adc >> 8; //MSB
    outBuffer[29] = (uint8_t) os.temperature_external_1_adc; //LSB
    outBuffer[30] = os.temperature_external_1_adc >> 8; //MSB
    outBuffer[31] = (uint8_t) os.temperature_external_2_adc; //LSB
    outBuffer[32] = os.temperature_external_2_adc >> 8; //MSB
    outBuffer[33] = (uint8_t) os.input_voltage_adc[(os.timeSlot&0b00110000)>>4]; //LSB
    outBuffer[34] = os.input_voltage_adc[(os.timeSlot&0b00110000)>>4] >> 8; //MSB
    outBuffer[35] = (uint8_t) os.output_voltage_adc[(os.timeSlot&0b00110000)>>4]; //LSB
    outBuffer[36] = os.output_voltage_adc[(os.timeSlot&0b00110000)>>4] >> 8; //MSB
    outBuffer[37] = (uint8_t) os.input_current_adc[(os.timeSlot&0b00110000) >>4]; //LSB
    outBuffer[38] = os.input_current_adc[(os.timeSlot&0b00110000)>>4] >> 8; //MSB
    outBuffer[39] = (uint8_t) os.output_current_adc[(os.timeSlot&0b00110000)>>4]; //LSB
    outBuffer[40] = os.output_current_adc[(os.timeSlot&0b00110000)>>4]>> 8; //MSB    
    //Display status, display off, startup etc
    outBuffer[41] = ui_get_status();
    outBuffer[42] = os.timeSlot;
}

//Fill buffer with display content
static void _fill_buffer_get_display(uint8_t *outBuffer, uint8_t secondHalf)
{
    uint8_t cntr;
    uint8_t line;
    uint8_t start_line;
    uint8_t position;
    
    //Echo back to the host PC the command we are fulfilling in the first uint8_t
    if(secondHalf)
    {
        outBuffer[0] = DATAREQUEST_GET_DISPLAY_2;
    }
    else
    {
        outBuffer[0] = DATAREQUEST_GET_DISPLAY_1;
    }
   
    //Get display data
    cntr = 1;
    if(secondHalf)
    {
        start_line = 2;
    }
    else
    {
        start_line = 0;
    }
    for(line=start_line; line<start_line+2; ++line)
    {
        for(position=0; position<20; ++position)
        {
            outBuffer[cntr] = display_get_character(line, position);
            ++cntr;
        }
    }
}

//Fill buffer with first half of display content
static void _fill_buffer_get_display1(uint8_t *outBuffer)
{
    uint8_t cntr;
    uint8_t line;
    uint8_t position;
    
   //Echo back to the host PC the command we are fulfilling in the first uint8_t
   outBuffer[0] = DATAREQUEST_GET_DISPLAY_1;
   
   //Get display data
   cntr = 1; 
   for(line=0; line<2; ++line)
   {
        for(position=0; position<20; ++position)
        {
            outBuffer[cntr] = display_get_character(line, position);
            ++cntr;
        }
   }
}


//static void _fill_buffer_get_configuration(uint8_t *outBuffer)
//{
//    //Echo back to the host PC the command we are fulfilling in the first uint8_t
//    outBuffer[0] = DATAREQUEST_GET_CONFIGURATION;
//   
//    //SPI settings
//    outBuffer[3] = communicationSettings.spiMode;
//    outBuffer[4] = communicationSettings.spiFrequency;
//    outBuffer[5] = communicationSettings.spiPolarity;
//    
//    //I2C settings
//    outBuffer[6] = communicationSettings.i2cMode;
//    outBuffer[7] = communicationSettings.i2cFrequency;
//    outBuffer[8] = communicationSettings.i2cSlaveModeSlaveAddress;
//    outBuffer[9] = communicationSettings.i2cMasterModeSlaveAddress;
//}

static void _fill_buffer_get_file_details(uint8_t *inBuffer, uint8_t *outBuffer)
{
    uint8_t file_number = inBuffer[1];
    
    //Echo command
    outBuffer[0] = DATAREQUEST_GET_FILE_DETAILS;
    
    //Echo file number
    outBuffer[3] = file_number;
    
    //Return desired data, i.e. entire 32-bit file information struct
    outBuffer[4] = fat_get_file_information(file_number, (rootEntry_t*) &outBuffer[5]);
}

static void _fill_buffer_find_file(uint8_t *inBuffer, uint8_t *outBuffer)
{
    uint8_t cntr;
    
    //Echo command
    outBuffer[0] = DATAREQUEST_FIND_FILE;
    
    //Find and return file number
    outBuffer[3] = fat_find_file(&inBuffer[1], &inBuffer[9]);
    
    //Echo file name and extention
    for(cntr=0; cntr<11; ++cntr)
    {
        outBuffer[cntr+12] = inBuffer[cntr+1];
    }
}

static void _fill_buffer_read_file(uint8_t *inBuffer, uint8_t *outBuffer)
{
    uint32_t start;
    uint32_t file_size;
    uint32_t data_length;
    
    //Echo command
    outBuffer[0] = DATAREQUEST_READ_FILE;
    
    //Echo file number
    outBuffer[3] = inBuffer[1];
    
    //Echo start
    outBuffer[4] = inBuffer[2];
    outBuffer[5] = inBuffer[3];
    outBuffer[6] = inBuffer[4];
    outBuffer[7] = inBuffer[5];
    
    //Calculate start
    start = inBuffer[2];
    start <<= 8;
    start |= inBuffer[3];
    start <<= 8;
    start |= inBuffer[4];
    start <<= 8;
    start |= inBuffer[5];
    
    //Get file size and calculate number of bytes to get
    file_size = fat_get_file_size(inBuffer[1]);
    data_length = file_size - start;
    if(data_length>54)
    {
        //More will not fit into our 64 byte buffer
        data_length = 54;
    }
    
    //Echo data length
    outBuffer[8] = (uint8_t) data_length;
    
    //Read data from file
    outBuffer[9] = fat_read_from_file(inBuffer[1], start, data_length, &outBuffer[10]);
}

static void _fill_buffer_read_buffer(uint8_t *inBuffer, uint8_t *outBuffer)
{
    uint16_t start;
    uint16_t data_length;
    
    //Echo command
    outBuffer[0] = DATAREQUEST_READ_BUFFER;

    //Echo start
    outBuffer[3] = inBuffer[1];
    outBuffer[4] = inBuffer[2];
    
    //Calculate start
    start = inBuffer[1];
    start <<= 8;
    start |= inBuffer[2];
    
    //Get file size and calculate number of bytes to get
    data_length = 512 - start;
    if(data_length>54)
    {
        //More will not fit into our 64 byte buffer
        data_length = 58;
    }
    
    //Echo data length
    outBuffer[5] = (uint8_t) data_length;
    
    //Read data from file
    fat_read_from_buffer(start, data_length, &outBuffer[6]);
}

//Fill buffer with voltage and current measurement parameters
static void _fill_buffer_get_calibration1(uint8_t *outBuffer)
{
   //Echo back to the host PC the command we are fulfilling in the first uint8_t
   outBuffer[0] = DATAREQUEST_GET_CALIBRATION_1;
   //Copy calibration to buffer
   memcpy(&outBuffer[1], &calibrationParameters[CALIBRATION_INDEX_INPUT_VOLTAGE], 12);
   memcpy(&outBuffer[13], &calibrationParameters[CALIBRATION_INDEX_OUTPUT_VOLTAGE], 12);
   memcpy(&outBuffer[25], &calibrationParameters[CALIBRATION_INDEX_INPUT_CURRENT], 12);
   memcpy(&outBuffer[37], &calibrationParameters[CALIBRATION_INDEX_OUTPUT_CURRENT], 12);
}

//Fill buffer with temperature measurement parameters
static void _fill_buffer_get_calibration2(uint8_t *outBuffer)
{
   //Echo back to the host PC the command we are fulfilling in the first uint8_t
   outBuffer[0] = DATAREQUEST_GET_CALIBRATION_2;
   //Copy calibration to buffer
   memcpy(&outBuffer[1], &calibrationParameters[CALIBRATION_INDEX_ONBOARD_TEMPERATURE], 12);
   memcpy(&outBuffer[13], &calibrationParameters[CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_1], 12);
   memcpy(&outBuffer[25], &calibrationParameters[CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_2], 12);
}

static void _parse_command_short(uint8_t cmd)
{
    switch(cmd)
    {   
        case COMMAND_REBOOT:
            reboot();
            break;
            
        case COMMAND_OUTPUT_1_OFF:
            system_output_off(OUTPUT_1);
            break;
            
        case COMMAND_OUTPUT_1_ON:
            system_output_on(OUTPUT_1);
            break;
            
        case COMMAND_OUTPUT_2_OFF:
            system_output_off(OUTPUT_2);
            break;
            
        case COMMAND_OUTPUT_2_ON:
            system_output_on(OUTPUT_2);
            break;
            
        case COMMAND_OUTPUT_3_OFF:
            system_output_off(OUTPUT_3);
            break;
            
        case COMMAND_OUTPUT_3_ON:
            system_output_on(OUTPUT_3);
            break;
            
        case COMMAND_OUTPUT_4_OFF:
            system_output_off(OUTPUT_4);
            break;
            
        case COMMAND_OUTPUT_4_ON:
            system_output_on(OUTPUT_4);
            break;
            
        case COMMAND_OUTPUT_USB_OFF:
            system_output_off(OUTPUT_USB);
            break;
            
        case COMMAND_OUTPUT_USB_ON:
            system_output_on(OUTPUT_USB);
            break;
            
        case COMMAND_OUTPUT_FAN_OFF:
            system_output_off(OUTPUT_FAN);
            break;
            
        case COMMAND_OUTPUT_FAN_ON:
            system_output_on(OUTPUT_FAN);
            break;
           
        case COMMAND_ENCODER_CCW:
            --os.encoderCount;
            break;
            
        case COMMAND_ENCODER_CW:
            ++os.encoderCount;
            break;
            
        case COMMAND_ENCODER_PUSH:
            ++os.buttonCount;
            break;
            
        case COMMAND_WRITE_RTCC_EEPROM:
            rtcc_write_eeprom();
            break;
    }
}

static uint8_t _parse_command_long(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    uint8_t length = 65;
    
    switch(data[0])
    {
        case COMMAND_SET_YEAR:
            rtcc_set_year(data[1]);
            length = 2;
            break;
            
        case COMMAND_SET_MONTH:
            rtcc_set_month(data[1]);
            length = 2;
            break;
            
        case COMMAND_SET_DAY:
            rtcc_set_day(data[1]);
            length = 2;
            break;
            
        case COMMAND_SET_HOURS:
            rtcc_set_hours(data[1]);
            length = 2;
            break;
            
        case COMMAND_SET_MINUTES:
            rtcc_set_minutes(data[1]);
            length = 2;
            break;
            
        case COMMAND_SET_SECONDS:
            rtcc_set_seconds(data[1]);
            length = 2;
            break;
            
        case COMMAND_BUCK_REMOTE_ON:
            buck_remote_set_enable(1);
            length = 2;
            break;
            
        case COMMAND_BUCK_REMOTE_OFF:
            buck_remote_set_enable(0);
            length = 2;
            break;
            
        case COMMAND_BUCK_ON:
            buck_remote_set_on(1);
            length = 2;
            break;
            
        case COMMAND_BUCK_OFF:
            buck_remote_set_on(0);
            length = 2;
            break;
            
        case COMMAND_BUCK_SYNCHRONOUS:
            buck_remote_set_synchronous(0);
            length = 2;
            break;
            
        case COMMAND_BUCK_ASYNCHRONOUS:
            buck_remote_set_synchronous(1);
            length = 2;
            break;
            
        case COMMAND_BUCK_DECREMENT_DUTYCYCLE:
            buck_remote_change_dutycycle(-1);
            length = 2;
            break;
            
        case COMMAND_BUCK_INCREMENT_DUTYCYCLE:
            buck_remote_change_dutycycle(1);
            length = 2;
            break;
            
        case COMMAND_BUCK_SET_DUTYCYCLE:
            buck_remote_set_dutycycle(data[1]);
            length = 2;
            break;
            
        case COMMAND_SET_CALIBRATION:
            length = _parse_calibration(data, out_buffer, out_idx_ptr);
            break;

        case COMMAND_FILE_RESIZE:
            length = _parse_file_resize(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_FILE_DELETE:
            length = _parse_file_delete(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_FILE_CREATE:
            length = _parse_file_create(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_FILE_RENAME:
            length = _parse_file_rename(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_FILE_APPEND:
            length = _parse_file_append(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_FILE_MODIFY:
            length = _parse_file_modify(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_FORMAT_DRIVE:
            length = _parse_format_drive(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_SECTOR_TO_BUFFER:
            length = _parse_sector_to_buffer(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_BUFFER_TO_SECTOR:
            length = _parse_buffer_to_sector(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_WRITE_BUFFER:
            length = _parse_write_buffer(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_FILE_COPY:
            length = _parse_file_copy(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_SET_SPI_MODE:
            length = _parse_settings_spi_mode(data, out_buffer, out_idx_ptr);
            break;
            
        case COMMAND_SET_SPI_FREQUENCY:
            length = _parse_settings_spi_frequency(data, out_buffer, out_idx_ptr);
            break;
          
        case COMMAND_SET_SPI_POLARITY:
            length = _parse_settings_spi_polarity(data, out_buffer, out_idx_ptr);
            break;
          
        case COMMAND_SET_I2C_MODE:
            length = _parse_settings_i2c_mode(data, out_buffer, out_idx_ptr);
            break;
          
        case COMMAND_SET_I2C_FREQUENCY:
            length = _parse_settings_i2c_frequency(data, out_buffer, out_idx_ptr);
            break;
          
        case COMMAND_SET_I2C_SLAVE_MODE_ADDRESS:
            length = _parse_settings_i2c_slaveModeSlaveAddress(data, out_buffer, out_idx_ptr);
            break;
          
        case COMMAND_SET_I2C_MASTER_MODE_ADDRESS:
            length = _parse_settings_i2c_masterModeSlaveAddress(data, out_buffer, out_idx_ptr);
            break; 
    }    
    
    return length;
}

static uint8_t _parse_file_resize(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x50: Resize file. Parameters: uint8_t FileNumber, uint32_t newFileSize, 0x4CEA
    uint32_t file_size;
    uint8_t return_value;
    uint8_t file_number;
    
    if((data[0]!=COMMAND_FILE_RESIZE) || (data[6]!=0x4C) || (data[7]!=0xEA))
    {
        return 8;
    }
    
    //Get file number
    file_number = data[1];
    
    //Calculate file size
    file_size = data[2];
    file_size <<= 8;
    file_size |= data[3];
    file_size <<= 8;
    file_size |= data[4];
    file_size <<= 8;
    file_size |= data[5];
    
    //Resize file
    return_value = fat_resize_file(file_number, file_size);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<58))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FILE_RESIZE;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(HIGH_WORD(file_size));
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(HIGH_WORD(file_size));
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(LOW_WORD(file_size));
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(LOW_WORD(file_size));
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return 8;
}

static uint8_t _parse_file_delete(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x51: Delete file. Parameters: uint8_t FileNumber, 0x66A0
    
    uint8_t return_value;
    uint8_t file_number;
    
    if((data[0]!=COMMAND_FILE_DELETE) || (data[2]!=0x66) || (data[3]!=0xA0))
    {
        return 4;
    }
    
    //Save file number
    file_number = data[1];
    
    //Delete file
    return_value = fat_delete_file(file_number);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<62))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FILE_DELETE;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return 4;
}

static uint8_t _parse_file_create(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x52: Create file. Parameters: char[8] FileName, char[3] FileExtention, uint32_t FileSize, 0xBD4F 
    uint8_t file_number;
    
    uint32_t file_size;
    if((data[0]!=COMMAND_FILE_CREATE) || (data[16]!=0xBD) || (data[17]!=0x4F))
    {
        return 18;
    }
    
    //Calculate file size
    file_size = data[12];
    file_size <<= 8;
    file_size |= data[13];
    file_size <<= 8;
    file_size |= data[14];
    file_size <<= 8;
    file_size |= data[15];
    
    //Create file
    file_number = fat_create_file(&data[1], &data[9], file_size);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FILE_CREATE;
        out_buffer[(*out_idx_ptr)++] = file_number;
    }
    
    return 18;
}

static uint8_t _parse_file_rename(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x53: Rename file. Parameters: uint8_t FileNumber, char[8] NewFileName, char[3] NewFileExtention, 0x7E18
    
    uint8_t file_number;
    uint8_t return_value;
    
    if((data[0]!=COMMAND_FILE_RENAME) || (data[13]!=0x7E) || (data[14]!=0x18))
    {
        return 15;
    }
    
    //Save file number
    file_number = data[1];
    
    //Rename file
    return_value = fat_rename_file(file_number, &data[2], &data[10]);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<62))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FILE_RENAME;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return 15;
}

static uint8_t _parse_file_append(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x54: Append to file. Parameters: uint8_t FileNumber, uint8_t NumberOfBytes, 0xFE4B, DATA
    
    uint8_t file_number;
    uint8_t return_value;
    uint16_t number_of_bytes;
    
    if((data[0]!=COMMAND_FILE_APPEND) || (data[3]!=0xFE) || (data[4]!=0x4B))
    {
        //Can't trust the number of bytes in this case
        return 65;
    }
    
    //Save file number
    file_number = data[1];
    
    //Get number of bytes
    number_of_bytes = (uint16_t) data[2];
    
    //Append to file
    return_value = fat_append_to_file(file_number, number_of_bytes, &data[5]);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<60))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FILE_APPEND;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(number_of_bytes);
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(number_of_bytes);
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return data[2] + 5;
}

static uint8_t _parse_file_modify(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    uint16_t number_of_bytes;
    uint32_t start_byte;
    uint8_t file_number;
    uint8_t return_value;
    
    //0x55: Modify file. Parameters: uint8_t FileNumber, uint32_t StartByte, uint8_t NumerOfBytes, 0x0F9B, DATA
    if((data[0]!=COMMAND_FILE_MODIFY) || (data[7]!=0x0F) || (data[8]!=0x9B))
    {
        //Can't trust the number of bytes in this case
        return 65;
    }
    
    //Save file number
    file_number = data[1];
    
    //Get number_of_bytes
    number_of_bytes = data[6];
    
    //Calculate start byte
    start_byte = data[2];
    start_byte <<= 8;
    start_byte |= data[3];
    start_byte <<= 8;
    start_byte |= data[4];
    start_byte <<= 8;
    start_byte |= data[5];
    
    return_value = fat_modify_file(file_number, start_byte, number_of_bytes, &data[9]);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<56))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FILE_MODIFY;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(HIGH_WORD(start_byte));
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(HIGH_WORD(start_byte));
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(LOW_WORD(start_byte));
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(LOW_WORD(start_byte));
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(number_of_bytes);
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(number_of_bytes);
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return number_of_bytes + 9;
}

static uint8_t _parse_format_drive(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x56: Format drive. Parameters: none, 0xDA22
    
    uint8_t return_value;
    
    if((data[0]!=COMMAND_FORMAT_DRIVE) || (data[1]!=0xDA) || (data[2]!=0x22))
    {
        return 3;
    }
    
    return_value = fat_format();
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FORMAT_DRIVE;
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return 3;
} 

static uint8_t _parse_sector_to_buffer(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x57: Read file sector to buffer. Parameters: uint8_t file_number, uint16_t sector, 0x1B35
    uint8_t file_number;
    uint16_t sector;
    uint8_t return_value;
    
    if((data[0]!=COMMAND_SECTOR_TO_BUFFER) || (data[4]!=0x1B) || (data[5]!=0x35))
    {
        return 6;
    }
    
    //Get file number
    file_number = data[1];
    
    //Calculate sector
    sector = data[2];
    sector <<= 8;
    sector |= data[3];

    //Read desired sector from flash into buffer 2
    return_value = fat_copy_sector_to_buffer(data[1], sector);
    
    //Return result if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<60))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SECTOR_TO_BUFFER;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(sector);
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(sector);
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return 6;
}

static uint8_t _parse_buffer_to_sector(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x58: Write buffer to file sector. Parameters: uint8_t file_number, uint16_t sector, 0x6A6D
    uint8_t file_number;
    uint16_t sector;
    uint8_t return_value;
    
    if((data[0]!=COMMAND_BUFFER_TO_SECTOR) || (data[4]!=0x6A) || (data[5]!=0x6D))
    {
        return 6;
    }
    
    //Get file number
    file_number = data[1];
    
    //Calculate sector
    sector = data[2];
    sector <<= 8;
    sector |= data[3];

    //Write content of buffer 2 to desired flash location
    return_value = fat_write_sector_from_buffer(file_number, sector);
    
    //Return result if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<60))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_BUFFER_TO_SECTOR;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(sector);
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(sector);
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return 6;
}

static uint8_t _parse_write_buffer(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    uint16_t start_byte;
    uint16_t number_of_bytes;
    
    //0x59: Modify buffer. Parameters: uint16_t StartByte, uint8_t NumerOfBytes, 0xE230, DATA
    if((data[0]!=COMMAND_WRITE_BUFFER) || (data[4]!=0xE2) || (data[5]!=0x30))
    {
        //Can't trust the number of bytes in this case
        return 65;
    }
    
    //Calculate sector
    start_byte |= data[1];
    start_byte <<= 8;
    start_byte |= data[2];
    
    //Get number of bytes
    number_of_bytes = data[3];
    
    //Modify content of buffer 2, i.e. write to buffer
    fat_write_to_buffer(start_byte, number_of_bytes, &data[6]);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<61))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_WRITE_BUFFER;
        out_buffer[(*out_idx_ptr)++] = HIGH_BYTE(start_byte);
        out_buffer[(*out_idx_ptr)++] = LOW_BYTE(start_byte);
        out_buffer[(*out_idx_ptr)++] = number_of_bytes;
    }
    
    //Return actual command length
    return 6 + number_of_bytes;
}

static uint8_t _parse_file_copy(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x5A: Copy file. PParameters: uint8_t FileNumber, char[8] NewFileName, char[3] NewFileExtention, 0x54D9
    
    uint8_t file_number;
    uint8_t return_value;
    
    if((data[0]!=COMMAND_FILE_COPY) || (data[13]!=0x54) || (data[14]!=0xD9))
    {
        return 15;
    }
    
    //Save file number
    file_number = data[1];
    
    //Rename file
    return_value = fat_copy_file(file_number, &data[2], &data[10]);
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<62))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_FILE_COPY;
        out_buffer[(*out_idx_ptr)++] = file_number;
        out_buffer[(*out_idx_ptr)++] = return_value;
    }
    
    return 15;
}

static uint8_t _parse_calibration(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    int8_t slope_or_offset;
    calibrationIndex_t item;
    int16_t parameter;
    uint8_t shift;
    
    //Get item
    item =  (calibrationIndex_t) (data[1]>>4);
    
    //Get slope or offset
    slope_or_offset = data[1] & 0x0F;
    
    //Calculate parameter
    parameter = data[2];
    parameter <<= 8;
    parameter |= data[3];
    
    //Get shift
    shift = data[4];
    
    //Store changes in RAM
    switch(slope_or_offset)
    {
        //Offset
        case 0x00:
            calibrationParameters[item].Offset = parameter;
            break;
        //Slope
        case 0x01:
            calibrationParameters[item].Multiplier = parameter;
            calibrationParameters[item].Shift = shift;
            break;
    }
    
    //Schedule changes to be written to EEPROM
    switch(item)
    {
        case CALIBRATION_INDEX_INPUT_VOLTAGE:
            schedule_eeprom_write_task(EEPROM_WRITE_TASK_CALIBRATION_INPUT_VOLTAGE);
            break;
        case CALIBRATION_INDEX_OUTPUT_VOLTAGE:
            schedule_eeprom_write_task(EEPROM_WRITE_TASK_CALIBRATION_OUTPUT_VOLTAGE);
            break;
        case CALIBRATION_INDEX_INPUT_CURRENT:
            schedule_eeprom_write_task(EEPROM_WRITE_TASK_CALIBRATION_INPUT_CURRENT);
            break;
        case CALIBRATION_INDEX_OUTPUT_CURRENT:
            schedule_eeprom_write_task(EEPROM_WRITE_TASK_CALIBRATION_OUTPUT_CURRENT);
            break;
        case CALIBRATION_INDEX_ONBOARD_TEMPERATURE:
            schedule_eeprom_write_task(EEPROM_WRITE_TASK_CALIBRATION_ONBOARD_TEMPERATURE);
            break;
        case CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_1:
            schedule_eeprom_write_task(EEPROM_WRITE_TASK_CALIBRATION_EXTERNAL_TEMPERATURE_1);
            break;
        case CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_2:
            schedule_eeprom_write_task(EEPROM_WRITE_TASK_CALIBRATION_EXTERNAL_TEMPERATURE_2);
            break;
    }
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<62))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_CALIBRATION;
        out_buffer[(*out_idx_ptr)++] = item;
        out_buffer[(*out_idx_ptr)++] = slope_or_offset;
    }
    
    return 5;
}

static uint8_t _parse_settings_spi_mode(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x70: Change SPI mode. Parameters: uint8_t NewMode, 0x88E2
    
    if((data[0]!=COMMAND_SET_SPI_MODE) || (data[2]!=0x88) || (data[3]!=0xE2))
    {
        return 4;
    }
    
    //Save new setting
    os.communicationSettings.spiMode = (externalMode_t) data[1];
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_SPI_MODE;
        out_buffer[(*out_idx_ptr)++] = data[1];
    }
    
    return 4;
}

static uint8_t _parse_settings_spi_frequency(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x71: Change SPI frequency. Parameters: uint8_t NewFrequency, 0xAEA8
    
    
    if((data[0]!=COMMAND_SET_SPI_FREQUENCY) || (data[2]!=0xAE) || (data[3]!=0xA8))
    {
        return 4;
    }
    
    //Save new setting
    os.communicationSettings.spiFrequency = (spiFrequency_t) data[1];
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_SPI_FREQUENCY;
        out_buffer[(*out_idx_ptr)++] = data[1];
    }
    
    return 4;
}

static uint8_t _parse_settings_spi_polarity(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x72: Change SPI polarity. Parameters: uint8_t NewPolarity, 0x0DBB
    
    if((data[0]!=COMMAND_SET_SPI_POLARITY) || (data[2]!=0x0D) || (data[3]!=0xBB))
    {
        return 4;
    }
    
    //Save new setting
    os.communicationSettings.spiFrequency = (spiFrequency_t) data[1];
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_SPI_POLARITY;
        out_buffer[(*out_idx_ptr)++] = data[1];
    }
    
    return 4;
}
            
static uint8_t _parse_settings_i2c_mode(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x73: Change I2C mode. Parameters: uint8_t NewMode, 0xB6B9
    
    if((data[0]!=COMMAND_SET_I2C_MODE) || (data[2]!=0xB6) || (data[3]!=0xB9))
    {
        return 4;
    }
    
    //Save new setting
    os.communicationSettings.i2cMode = (externalMode_t) data[1];
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_I2C_MODE;
        out_buffer[(*out_idx_ptr)++] = data[1];
    }
    
    return 4;
}
    
static uint8_t _parse_settings_i2c_frequency(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x74: Change I2C frequency. Parameters: uint8_t NewFrequency, 0x4E03
    
    if((data[0]!=COMMAND_SET_I2C_FREQUENCY) || (data[2]!=0x4E) || (data[3]!=0x03))
    {
        return 4;
    }
    
    //Save new setting
    os.communicationSettings.i2cFrequency = (i2cFrequency_t) data[1];
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_I2C_FREQUENCY;
        out_buffer[(*out_idx_ptr)++] = data[1];
    }
    
    return 4;
}

static uint8_t _parse_settings_i2c_slaveModeSlaveAddress(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x75: Change I2C slave mode slave address. Parameters: uint8_t NewAddress, 0x88E2
    
    if((data[0]!=COMMAND_SET_I2C_SLAVE_MODE_ADDRESS) || (data[2]!=0x88) || (data[3]!=0xE2))
    {
        return 4;
    }
    
    //Save new setting
    os.communicationSettings.i2cSlaveModeSlaveAddress = data[1];
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_I2C_SLAVE_MODE_ADDRESS;
        out_buffer[(*out_idx_ptr)++] = data[1];
    }
    
    return 4;
}

static uint8_t _parse_settings_i2c_masterModeSlaveAddress(uint8_t *data, uint8_t *out_buffer, uint8_t *out_idx_ptr)
{
    //0x76: Change I2C master mode slave address. Parameters: uint8_t NewAddress, 0x540D
    
    if((data[0]!=COMMAND_SET_I2C_MASTER_MODE_ADDRESS) || (data[2]!=0x54) || (data[3]!=0x0D))
    {
        return 4;
    }
    
    //Save new setting
    os.communicationSettings.i2cMasterModeSlaveAddress = data[1];
    
    //Return confirmation if desired
    if(((*out_idx_ptr)>0) && ((*out_idx_ptr)<63))
    {
        out_buffer[(*out_idx_ptr)++] = COMMAND_SET_I2C_MASTER_MODE_ADDRESS;
        out_buffer[(*out_idx_ptr)++] = data[1];
    }
    
    return 4;
}
