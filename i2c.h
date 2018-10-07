/* 
 * File:   i2c.h
 * Author: Luke
 *
 * Created on 4. September 2016, 09:26
 */

#ifndef I2C_H
#define	I2C_H

#include <stdint.h>
#include "application_config.h"

/* ****************************************************************************
 * Type definitions
 * ****************************************************************************/

typedef enum
{
    I2C_FREQUENCY_100kHz,
    I2C_FREQUENCY_200kHz,
    I2C_FREQUENCY_400kHz
} i2cFrequency_t;

#ifdef ANALOG_DIGITAL_CONVERTER_AVAILABLE 
typedef enum
{
    I2C_ADC_OUTPUT_VOLTAGE,
    I2C_ADC_OUTPUT_CURRENT,
    I2C_ADC_INPUT_CURRENT,
    I2C_ADC_INPUT_VOLTAGE
} i2cAdcPort_t;

typedef enum
{
    I2C_ADC_RESOLUTION_12BIT,
    I2C_ADC_RESOLUTION_14BIT,
    I2C_ADC_RESOLUTION_16BIT,
    I2C_ADC_RESOLUTION_18BIT
} i2cAdcResolution_t;

typedef enum
{
    I2C_ADC_GAIN_1,
    I2C_ADC_GAIN_2,
    I2C_ADC_GAIN_4,
    I2C_ADC_GAIN_8
} i2cAdcGain_t;
#endif /*ANALOG_DIGITAL_CONVERTER_AVAILABLE*/

#ifdef I2C_TASK_SCHEDULING_AVAILABLE
typedef enum
{
       EEPROM_WRITE_TASK_NONE = 0,
       EEPROM_WRITE_TASK_REAL_TIME_CLOCK,
       EEPROM_WRITE_TASK_CALIBRATION_INPUT_VOLTAGE,
       EEPROM_WRITE_TASK_CALIBRATION_OUTPUT_VOLTAGE,
       EEPROM_WRITE_TASK_CALIBRATION_INPUT_CURRENT,
       EEPROM_WRITE_TASK_CALIBRATION_OUTPUT_CURRENT,
       EEPROM_WRITE_TASK_CALIBRATION_ONBOARD_TEMPERATURE,
       EEPROM_WRITE_TASK_CALIBRATION_EXTERNAL_TEMPERATURE_1,
       EEPROM_WRITE_TASK_CALIBRATION_EXTERNAL_TEMPERATURE_2
} eeprom_write_task_t;
#endif /*I2C_TASK_SCHEDULING_AVAILABLE*/

/* ****************************************************************************
 * General I2C functionality
 * ****************************************************************************/

void i2c_init(void);
void i2c_reset(void);
i2cFrequency_t i2c_get_frequency(void);
void i2c_set_frequency(i2cFrequency_t frequency);


/* ****************************************************************************
 * I2C Task Scheduling
 * ****************************************************************************/

#ifdef I2C_TASK_SCHEDULING_AVAILABLE
uint8_t get_eeprom_write_task_count(void);
void schedule_eeprom_write_task(eeprom_write_task_t task);
eeprom_write_task_t get_next_eeprom_write_task(void);
void i2c_eeprom_tasks(void);
#endif /*I2C_TASK_SCHEDULING_AVAILABLE*/

/* ****************************************************************************
 * I2C Display Functionality
 * ****************************************************************************/

void i2c_display_send_init_sequence(void);
void i2c_display_cursor(uint8_t line, uint8_t position);
void i2c_display_write(char *data);
void i2c_display_write_fixed(char *data, uint8_t length);


/* ****************************************************************************
 * I2C Dual Digipot Functionality
 * ****************************************************************************/

void i2c_digipot_reset_on(void);
void i2c_digipot_reset_off(void);
void i2c_digipot_backlight(uint8_t level);
void i2c_digipot_set_defaults(void);


/* ****************************************************************************
 * I2C EEPROM Functionality
 * ****************************************************************************/

void i2c_eeprom_writeByte(uint16_t address, uint8_t data);
uint8_t i2c_eeprom_readByte(uint16_t address);
void i2c_eeprom_write(uint16_t address, uint8_t *data, uint8_t length);
void i2c_eeprom_read(uint16_t address, uint8_t *data, uint8_t length);


/* ****************************************************************************
 * Calibration for ADC, stored in EEPROM
 * ****************************************************************************/

#ifdef ANALOG_DIGITAL_CONVERTER_AVAILABLE 
void i2c_eeprom_read_calibration(void);
#endif /*ANALOG_DIGITAL_CONVERTER_AVAILABLE*/


/* ****************************************************************************
 * I2C ADC Functionality
 * ****************************************************************************/

#ifdef ANALOG_DIGITAL_CONVERTER_AVAILABLE 
void i2c_adc_start(i2cAdcPort_t channel, i2cAdcResolution_t resolution, i2cAdcGain_t gain);
int16_t i2c_adc_read(void);
#endif /*ANALOG_DIGITAL_CONVERTER_AVAILABLE*/


#endif	/* I2C_H */

