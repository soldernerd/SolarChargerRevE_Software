/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/** INCLUDES *******************************************************/
#include "usb.h"
#include "usb_device_hid.h"
#include <string.h>
#include "system.h"

#include "os.h"
#include "rtcc.h"
#include "display.h"
#include "buck.h"


/** VARIABLES ******************************************************/
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];

volatile USB_HANDLE USBOutHandle;    
volatile USB_HANDLE USBInHandle;

extern calibration_t calibrationParameters[7];

/** DEFINITIONS ****************************************************/
typedef enum
{
    //These are commands from the Microchip USB HID demo, leave them in place for compatibility
    COMMAND_TOGGLE_LED = 0x80,
    COMMAND_GET_BUTTON_STATUS = 0x81,
    COMMAND_READ_POTENTIOMETER = 0x37,
    //These commands are specific to this application
    COMMAND_GET_STATUS = 0x10,
    COMMAND_GET_DISPLAY_1 = 0x11,
    COMMAND_GET_DISPLAY_2 = 0x12,
    COMMAND_GET_CALIBRATION_1 = 0x13,
    COMMAND_GET_CALIBRATION_2 = 0x14
} CUSTOM_HID_DEMO_COMMANDS;

/** FUNCTIONS ******************************************************/
static void _fill_buffer_get_status(void);
static void _fill_buffer_get_display1(void);
static void _fill_buffer_get_display2(void);
static void _fill_buffer_get_calibration1(void);
static void _parse_command_short(uint8_t cmd);
static void _parse_command_long(uint8_t cmd, uint8_t data);


