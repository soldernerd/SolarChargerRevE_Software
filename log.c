/*
 * File:   log.c
 * Author: luke
 *
 * Created on 27. November 2017, 22:00
 */



#include <xc.h>
#include <stdint.h>

#include "log.h"
#include "rtcc.h"
#include "buck.h"

char filename[9] = LOG_FILENAME;
char extension[4] = LOG_EXTENSION;

//Local variables to keep sums while accumulating data
uint32_t inputVoltageSum;
uint32_t inputCurrentSum;
uint32_t outputVoltageSum;
uint32_t outputCurrentSum;
uint32_t inputPowerSum;
uint32_t outputPowerSum;
uint32_t inputCapacitySum;
uint32_t outputCapacitySum;
uint16_t temperatureOnboardSum;
uint16_t temperatureExternal1Sum;
uint16_t temperatureExternal2Sum;
uint8_t chargerOnTime;
uint8_t lowPowerTime;
uint8_t status;
uint16_t averageCount;

uint8_t lastSecond;

static uint32_t _get_dateTime(void)
{
    uint32_t datetime;
    uint8_t year = rtcc_get_year_decimal();
    uint8_t month = rtcc_get_month_decimal();
    uint8_t day = rtcc_get_day_decimal();
    uint8_t hours = rtcc_get_hours_decimal();
    uint8_t minutes = rtcc_get_minutes_decimal();
    uint8_t seconds = rtcc_get_seconds_decimal();
    datetime = ((year&0b111111) << 26);
    datetime |= ((month&0b1111) << 22);
    datetime |= (day&0b11111) << 17;
    datetime |= ((hours&0b11111) << 12);
    datetime |= ((minutes&0b111111) << 6);
    datetime |= (seconds&0b11111);
    return datetime;
};

static uint8_t _get_checksum(logEntry_t *log_entry)
{
    uint8_t checksum;
    uint8_t *byte_array;
    uint8_t cntr;
    byte_array = (uint8_t*) log_entry;
    checksum = 0x00;
    for(cntr=0; cntr<31; ++cntr)
    {
        checksum ^= byte_array[cntr];
    }
    return checksum;
}

void log_start_new(void)
{
    inputVoltageSum = 0;
    inputCurrentSum = 0;
    outputVoltageSum = 0;
    outputCurrentSum = 0;
    inputPowerSum = 0;
    outputPowerSum = 0;
    inputCapacitySum = 0;
    outputCapacitySum = 0;
    temperatureOnboardSum = 0;
    temperatureExternal1Sum = 0;
    temperatureExternal2Sum = 0;
    chargerOnTime = 0;
    lowPowerTime = 0;
    status = 0;
    averageCount = 0;
    lastSecond = rtcc_get_seconds_decimal();
}

uint16_t _get_input_power(void)
{
    uint32_t pwr;
    pwr = os.input_voltage * os.input_current;
    pwr /= 1000;
    return (uint16_t) pwr;
}

uint16_t _get_output_power(void)
{
    uint32_t pwr;
    pwr = os.output_voltage * os.output_current;
    pwr /= 1000;
    return (uint16_t) pwr;
}

uint8_t _get_temperature(int16_t temperature)
{
    return (uint8_t) (temperature+4000) / 100;
}

void log_collect_data(void)
{
    uint16_t temp;
    
    //Quit if we've already logged data this second
    temp = rtcc_get_seconds_decimal();
    if(lastSecond==temp)
    {
        return;
    }
    
    //Store current second
    lastSecond = (uint8_t) temp;
    
    //Need to collect data
    inputVoltageSum += os.input_voltage;
    inputCurrentSum += os.input_current;
    outputVoltageSum += os.output_voltage;
    outputCurrentSum += os.output_current;
    temp = _get_input_power();
    inputPowerSum += temp>>1;
    inputCapacitySum += temp;
    temp = _get_output_power();
    outputPowerSum += temp>>1;
    outputCapacitySum += temp;
    temperatureOnboardSum += _get_temperature(os.temperature_onboard);
    temperatureExternal1Sum += _get_temperature(os.temperature_external_1);
    temperatureExternal2Sum += _get_temperature(os.temperature_external_2);
    if(buck_get_mode()&0b01111111)
    {
        ++chargerOnTime;
    }
    if(os.boardVoltage==BOARD_VOLTAGE_LOW)
    {
        ++lowPowerTime;
    }
    if(system_output_is_on(OUTPUT_1))
        status |= 0b10000000;
    if(system_output_is_on(OUTPUT_2))
        status |= 0b01000000;
    if(system_output_is_on(OUTPUT_3))
        status |= 0b00100000;
    if(system_output_is_on(OUTPUT_4))
        status |= 0b00010000;
    if(system_output_is_on(OUTPUT_USB))
        status |= 0b00001000;
    if(system_output_is_on(OUTPUT_FAN))
        status |= 0b00000100;
    //if(USBDeviceState == ATTACHED_STATE) Need to get access to that first
        status |= 0b00000010;
    ++averageCount; 
    
    
}

uint16_t log_get_number_of_samples(void)
{
    return averageCount;
}

void log_generate_entry(logEntry_t *log_entry)
{
    log_entry->dateTime = _get_dateTime();
    log_entry->inputVoltage = inputVoltageSum / averageCount;
    log_entry->inputCurrent = inputCurrentSum / averageCount;
    log_entry->outputVoltage = outputVoltageSum / averageCount;
    log_entry->outputCurrent = outputCurrentSum / averageCount;
    log_entry->inputPower = inputPowerSum / averageCount;
    log_entry->outputPower = outputPowerSum / averageCount;
    log_entry->inputCapacity = inputCapacitySum;
    log_entry->outputCapacity = outputCapacitySum;
    log_entry->temperatureOnboard = temperatureOnboardSum / averageCount;
    log_entry->temperatureExternal1 = temperatureExternal1Sum / averageCount;
    log_entry->temperatureExternal2 = temperatureExternal2Sum / averageCount;
    log_entry->chargerOnTime = chargerOnTime;
    log_entry->lowPowerTime = lowPowerTime;
    log_entry->unused[0] = 0x00;
    log_entry->unused[1] = 0x00;
    log_entry->unused[2] = 0x00;
    log_entry->unused[3] = 0x00;
    log_entry->unused[4] = 0x00;
    log_entry->status = status;
    log_entry->checksum = _get_checksum(log_entry);
}

void log_write_to_disk(void)
{
    uint8_t file_number;
    logEntry_t log_entry;
    
    //Prepare log entry
    log_generate_entry(&log_entry);
    
    //Find or create file
    file_number = fat_find_file(filename, extension);
    if(file_number==0xFF) 
    {
        //File does not exist, create it
        file_number = fat_create_file(filename, extension, 0);
    }
    
    //Append entry to file
    fat_append_to_file(file_number, sizeof(logEntry_t), &log_entry);
}
