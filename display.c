
#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "i2c.h"
#include "os.h"
//#include "rtcc.h"
//#include "buck.h"


char display_content[4][20];

static void _display_inout(void);
static void _display_time(uint8_t mode);
static void _display_usb(void);
static void _display_outputs(uint8_t mode);
static void _display_charger(void);
static void _display_efficiency(void);
static void _display_temperature(void);

static void _display_clear(void)
{
    uint8_t row;
    uint8_t col;
    for(row=0;row<4;++row)
    {
        for(col=0;col<20;++col)
        {
            display_content[row][col] = ' ';
        }
    }
}

static void _display_itoa(int16_t value, uint8_t decimals, char *text)
{
    uint8_t pos;
    uint8_t len;
    int8_t missing;
    char tmp[10];
    itoa(tmp, value, 10);
    len = strlen(tmp);
    
    if(value<0) //negative values
    {
        missing = decimals + 2 - len;
        if(missing>0) //zero-padding needed
        {
            for(pos=decimals;pos!=0xFF;--pos)
            {
                if(pos>=missing) //there is a character to copy
                {
                    tmp[pos+1] = tmp[pos+1-missing];
                }
                else //there is no character
                {
                    tmp[pos+1] = '0';
                }
            }
            len = decimals + 2;
        }  
    }
    else
    {
        missing = decimals + 1 - len;
        if(missing>0) //zero-padding needed
        {
            for(pos=decimals;pos!=0xFF;--pos)
            {
                if(pos>=missing) //there is a character to copy
                {
                    tmp[pos] = tmp[pos-missing];
                }
                else //there is no character
                {
                    tmp[pos] = '0';
                }
            }
            len = decimals + 1;
        }       
    }
 
    decimals = len - decimals - 1;
    
    for(pos=0;pos<len;++pos)
    {
        text[pos] = tmp[pos];
        if(pos==decimals)
        {
            //Insert decimal point
            ++pos;
            text[pos] = '.';
            break;
        }
    }
    for(;pos<len;++pos)
    {
        text[pos+1] = tmp[pos];
    }
}

void display_prepare(uint8_t mode)
{
    _display_clear();
    
    switch(mode&0xF0)
    {
        case DISPLAY_MODE_OVERVIEW:
            _display_inout();
            break;
        case DISPLAY_MODE_DATETIME_OVERVIEW:
            _display_time(mode);
            break;
        case DISPLAY_MODE_USB_CHARGER:
            _display_usb();
            break;
        case DISPLAY_MODE_OUTPUTS:
            _display_outputs(mode);
            break;
        case DISPLAY_MODE_CHARGER_DETAILS:
            _display_charger();
            break;
        case DISPLAY_MODE_EFFICIENCY:
            _display_efficiency();
            break;
        case DISPLAY_MODE_TEMPERATURE:
            _display_temperature();
            break;
        default:
            _display_inout();
    }
}


static void _display_inout(void)
{
    uint8_t cntr;
    const char line1[] = "Voltage & Current";
    const char line2[] = "   Input   Output";
    const char line3[] = "V:";
    const char line4[] = "A: --      --";
    
    cntr = 0;
    while(line1[cntr])
        display_content[0][cntr] = line1[cntr++];
    
    cntr = 0;
    while(line2[cntr])
        display_content[1][cntr] = line2[cntr++]; 
    
    cntr = 0;
    while(line3[cntr])
        display_content[2][cntr] = line3[cntr++]; 
    //_display_itoa(os.input_voltage, 3, &display_content[2][3]);
    //_display_itoa(os.output_voltage, 3, &display_content[2][11]);
    
    cntr = 0;
    while(line4[cntr])
        display_content[3][cntr] = line4[cntr++];
    //if(i2c_expander_isHigh(I2C_EXPANDER_CHARGER_ENABLE))
    if(0)
    {
        _display_itoa(os.input_current, 3, &display_content[3][3]);
        _display_itoa(os.output_current, 3, &display_content[3][11]); 
    }
    
}