/*********************************************************************
* Function: void APP_DeviceCustomHIDInitialize(void);
*
* Overview: Initializes the Custom HID demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCustomHIDInitialize()
{
    //initialize the variable holding the handle for the last
    // transmission
    USBInHandle = 0;

    //enable the HID endpoint
    USBEnableEndpoint(CUSTOM_DEVICE_HID_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = (volatile USB_HANDLE)HIDRxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ReceivedDataBuffer,64);
}

/*********************************************************************
* Function: void APP_DeviceCustomHIDTasks(void);
*
* Overview: Keeps the Custom HID demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceCustomHIDInitialize() and APP_DeviceCustomHIDStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCustomHIDTasks()
{   
    uint8_t idx;
    
    /* If the USB device isn't configured yet, we can't really do anything
     * else since we don't have a host to talk to.  So jump back to the
     * top of the while loop. */
    if( USBGetDeviceState() < CONFIGURED_STATE )
    {
        return;
    }

    /* If we are currently suspended, then we need to see if we need to
     * issue a remote wakeup.  In either case, we shouldn't process any
     * keyboard commands since we aren't currently communicating to the host
     * thus just continue back to the start of the while loop. */
    if( USBIsDeviceSuspended()== true )
    {
        return;
    }
    
    //Check if we have received an OUT data packet from the host
    if(HIDRxHandleBusy(USBOutHandle) == false)
    {   
        //We just received a packet of data from the USB host.
        //Check the first uint8_t of the packet to see what command the host
        //application software wants us to fulfill.
        //Look at the data the host sent, to see what kind of application specific command it sent.
        switch(ReceivedDataBuffer[0])				
        {
            case COMMAND_GET_STATUS:
                //Check to make sure the endpoint/buffer is free before we modify the contents
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    //Call function to fill the buffer with general information
                    _fill_buffer_get_status();
                    //Prepare the USB module to send the data packet to the host
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;
                
            case COMMAND_GET_DISPLAY_1:
                //Check to make sure the endpoint/buffer is free before we modify the contents
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    //Call function to fill the buffer with general information
                    _fill_buffer_get_display1();
                    //Prepare the USB module to send the data packet to the host
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;
                
            case COMMAND_GET_DISPLAY_2:
                //Check to make sure the endpoint/buffer is free before we modify the contents
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    //Call function to fill the buffer with general information
                    _fill_buffer_get_display2();
                    //Prepare the USB module to send the data packet to the host
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;
                
            case COMMAND_GET_CALIBRATION_1:
                //Check to make sure the endpoint/buffer is free before we modify the contents
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    //Call function to fill the buffer with general information
                    _fill_buffer_get_calibration1();
                    //Prepare the USB module to send the data packet to the host
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;
                
            case COMMAND_TOGGLE_LED:
                //We don't have an LED so we toggle the fan
                FANOUT_PIN ^= 1;
                break;
                
            case COMMAND_GET_BUTTON_STATUS:
                //Check to make sure the endpoint/buffer is free before we modify the contents
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    //Echo back to the host PC the command we are fulfilling in the first uint8_t
                    ToSendDataBuffer[0] = COMMAND_GET_BUTTON_STATUS;
                    if(PUSHBUTTON_PIN) 
                    {
                        //Pin is high so push button is not pressed
                        ToSendDataBuffer[1] = 0x01;
                    }
                    else									
                    {
                        //Pin is low so push button is pressed
                        ToSendDataBuffer[1] = 0x00;
                    }
                    //Prepare the USB module to send the data packet to the host
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;

            case COMMAND_READ_POTENTIOMETER:
                {
                    uint16_t return_value;
                    //Check to make sure the endpoint/buffer is free before we modify the contents
                    if(!HIDTxHandleBusy(USBInHandle))
                    {
                        //Echo back to the host PC the command we are fulfilling in the first uint8_t
                        ToSendDataBuffer[0] = COMMAND_READ_POTENTIOMETER;
                        //We don't have a pot so we echo back the on-board temperature (in 0.01 degrees centigrade) divided by 8
                        return_value = os.temperature_onboard >> 3;
                        ToSendDataBuffer[1] = (uint8_t) return_value; //LSB
                        ToSendDataBuffer[2] = return_value >> 8; //MSB
                        //Prepare the USB module to send the data packet to the host
                        USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                    }
                }
                break;
        }
        
        //Check if the host expects us to do anything else
        idx = 1;
        while(idx<62)
        {
            switch(ReceivedDataBuffer[idx] & 0xF0)
            {
                case 0x30:
                    _parse_command_short(ReceivedDataBuffer[idx]);
                    ++idx;
                    break;
                case 0x40:
                    _parse_command_long(ReceivedDataBuffer[idx], ReceivedDataBuffer[idx+1]);
                    idx += 2;
                    break;
                default:
                    idx = 65; //exit loop
            }
        }

        //Re-arm the OUT endpoint, so we can receive the next OUT data packet 
        //that the host may try to send us.
        USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ReceivedDataBuffer, 64);
    }
}


