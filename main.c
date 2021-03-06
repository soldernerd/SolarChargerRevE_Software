/*
 * File:   main.c
 * Author: Luke
 *
 * Created on 26. December 2016, 21:31
 */


/** INCLUDES *******************************************************/

#include "system.h"

#include "usb.h"
#include "usb_device_hid.h"
#include "usb_device_msd.h"
//
//#include "internal_flash.h"
//
#include "app_device_custom_hid.h"
#include "app_device_msd.h"

//User defined code
#include "hardware_config.h"
#include "os.h"
#include "i2c.h"
#include "display.h"
#include "ui.h"
#include "rtcc.h"
#include "buck.h"
#include "adc.h"
#include "flash.h"
#include "fat16.h"
#include "log.h"


/********************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *******************************************************************/
MAIN_RETURN main(void)
{
    //uint8_t cntr;
    
    //This is a user defined function
    system_init();
    
    SYSTEM_Initialize(SYSTEM_STATE_USB_START);

    USBDeviceInit();
    USBDeviceAttach();
    
    while(1)
    {
        //Do this as often as possible
        APP_DeviceMSDTasks();
        
        if(!os.done)
        {
            //Do this every time
            
            //Clear watchdog timer
            ClrWdt();
            
            //Shut down outputs when battery voltage drops too low
            if(os.output_voltage<USB_CHARGING_VOLTAGE_MINIMUM)
            {
                system_output_off(OUTPUT_USB);
            }
            if(os.output_voltage<POWER_OUTPUTS_VOLTAGE_MINIMUM)
            {
                system_output_off(OUTPUT_1);
                system_output_off(OUTPUT_2);
                system_output_off(OUTPUT_3);
                system_output_off(OUTPUT_4);
            }
            
            //Run scheduled EEPROM write tasks
            i2c_eeprom_tasks();
            
            //Run user interface
            ui_run();
            
            //Measure temperature
            os.temperature_onboard_adc += adc_read(ADC_CHANNEL_TEMPERATURE_ONBOARD);
            os.temperature_external_1_adc += adc_read(ADC_CHANNEL_TEMPERATURE_EXTERNAL_1);
            os.temperature_external_2_adc += adc_read(ADC_CHANNEL_TEMPERATURE_EXTERNAL_2);

            //Run periodic tasks
            switch(os.timeSlot&0b00001111)
            {
                case 0:
                    i2c_adc_start(I2C_ADC_OUTPUT_VOLTAGE, I2C_ADC_RESOLUTION_14BIT, I2C_ADC_GAIN_1);
                    break;

                case 1:
                    APP_DeviceCustomHIDTasks();
                    //flash_page_read(0x1234);
                    //flash_page_read(5);
                    //flash_set_page_size(FLASH_PAGE_SIZE_512);
                    break;
                    
                    
                case 3:
                    os.output_voltage_adc[(os.timeSlot&0b00110000)>>4] = i2c_adc_read();
                    i2c_adc_start(I2C_ADC_INPUT_VOLTAGE, I2C_ADC_RESOLUTION_14BIT, I2C_ADC_GAIN_1);
                    system_calculate_output_voltage();
                    break;
                    
                case 4:
                    if(ui_get_status()==USER_INTERFACE_STATUS_ON)
                    {
                        display_prepare(os.display_mode);
                    }
                    break;

                case 5:
                    if(ui_get_status()==USER_INTERFACE_STATUS_ON)
                    {
                        display_update();
                    }
                    break;
                    
                case 6:
                    os.input_voltage_adc[(os.timeSlot&0b00110000)>>4] = i2c_adc_read();
                    if(1 || buck_get_mode()!=BUCK_STATUS_OFF)
                    {
                        i2c_adc_start(I2C_ADC_OUTPUT_CURRENT, I2C_ADC_RESOLUTION_14BIT, I2C_ADC_GAIN_1); 
                    }
                    system_calculate_input_voltage();
                    break;
                    
                case 8:
                    APP_DeviceCustomHIDTasks();
                    break;

                case 9:
                    if(1 || buck_get_mode()!=BUCK_STATUS_OFF)
                    {
                        os.output_current_adc[(os.timeSlot&0b00110000)>>4] = i2c_adc_read();
                        i2c_adc_start(I2C_ADC_INPUT_CURRENT, I2C_ADC_RESOLUTION_14BIT, I2C_ADC_GAIN_1);
                        system_calculate_output_current();    
                    }
                    break;
                    
                case 11:
                    APP_DeviceCustomHIDTasks();
                    log_collect_data();
//                    if(log_get_number_of_samples() == LOG_PERIOD)
//                    {
//                        log_write_to_disk();
//                        log_start_new();
//                    }
                    break;

                case 12:
                    if(1 || buck_get_mode()!=BUCK_STATUS_OFF)
                    {
                        os.input_current_adc[(os.timeSlot&0b00110000)>>4] = i2c_adc_read();
                        system_calculate_input_current();    
                    }
                    break;

                case 13:
                    buck_operate();
                    break;
                        
                
                case 14:
                    os.temperature_onboard = adc_calculate_temperature(os.temperature_onboard_adc, CALIBRATION_INDEX_ONBOARD_TEMPERATURE);
                    os.temperature_onboard_adc = 0;
                    os.temperature_external_1 = adc_calculate_temperature(os.temperature_external_1_adc, CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_1);
                    os.temperature_external_1_adc = 0;
                    os.temperature_external_2 = adc_calculate_temperature(os.temperature_external_2_adc, CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_2);
                    os.temperature_external_2_adc = 0;  
                    adc_calibrate();
                    
                    if(os.temperature_onboard>3000)
                    {
                        FANOUT_PIN = 1;
                    }
                    else if(os.temperature_onboard<2500)
                    {
                        FANOUT_PIN = 0;
                    }
                    display_prepare(os.display_mode);
                    break;
                    
                case 15:
                    if(ui_get_status()==USER_INTERFACE_STATUS_ON)
                    {
                        display_update();
                    }
                    break;   
            }
            os.done = 1;
        }

        /*
        //Application specific tasks
        APP_DeviceCustomHIDTasks();
        APP_DeviceMSDTasks();
        
        for(cntr=0;cntr<16;++cntr)
        {
            ui_run();
            flash_dummy_read();
            
            adc_calibrate();
            os.temperature_onboard_adc += adc_read(ADC_CHANNEL_TEMPERATURE_ONBOARD);
            os.temperature_external_1_adc += adc_read(ADC_CHANNEL_TEMPERATURE_EXTERNAL_1);
            os.temperature_external_2_adc += adc_read(ADC_CHANNEL_TEMPERATURE_EXTERNAL_2);
            
            if(cntr==14)
            {
                os.temperature_onboard = adc_calculate_temperature(os.temperature_onboard_adc);
                os.temperature_onboard_adc = 0;
                os.temperature_external_1 = adc_calculate_temperature(os.temperature_external_1_adc);
                os.temperature_external_1_adc = 0;
                os.temperature_external_2 = adc_calculate_temperature(os.temperature_external_2_adc);
                os.temperature_external_2_adc = 0;
                display_prepare(os.display_mode);
                
                adc_calibrate();
            }
            if(cntr==15)
            {
                display_update();
            }
            system_delay_ms(8);
        }
        */
    }//end while(1)
}//end main

/*******************************************************************************
 End of File
*/
