
#include "rtcc.h"
#include "i2c.h"
#include "os.h"
#include <xc.h>


static void _rtcc_write_enable(void)
{
    while(!RTCCFGbits.RTCWREN)
    {
        EECON2 = 0x55; //Make RTCWREN bit writable
        EECON2 = 0xAA;
        RTCCFGbits.RTCWREN = 1; //Enable writes 
    }
}

static void _rtcc_write_disable(void)
{
    while(RTCCFGbits.RTCWREN)
    {
        EECON2 = 0x55; //Make RTCWREN bit writable
        EECON2 = 0xAA;
        RTCCFGbits.RTCWREN = 0; //Enable writes 
    }
}

static uint8_t _rtcc_increment_bcd(uint8_t value, uint8_t min, uint8_t max)
{
    if(value==max)
        return min;
    ++value;
    if((value&0x0F)>0x09)
    {
        value += 0x10;
        value &= 0xF0;
    }
    return value;        
}

static uint8_t _rtcc_decrement_bcd(uint8_t value, uint8_t min, uint8_t max)
{
    if(value==min)
        return max;
    --value;
    if((value&0x0F)>0x09)
    {
        value &= 0xF0;
        value |= 0x09;
    }
    return value;        
}

static uint8_t _number_of_days(void)
{
    uint8_t month = rtcc_get_month();
    uint8_t bcd_year = rtcc_get_year();
    uint8_t year;
    if(month==0x04 || month==0x06 || month==0x09 || month==0x11)
        return 0x30;
    if(month==0x02)
    {
       year = bcd_year>>4;
       year *= 10;
       year += bcd_year & 0x0F;
       if(year&0b11) // no leap year
           return 0x28;
       else //leap year
           return 0x29;
    }
    return 0x31;
}


void rtcc_init(void)
{
    EECON2 = 0x55; //Make RTCWREN bit writable
    EECON2 = 0xAA;
    RTCCFGbits.RTCWREN = 1; //Enable writes
    RTCCFGbits.RTCEN = 1;
    RTCCFGbits.RTCOE = 0;
    RTCCAL = 0x00; //No calibration
    
    //Restore date and time from EEPROM
    rtcc_read_eeprom();
}

void rtcc_correct_day(void)
{
    uint8_t day = rtcc_get_day();
    uint8_t max = _number_of_days();
    if(day>max)
        rtcc_set_day(max);
}


uint8_t rtcc_get_year(void)
{
    RTCCFGbits.RTCPTR1 = 1;
    RTCCFGbits.RTCPTR0 = 1;
    while(RTCCFGbits.RTCSYNC);
    return RTCVALL;
}

void rtcc_set_year(uint8_t year)
{
    _rtcc_write_enable();
    RTCCFGbits.RTCPTR1 = 1;
    RTCCFGbits.RTCPTR0 = 1;
    RTCVALL = year;
    _rtcc_write_disable();
}
    
void rtcc_increment_year(void)
{
    uint8_t year = rtcc_get_year();
    year = _rtcc_increment_bcd(year, 0x00, 0x99);
    rtcc_set_year(year);
}

void rtcc_decrement_year(void)
{
    uint8_t year = rtcc_get_year();
    year = _rtcc_decrement_bcd(year, 0x00, 0x99);
    rtcc_set_year(year);
}


uint8_t rtcc_get_month(void)
{
    RTCCFGbits.RTCPTR1 = 1;
    RTCCFGbits.RTCPTR0 = 0;
    while(RTCCFGbits.RTCSYNC);
    return RTCVALH;
}

void rtcc_set_month(uint8_t month)
{
    _rtcc_write_enable();
    RTCCFGbits.RTCPTR1 = 1;
    RTCCFGbits.RTCPTR0 = 0;
    RTCVALH = month;
    _rtcc_write_disable();
}
    
void rtcc_increment_month(void)
{
    uint8_t month = rtcc_get_month();
    month = _rtcc_increment_bcd(month, 0x01, 0x12);
    rtcc_set_month(month);
}

void rtcc_decrement_month(void)
{
    uint8_t month = rtcc_get_month();
    month = _rtcc_decrement_bcd(month, 0x01, 0x12);
    rtcc_set_month(month);
}


uint8_t rtcc_get_day(void)
{
    RTCCFGbits.RTCPTR1 = 1;
    RTCCFGbits.RTCPTR0 = 0;
    while(RTCCFGbits.RTCSYNC);
    return RTCVALL;
}

