#include <xc.h>
#include <stdint.h>

#include "os.h"
#include "i2c.h"
//#include "rtcc.h"
#include "ui.h"

userInterfaceStatus_t userInterfaceStatus;
uint16_t system_ui_inactive_count;

userInterfaceStatus_t ui_get_status(void)
{
    return userInterfaceStatus;
}

static void _ui_encoder(void)
{
    switch(os.display_mode)
    {
        case DISPLAY_MODE_OVERVIEW:
            if(os.encoderCount>0)
            {
                os.display_mode = DISPLAY_MODE_DATETIME_OVERVIEW;
            }
            else if(os.encoderCount<0)
            {
                os.display_mode = DISPLAY_MODE_TEMPERATURE;
            }
            if(os.buttonCount)
            {
               ; 
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_DATETIME_OVERVIEW:
            if(os.encoderCount>0)
            {
                os.display_mode = DISPLAY_MODE_USB_CHARGER;
            }
            else if(os.encoderCount<0)
            {
                os.display_mode = DISPLAY_MODE_OVERVIEW;
            }
            if(os.buttonCount)
            {
               os.display_mode = DISPLAY_MODE_DATETIME_YEAR;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_DATETIME_YEAR:
            if(os.encoderCount>0)
            {
                rtcc_increment_year();
            }
            else if(os.encoderCount<0)
            {
                rtcc_decrement_year();
            }
            if(os.buttonCount)
            {
                rtcc_correct_day(); 
                os.display_mode = DISPLAY_MODE_DATETIME_MONTH;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_DATETIME_MONTH:
            if(os.encoderCount>0)
            {
                rtcc_increment_month();
            }
            else if(os.encoderCount<0)
            {
                rtcc_decrement_month();
            }
            if(os.buttonCount)
            {
                rtcc_correct_day();
                os.display_mode = DISPLAY_MODE_DATETIME_DAY;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_DATETIME_DAY:
            if(os.encoderCount>0)
            {
                rtcc_increment_day();
            }
            else if(os.encoderCount<0)
            {
                rtcc_decrement_day();
            }
            if(os.buttonCount)
            {
               os.display_mode = DISPLAY_MODE_DATETIME_HOURS;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_DATETIME_HOURS:
            if(os.encoderCount>0)
            {
                rtcc_increment_hours();
            }
            else if(os.encoderCount<0)
            {
                rtcc_decrement_hours();
            }
            if(os.buttonCount)
            {
               os.display_mode = DISPLAY_MODE_DATETIME_MINUTES;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_DATETIME_MINUTES:
            if(os.encoderCount>0)
            {
                rtcc_increment_minutes();
            }
            else if(os.encoderCount<0)
            {
                rtcc_decrement_minutes();
            }
            if(os.buttonCount)
            {
               os.display_mode = DISPLAY_MODE_DATETIME_SECONDS;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_DATETIME_SECONDS:
            if(os.encoderCount>0)
            {
                rtcc_increment_seconds();
            }
            else if(os.encoderCount<0)
            {
                rtcc_decrement_seconds();
            }
            if(os.buttonCount)
            {
               os.display_mode = DISPLAY_MODE_DATETIME_OVERVIEW;
               rtcc_write_eeprom();
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_USB_CHARGER:
            if(os.encoderCount>0)
            {
                os.display_mode = DISPLAY_MODE_OUTPUTS;
            }
            else if(os.encoderCount<0)
            {
                os.display_mode = DISPLAY_MODE_DATETIME_OVERVIEW;
            }
            if(os.buttonCount)
            {
                if(os.output_voltage>USB_CHARGING_VOLTAGE_MINIMUM)
                    //i2c_expander_toggle(I2C_EXPANDER_USB_CHARGER);
                    ;//system_output_toggle(OUTPUT_USB);
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_OUTPUTS:
            if(os.encoderCount>0)
            {
                os.display_mode = DISPLAY_MODE_CHARGER_DETAILS;
            }
            else if(os.encoderCount<0)
            {
                os.display_mode = DISPLAY_MODE_USB_CHARGER;
            }
            if(os.buttonCount)
            {
                os.display_mode = DISPLAY_MODE_OUTPUTS_1;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_OUTPUTS_1:
            if(os.encoderCount>0)
            {
                if(os.output_voltage>POWER_OUTPUTS_VOLTAGE_MINIMUM)
                    ;//system_output_on(OUTPUT_1);
            }
            else if(os.encoderCount<0)
            {
                ;//system_output_off(OUTPUT_1);
            }
            if(os.buttonCount)
            {
                os.display_mode = DISPLAY_MODE_OUTPUTS_2;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_OUTPUTS_2:
            if(os.encoderCount>0)
            {
                if(os.output_voltage>POWER_OUTPUTS_VOLTAGE_MINIMUM)
                    ;//system_output_on(OUTPUT_2);
            }
            else if(os.encoderCount<0)
            {
                ;//system_output_off(OUTPUT_2);
            }
            if(os.buttonCount)
            {
                os.display_mode = DISPLAY_MODE_OUTPUTS_3;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_OUTPUTS_3:
            if(os.encoderCount>0)
            {
                if(os.output_voltage>POWER_OUTPUTS_VOLTAGE_MINIMUM)
                    ;//system_output_on(OUTPUT_3);
            }
            else if(os.encoderCount<0)
            {
                ;//system_output_off(OUTPUT_3);
            }
            if(os.buttonCount)
            {
                os.display_mode = DISPLAY_MODE_OUTPUTS_4;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_OUTPUTS_4:
            if(os.encoderCount>0)
            {
                if(os.output_voltage>POWER_OUTPUTS_VOLTAGE_MINIMUM)
                    ;//system_output_on(OUTPUT_4);
            }
            else if(os.encoderCount<0)
            {
                ;//system_output_off(OUTPUT_4);
            }
            if(os.buttonCount)
            {
                os.display_mode = DISPLAY_MODE_OUTPUTS;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        case DISPLAY_MODE_CHARGER_DETAILS:
            if(os.encoderCount>0)
            {
                os.display_mode = DISPLAY_MODE_EFFICIENCY;
            }
            else if(os.encoderCount<0)
            {
                os.display_mode = DISPLAY_MODE_OUTPUTS;
            }
            if(os.buttonCount)
            {
                ;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

            case DISPLAY_MODE_EFFICIENCY:
            if(os.encoderCount>0)
            {
                os.display_mode = DISPLAY_MODE_TEMPERATURE;
            }
            else if(os.encoderCount<0)
            {
                os.display_mode = DISPLAY_MODE_CHARGER_DETAILS;
            }
            if(os.buttonCount)
            {
                ;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

            case DISPLAY_MODE_TEMPERATURE:
            if(os.encoderCount>0)
            {
                os.display_mode = DISPLAY_MODE_OVERVIEW;
            }
            else if(os.encoderCount<0)
            {
                os.display_mode = DISPLAY_MODE_EFFICIENCY;
            }
            if(os.buttonCount)
            {
                ;
            }
            os.encoderCount = 0;
            os.buttonCount = 0;
            break;

        default:
            os.display_mode = DISPLAY_MODE_OVERVIEW;
    }    
}

void ui_init(void)
{
    system_ui_inactive_count = 0;
    //Enable high (3.3volts) board voltage
    VCC_HIGH_PORT = 1;
    userInterfaceStatus = USER_INTERFACE_STATUS_STARTUP_1;
}


void ui_run(void)
{
	switch(userInterfaceStatus)
	{
		case USER_INTERFACE_STATUS_OFF:
			if (os.buttonCount!=0)
			{
				//Enable high (3.3volts) board voltage
                VCC_HIGH_PORT = 1;
                os.buttonCount = 0;
                //Proceed to next state
                system_ui_inactive_count = 0;
				userInterfaceStatus = USER_INTERFACE_STATUS_STARTUP_1;
			}
			break;

		case USER_INTERFACE_STATUS_STARTUP_1:
            //Switch CPU to 48MHz operating frequency
            //if(os.clockFrequency==CPU_FREQUENCY_32kHz)
            //{
                //system_set_cpu_frequency(CPU_FREQUENCY_48MHz);
            //}
            //Enable the user interface
            DISP_EN_PORT = 0;
            //Proceed to next state
			system_ui_inactive_count = 0;
			userInterfaceStatus = USER_INTERFACE_STATUS_STARTUP_2;
			break;
            
        case USER_INTERFACE_STATUS_STARTUP_2:
            //Put display into reset
            i2c_digipot_reset_on();
            //Proceed to next state
			system_ui_inactive_count = 0;
			userInterfaceStatus = USER_INTERFACE_STATUS_STARTUP_3;
			break;

		case USER_INTERFACE_STATUS_STARTUP_3:
			++system_ui_inactive_count;
            //Turn off reset after 16ms
			if (system_ui_inactive_count>3)
			{
				i2c_digipot_reset_off();
				system_ui_inactive_count = 0;
				userInterfaceStatus = USER_INTERFACE_STATUS_STARTUP_4;
			}
			break;

		case USER_INTERFACE_STATUS_STARTUP_4:
            //Send init sequence
            i2c_display_send_init_sequence();
            //Turn backlight on
            i2c_digipot_backlight(150); 
            //Enable rotary encoder inputs
            system_encoder_enable();
            //User interface is now up and running
            system_ui_inactive_count = 0;
            userInterfaceStatus = USER_INTERFACE_STATUS_ON;
			break;

		case USER_INTERFACE_STATUS_ON:
			if (os.encoderCount==0 && os.buttonCount==0)
			{
				++system_ui_inactive_count;
				if(system_ui_inactive_count > OS_USER_INTERFACE_TIMEOUT)
				{
                    //Disable rotary encoder inputs
                    system_encoder_disable();
                    //Disable the user interface
                    DISP_EN_PORT = 1;
                    //i2c_expander_low(I2C_EXPANDER_USER_INTERFACE);
                    //User interface is now off
                    system_ui_inactive_count = 0;
					userInterfaceStatus = USER_INTERFACE_STATUS_OFF;
				}
			}
			else
			{
				system_ui_inactive_count = 0;
				_ui_encoder();
			}
			break;
	}
}