void display_update(void)
{
    i2c_display_cursor(0, 0);
    i2c_display_write_fixed(&display_content[0][0], 20);
    i2c_display_cursor(1, 0);
    i2c_display_write_fixed(&display_content[1][0], 20);
    i2c_display_cursor(2, 0);
    i2c_display_write_fixed(&display_content[2][0], 20);
    i2c_display_cursor(3, 0);
    i2c_display_write_fixed(&display_content[3][0], 20);
}

static void _display_time(uint8_t mode)
{
    const char tad[] = "Time and Date";
    const char setyear[] = "Set year";
    const char setmonth[] = "Set month";
    const char setday[] = "Set day";
    const char sethours[] = "Set hours";
    const char setminutes[] = "Set minutes";
    const char setseconds[] = "Set seconds";
    const char pts[] = "->Press to set time";
    const char pressmonth[] = "->Press for month";
    const char pressday[] = "->Press for day";
    const char presshours[] = "->Press for hours";
    const char pressminutes[] = "->Press for minutes";
    const char pressseconds[] = "->Press for seconds";
    const char presssave[] = "->Press to save";
    
    uint8_t cntr;
    uint8_t year = rtcc_get_year();
    uint8_t month = rtcc_get_month();
    uint8_t day = rtcc_get_day();
    uint8_t hours = rtcc_get_hours();
    uint8_t minutes = rtcc_get_minutes();
    uint8_t seconds = rtcc_get_seconds();
    
    switch(mode)
    {
        case DISPLAY_MODE_DATETIME_OVERVIEW:
            cntr = 0;
            while(tad[cntr])
                display_content[0][cntr] = tad[cntr++];
            cntr = 0;
            while(pts[cntr])
                display_content[3][cntr] = pts[cntr++];    
            break;
            
        case DISPLAY_MODE_DATETIME_YEAR:
            cntr = 0;
            while(setyear[cntr])
                display_content[0][cntr] = setyear[cntr++];
            cntr = 0;
            while(pressmonth[cntr])
                display_content[3][cntr] = pressmonth[cntr++];    
            break;
            
        case DISPLAY_MODE_DATETIME_MONTH:
            cntr = 0;
            while(setmonth[cntr])
                display_content[0][cntr] = setmonth[cntr++];
            cntr = 0;
            while(pressday[cntr])
                display_content[3][cntr] = pressday[cntr++];    
            break;
            
        case DISPLAY_MODE_DATETIME_DAY:
            cntr = 0;
            while(setday[cntr])
                display_content[0][cntr] = setday[cntr++];
            cntr = 0;
            while(presshours[cntr])
                display_content[3][cntr] = presshours[cntr++];    
            break;
            
        case DISPLAY_MODE_DATETIME_HOURS:
            cntr = 0;
            while(sethours[cntr])
                display_content[0][cntr] = sethours[cntr++];
            cntr = 0;
            while(pressminutes[cntr])
                display_content[3][cntr] = pressminutes[cntr++];    
            break;
            
        case DISPLAY_MODE_DATETIME_MINUTES:
            cntr = 0;
            while(setminutes[cntr])
                display_content[0][cntr] = setminutes[cntr++];
            cntr = 0;
            while(pressseconds[cntr])
                display_content[3][cntr] = pressseconds[cntr++];    
            break;
            
        case DISPLAY_MODE_DATETIME_SECONDS:
            cntr = 0;
            while(setseconds[cntr])
                display_content[0][cntr] = setseconds[cntr++];
            cntr = 0;
            while(presssave[cntr])
                display_content[3][cntr] = presssave[cntr++];    
            break;
    }

    //Year
    display_content[1][0] = '2';
    display_content[1][1] = '0';
    display_content[1][2] = (year>>4) + 48;
    display_content[1][3] = (year&0x0F) + 48;
    //Month
    display_content[1][4] = '-';
    display_content[1][5] = (month>>4) + 48;
    display_content[1][6] = (month&0x0F) + 48;
    //Day
    display_content[1][7] = '-';
    display_content[1][8] = (day>>4) + 48;
    display_content[1][9] = (day&0x0F) + 48;
    //Hours
    display_content[2][0] = (hours>>4) + 48;
    display_content[2][1] = (hours&0x0F) + 48;
    //Month
    display_content[2][2] = ':';
    display_content[2][3] = (minutes>>4) + 48;
    display_content[2][4] = (minutes&0x0F) + 48;
    //Day
    display_content[2][5] = ':';
    display_content[2][6] = (seconds>>4) + 48;
    display_content[2][7] = (seconds&0x0F) + 48;  
}