//Fill buffer with general status information
static void _fill_buffer_get_status(void)
{
    //Echo back to the host PC the command we are fulfilling in the first uint8_t
    ToSendDataBuffer[0] = COMMAND_GET_STATUS;
    //Bytes 1-2: input voltage
    ToSendDataBuffer[1] = (uint8_t) os.input_voltage; //LSB
    ToSendDataBuffer[2] = os.input_voltage >> 8; //MSB
    //Bytes 3-4: output voltage
    ToSendDataBuffer[3] = (uint8_t) os.output_voltage; //LSB
    ToSendDataBuffer[4] = os.output_voltage >> 8; //MSB
    //Bytes 5-6: input current
    ToSendDataBuffer[5] = (uint8_t) os.input_current; //LSB
    ToSendDataBuffer[6] = os.input_current >> 8; //MSB
    //Bytes 7-8: output current
    ToSendDataBuffer[7] = (uint8_t) os.output_current; //LSB
    ToSendDataBuffer[8] = os.output_current >> 8; //MSB
    //Bytes 9-10: on-board temperature
    ToSendDataBuffer[9] = (uint8_t) os.temperature_onboard; //LSB
    ToSendDataBuffer[10] = os.temperature_onboard >> 8; //MSB
    //Bytes 11-12: external temperature 1
    ToSendDataBuffer[11] = (uint8_t) os.temperature_external_1; //LSB
    ToSendDataBuffer[12] = os.temperature_external_1 >> 8; //MSB
    //Bytes 13-14: external temperature 2
    ToSendDataBuffer[13] = (uint8_t) os.temperature_external_2; //LSB
    ToSendDataBuffer[14] = os.temperature_external_2 >> 8; //MSB
    //Byte 15: Outputs
    ToSendDataBuffer[15] = os.outputs;
    //Byte 16: Display mode
    ToSendDataBuffer[16] = os.display_mode;
    //Byte 17-22: Date and time, all in 2-digit DCB
    ToSendDataBuffer[17] = rtcc_get_year();
    ToSendDataBuffer[18] = rtcc_get_month();
    ToSendDataBuffer[19] = rtcc_get_day();
    ToSendDataBuffer[20] = rtcc_get_hours();
    ToSendDataBuffer[21] = rtcc_get_minutes();
    ToSendDataBuffer[22] = rtcc_get_seconds();
    //Charger details
    ToSendDataBuffer[23] = buck_get_mode();
    ToSendDataBuffer[24] = buck_get_dutycycle();
    ToSendDataBuffer[25] = buck_remote_get_status();
    ToSendDataBuffer[26] = buck_remote_get_dutycycle();
    //Raw ADC data
    ToSendDataBuffer[27] = (uint8_t) os.temperature_onboard_adc; //LSB
    ToSendDataBuffer[28] = os.temperature_onboard_adc >> 8; //MSB
    ToSendDataBuffer[29] = (uint8_t) os.temperature_external_1_adc; //LSB
    ToSendDataBuffer[30] = os.temperature_external_1_adc >> 8; //MSB
    ToSendDataBuffer[31] = (uint8_t) os.temperature_external_2_adc; //LSB
    ToSendDataBuffer[32] = os.temperature_external_2_adc >> 8; //MSB
    ToSendDataBuffer[33] = (uint8_t) os.input_voltage_adc[(os.timeSlot&0b00110000)>>4]; //LSB
    ToSendDataBuffer[34] = os.input_voltage_adc[(os.timeSlot&0b00110000)>>4] >> 8; //MSB
    ToSendDataBuffer[35] = (uint8_t) os.output_voltage_adc[(os.timeSlot&0b00110000)>>4]; //LSB
    ToSendDataBuffer[36] = os.output_voltage_adc[(os.timeSlot&0b00110000)>>4] >> 8; //MSB
    ToSendDataBuffer[37] = (uint8_t) os.input_current_adc[(os.timeSlot&0b00110000) >>4]; //LSB
    ToSendDataBuffer[38] = os.input_current_adc[(os.timeSlot&0b00110000)>>4] >> 8; //MSB
    ToSendDataBuffer[39] = (uint8_t) os.output_current_adc[(os.timeSlot&0b00110000)>>4]; //LSB
    ToSendDataBuffer[40] = os.output_current_adc[(os.timeSlot&0b00110000)>>4]>> 8; //MSB    
    //Display status, display off, startup etc
    ToSendDataBuffer[41] = ui_get_status();
    ToSendDataBuffer[42] = os.timeSlot;
}

//Fill buffer with first half of display content
static void _fill_buffer_get_display1(void)
{
    uint8_t cntr;
    uint8_t line;
    uint8_t position;
    
   //Echo back to the host PC the command we are fulfilling in the first uint8_t
   ToSendDataBuffer[0] = COMMAND_GET_DISPLAY_1;
   
   //Get display data
   cntr = 1; 
   for(line=0; line<2; ++line)
   {
        for(position=0; position<20; ++position)
        {
            ToSendDataBuffer[cntr] = display_get_character(line, position);
            ++cntr;
        }
   }
}