void rtcc_set_day(uint8_t day)
{
    _rtcc_write_enable();
    RTCCFGbits.RTCPTR1 = 1;
    RTCCFGbits.RTCPTR0 = 0;
    RTCVALL = day;
    _rtcc_write_disable();
}
    
void rtcc_increment_day(void)
{
    uint8_t day = rtcc_get_day();
    day = _rtcc_increment_bcd(day, 0x01, _number_of_days());
    rtcc_set_day(day);
}

void rtcc_decrement_day(void)
{
    uint8_t day = rtcc_get_day();
    day = _rtcc_decrement_bcd(day, 0x01, _number_of_days());
    rtcc_set_day(day);
}


uint8_t rtcc_get_hours(void)
{
    RTCCFGbits.RTCPTR1 = 0;
    RTCCFGbits.RTCPTR0 = 1;
    while(RTCCFGbits.RTCSYNC);
    return RTCVALL;
}

void rtcc_set_hours(uint8_t hours)
{
    _rtcc_write_enable();
    RTCCFGbits.RTCPTR1 = 0;
    RTCCFGbits.RTCPTR0 = 1;
    RTCVALL = hours;
    _rtcc_write_disable();
}
    
void rtcc_increment_hours(void)
{
    uint8_t hours = rtcc_get_hours();
    hours = _rtcc_increment_bcd(hours, 0x00, 0x23);
    rtcc_set_hours(hours);
}

void rtcc_decrement_hours(void)
{
    uint8_t hours = rtcc_get_hours();
    hours = _rtcc_decrement_bcd(hours, 0x00, 0x23);
    rtcc_set_hours(hours);
}


uint8_t rtcc_get_minutes(void)
{
    RTCCFGbits.RTCPTR1 = 0;
    RTCCFGbits.RTCPTR0 = 0;
    while(RTCCFGbits.RTCSYNC);
    return RTCVALH;
}

void rtcc_set_minutes(uint8_t minutes)
{
    _rtcc_write_enable();
    RTCCFGbits.RTCPTR1 = 0;
    RTCCFGbits.RTCPTR0 = 0;
    RTCVALH = minutes;
    _rtcc_write_disable();
}
    
void rtcc_increment_minutes(void)
{
    uint8_t minutes = rtcc_get_minutes();
    minutes = _rtcc_increment_bcd(minutes, 0x00, 0x59);
    rtcc_set_minutes(minutes);
}

void rtcc_decrement_minutes(void)
{
    uint8_t minutes = rtcc_get_minutes();
    minutes = _rtcc_decrement_bcd(minutes, 0x00, 0x59);
    rtcc_set_minutes(minutes);
}


uint8_t rtcc_get_seconds(void)
{
    RTCCFGbits.RTCPTR1 = 0;
    RTCCFGbits.RTCPTR0 = 0;
    while(RTCCFGbits.RTCSYNC);
    return RTCVALL;
}

void rtcc_set_seconds(uint8_t seconds)
{
    _rtcc_write_enable();
    RTCCFGbits.RTCPTR1 = 0;
    RTCCFGbits.RTCPTR0 = 0;
    RTCVALL = seconds;
    _rtcc_write_disable();
}
    
void rtcc_increment_seconds(void)
{
    uint8_t seconds = rtcc_get_seconds();
    seconds = _rtcc_increment_bcd(seconds, 0x00, 0x59);
    rtcc_set_seconds(seconds);
}

void rtcc_decrement_seconds(void)
{
    uint8_t seconds = rtcc_get_seconds();
    seconds = _rtcc_decrement_bcd(seconds, 0x00, 0x59);
    rtcc_set_seconds(seconds);
}

void rtcc_read_eeprom(void)
{
    uint8_t date[6];
    i2c_eeprom_read(EEPROM_RTCC_ADDRESS, &date[0], 6);
    
    //Set time and date
    rtcc_set_year(date[0]);
    rtcc_set_month(date[1]);
    rtcc_set_day(date[2]);
    rtcc_set_hours(date[3]);
    rtcc_set_minutes(date[4]);
    rtcc_set_seconds(date[5]);
}

void rtcc_write_eeprom(void)
{
    uint8_t date[6];
    date[0] = rtcc_get_year();
    date[1] = rtcc_get_month();
    date[2] = rtcc_get_day();
    date[3] = rtcc_get_hours();
    date[4] = rtcc_get_minutes();
    date[5] = rtcc_get_seconds();
    
    i2c_eeprom_write(EEPROM_RTCC_ADDRESS, &date[0], 6);
}