static void _display_usb(void)
{
    uint8_t cntr;
    const char header[] = "USB Charger Outputs";
    const char low[] = "Battery too low";
    const char usb_on[] = "USB charging on";
    const char usb_off[] = "USB charging off";
    
    
    cntr = 0;
    while(header[cntr])
        display_content[0][cntr] = header[cntr++];
    
    if(os.output_voltage<USB_CHARGING_VOLTAGE_MINIMUM)
    {    
        cntr = 0;
        while(low[cntr])
            display_content[2][cntr] = low[cntr++];
    }
    
    cntr = 0;
    //if(i2c_expander_isHigh(I2C_EXPANDER_USB_CHARGER))
    if(0)
    {
        while(usb_on[cntr])
            display_content[3][cntr] = usb_on[cntr++];
    }
    else
    {
        while(usb_off[cntr])
            display_content[3][cntr] = usb_off[cntr++];
    } 
}

static void _display_outputs(uint8_t mode)
{
    uint8_t cntr;
    const char header[] = "Power Outputs";
    const char low[] = "Battery too low";
    const char line3[] = "  Out1 off  Out2 off";
    const char line4[] = "  Out3 off  Out4 off";
    
    cntr = 0;
    while(header[cntr])
        display_content[0][cntr] = header[cntr++];
    
    if(os.output_voltage<POWER_OUTPUTS_VOLTAGE_MINIMUM)
    {    
        cntr = 0;
        while(low[cntr])
            display_content[1][cntr] = low[cntr++];
    }
    
    cntr = 0;
    while(line3[cntr])
        display_content[2][cntr] = line3[cntr++];
    
    cntr = 0;
    while(line4[cntr])
        display_content[3][cntr] = line4[cntr++];
    
    if(os.outputs & OUTPUT_1)
    {
        display_content[2][8] = 'n';
        display_content[2][9] = ' ';
    }
    if(os.outputs & OUTPUT_2)
    {
        display_content[2][18] = 'n';
        display_content[2][19] = ' ';
    }
    if(os.outputs & OUTPUT_3)
    {
        display_content[3][8] = 'n';
        display_content[3][9] = ' ';
    }
    if(os.outputs & OUTPUT_4)
    {
        display_content[3][18] = 'n';
        display_content[3][19] = ' ';
    }
    
    switch(mode)
    {
        case DISPLAY_MODE_OUTPUTS_1:
            display_content[2][0] = '-';
            display_content[2][1] = '>';
            break;
        case DISPLAY_MODE_OUTPUTS_2:
            display_content[2][10] = '-';
            display_content[2][11] = '>';
            break;
        case DISPLAY_MODE_OUTPUTS_3:
            display_content[3][0] = '-';
            display_content[3][1] = '>';
            break;
        case DISPLAY_MODE_OUTPUTS_4:
            display_content[3][10] = '-';
            display_content[3][11] = '>';
            break;
    }
}

