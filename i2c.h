/* 
 * File:   i2c.h
 * Author: Luke
 *
 * Created on 4. September 2016, 09:26
 */

#ifndef I2C_H
#define	I2C_H

#include <stdint.h>
#include "os.h"
   
//#define I2C_PORT_DISPLAY 0b00000001    
//#define I2C_PORT_ADC 0b00000010
//#define I2C_PORT_EXTENDER 0b00000100
//#define I2C_PORT_EEPROM 0b00001000

typedef enum
{
    I2C_FREQUENCY_100kHz,
    I2C_FREQUENCY_200kHz,
    I2C_FREQUENCY_400kHz
} i2cFrequency_t;

typedef enum
{
    I2C_MUX_DISPLAY,
    I2C_MUX_ADC,
    I2C_MUX_EXPANDER,
    I2C_MUX_EEPROM
} i2cMuxPort_t;

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

typedef enum
{
    I2C_EXPANDER_USER_INTERFACE,
    I2C_EXPANDER_USB_CHARGER,
    I2C_EXPANDER_FAN,
    I2C_EXPANDER_OUTPUTS_ENABLE,
    I2C_EXPANDER_CHARGER_ENABLE,
    I2C_EXPANDER_BOARD_VOLTAGE,
    I2C_EXPANDER_OUTPUT_3,
    I2C_EXPANDER_OUTPUT_4
} i2cExpanderPin_t;


void i2c_init(void);
i2cFrequency_t i2c_get_frequency(void);
void i2c_set_frequency(i2cFrequency_t frequency);

void i2c_display_init(void);
void i2c_display_send_init_sequence(void);
void i2c_display_cursor(uint8_t line, uint8_t position);
void i2c_display_write(char *data);
void i2c_display_write_fixed(char *data, uint8_t length);

void i2c_digipot_set_defaults(void);
void i2c_digipot_reset_on(void);
void i2c_digipot_reset_off(void);
void i2c_digipot_backlight(uint8_t level);

void i2c_eeprom_writeByte(uint16_t address, uint8_t data);
uint8_t i2c_eeprom_readByte(uint16_t address);
void i2c_eeprom_write(uint16_t address, uint8_t *data, uint8_t length);
void i2c_eeprom_read(uint16_t address, uint8_t *data, uint8_t length);

void i2c_eeprom_read_calibration(void);
//void i2c_eeprom_read_calibration(calibration_t *buffer, calibrationIndex_t index);


void i2c_adc_start(i2cAdcPort_t channel, i2cAdcResolution_t resolution, i2cAdcGain_t gain);
int16_t i2c_adc_read(void);

#endif	/* I2C_H */