//Fill buffer with second half of display content
static void _fill_buffer_get_display2(void)
{
    uint8_t cntr;
    uint8_t line;
    uint8_t position;
    
   //Echo back to the host PC the command we are fulfilling in the first uint8_t
   ToSendDataBuffer[0] = COMMAND_GET_DISPLAY_2;
   
   //Get display data
   cntr = 1; 
   for(line=2; line<4; ++line)
   {
        for(position=0; position<20; ++position)
        {
            ToSendDataBuffer[cntr] = display_get_character(line, position);
            ++cntr;
        }
   }
}


//Fill buffer with voltage and current measurement parameters
static void _fill_buffer_get_calibration1(void)
{
   //Echo back to the host PC the command we are fulfilling in the first uint8_t
   ToSendDataBuffer[0] = COMMAND_GET_CALIBRATION_1;
   //Copy calibration to buffer
   memcpy(&ToSendDataBuffer[1], &calibrationParameters[CALIBRATION_INDEX_INPUT_VOLTAGE], 12);
   memcpy(&ToSendDataBuffer[13], &calibrationParameters[CALIBRATION_INDEX_OUTPUT_VOLTAGE], 12);
   memcpy(&ToSendDataBuffer[25], &calibrationParameters[CALIBRATION_INDEX_INPUT_CURRENT], 12);
   memcpy(&ToSendDataBuffer[37], &calibrationParameters[CALIBRATION_INDEX_OUTPUT_CURRENT], 12);
}

static void _parse_command_short(uint8_t cmd)
{
    switch(cmd)
    {
        case 0x30:
            system_output_off(OUTPUT_1);
            break;
        case 0x31:
            system_output_on(OUTPUT_1);
            break;
        case 0x32:
            system_output_off(OUTPUT_2);
            break;
        case 0x33:
            system_output_on(OUTPUT_2);
            break;
        case 0x34:
            system_output_off(OUTPUT_3);
            break;
        case 0x35:
            system_output_on(OUTPUT_3);
            break;
        case 0x36:
            system_output_off(OUTPUT_4);
            break;
        case 0x37:
            system_output_on(OUTPUT_4);
            break;
        case 0x38:
            system_output_off(OUTPUT_USB);
            break;
        case 0x39:
            system_output_on(OUTPUT_USB);
            break;
        case 0x3A:
            system_output_off(OUTPUT_FAN);
            break;
        case 0x3B:
            system_output_on(OUTPUT_FAN);
            break;
        case 0x3C:
            --os.encoderCount;
            break;
        case 0x3D:
            ++os.encoderCount;
            break;
        case 0x3E:
            ++os.buttonCount;
            break;
        case 0x3F:
            rtcc_write_eeprom();
            break;
    }
}

static void _parse_command_long(uint8_t cmd, uint8_t data)
{
    switch(cmd)
    {
        case 0x40:
            rtcc_set_year(data);
            break;
        case 0x41:
            rtcc_set_month(data);
            break;
        case 0x42:
            rtcc_set_day(data);
            break;
        case 0x43:
            rtcc_set_hours(data);
            break;
        case 0x44:
            rtcc_set_minutes(data);
            break;
        case 0x45:
            rtcc_set_seconds(data);
            break;
        case 0x46:
            buck_remote_set_enable(1);
            break;
        case 0x47:
            buck_remote_set_enable(0);
            break;
        case 0x48:
            buck_remote_set_on(1);
            break;
        case 0x49:
            buck_remote_set_on(0);
            break;
        case 0x4A:
            buck_remote_set_synchronous(0);
            break;
        case 0x4B:
            buck_remote_set_synchronous(1);
            break;
        case 0x4C:
            buck_remote_change_dutycycle(-1);
            break;
        case 0x4D:
            buck_remote_change_dutycycle(1);
            break;
        case 0x4E:
            buck_remote_set_dutycycle(data);
            break;
    }    
}