static void _display_charger(void)
{
    uint8_t cntr;
    const char header[] = "MPPT Charger Details";
    const char supply_on[] = "Charger supply on";
    const char supply_off[] = "Charger supply off";
    const char fan_on[] = "Fan on";
    const char fan_off[] = "Fan off";
    
    const char charger_off[] = "Charger off";
    const char charger_startup[] = "Charger Startup";
    const char charger_async[] = "Charger Asynchronous";
    const char charger_sync[] = "Charger Synchronous";
                
    cntr = 0;
    while(header[cntr])
        display_content[0][cntr] = header[cntr++];
    
    cntr = 0;
    //if(i2c_expander_isHigh(I2C_EXPANDER_CHARGER_ENABLE))
    if(0)
    {
        while(supply_on[cntr])
            display_content[1][cntr] = supply_on[cntr++];
    }
    else
    {
        while(supply_off[cntr])
            display_content[1][cntr] = supply_off[cntr++];
    }
    
    cntr = 0;
    //switch(buck_get_mode())
    if(1)
    {
        //case BUCK_STATUS_OFF:
            while(charger_off[cntr])
                display_content[2][cntr] = charger_off[cntr++];
            //break;
            
//        case BUCK_STATUS_STARTUP:
//            while(charger_startup[cntr])
//                display_content[2][cntr] = charger_startup[cntr++];
//            break;
//            
//        case BUCK_STATUS_ASYNCHRONOUS:
//            while(charger_async[cntr])
//                display_content[2][cntr] = charger_async[cntr++];
//            break;
//            
//        case BUCK_STATUS_SYNCHRONOUS:
//            while(charger_sync[cntr])
//                display_content[2][cntr] = charger_sync[cntr++];
//            break;
    }
    
    _display_itoa((int16_t) buck_get_dutycycle(), 0, &display_content[3][0]);
    
//    cntr = 0;
//    if(i2c_expander_isHigh(I2C_EXPANDER_FAN))
//    {
//        while(fan_on[cntr])
//            display_content[3][cntr] = fan_on[cntr++];
//    }
//    else
//    {
//        while(fan_off[cntr])
//            display_content[3][cntr] = fan_off[cntr++];
//    }
}

static void _display_efficiency(void)
{
    uint8_t cntr;
    const char header[] = "Charger Efficiency";
    const char source[] = "Panel:    Battery:";
    const char footer[] = "E:         L:";
    int32_t panel;
    int32_t battery = (int32_t) os.output_voltage * (int32_t) os.output_current;
    float efficiency;
    
    panel = (int32_t) os.input_voltage;
    panel *= (int32_t) os.input_current;
    panel /= 1000;
    
    battery = (int32_t) os.output_voltage;
    battery *= os.output_current;
    efficiency = battery / panel;
    battery /= 1000;

    cntr = 0;
    while(header[cntr])
        display_content[0][cntr] = header[cntr++]; 
    
    cntr = 0;
    while(source[cntr])
        display_content[1][cntr] = source[cntr++];
    
    _display_itoa((int16_t) panel, 3, &display_content[2][0]);
    _display_itoa((int16_t) battery, 3, &display_content[2][10]);
    
    cntr = 0;
    while(footer[cntr])
        display_content[3][cntr] = footer[cntr++]; 
    
    _display_itoa((int16_t) efficiency, 1, &display_content[3][3]);
    _display_itoa((int16_t) (panel-battery), 3, &display_content[3][14]);
}

static void _display_temperature(void)
{
    uint8_t cntr;
    const char header[] = "Temperature Monitor";
    const char internal[] = "On-board: ";
    const char external1[] = "External 1:";
    const char external2[] = "External 2:";

    cntr = 0;
    while(header[cntr])
        display_content[0][cntr] = header[cntr++];
    
    cntr = 0;
    while(internal[cntr])
        display_content[1][cntr] = internal[cntr++];
    
    
    //_display_itoa(os.temperature_onboard_adc, 2, &display_content[1][12]);
    _display_itoa(os.temperature_onboard, 2, &display_content[1][12]);
    
    cntr = 0;
    while(external1[cntr])
        display_content[2][cntr] = external1[cntr++];
    
    _display_itoa(os.temperature_external_1, 2, &display_content[2][12]);
    
    cntr = 0;
    while(external2[cntr])
        display_content[3][cntr] = external2[cntr++];
    
    _display_itoa(os.temperature_external_2, 2, &display_content[3][12]);  
}