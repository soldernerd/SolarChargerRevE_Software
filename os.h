/* 
 * File:   system.h
 * Author: Luke
 *
 * Created on 5. September 2016, 21:17
 */

#ifndef OS_H
#define	OS_H

#include <stdint.h>
#include "i2c.h"
#include "spi.h"

/*
 * Function prototypes
 */

void reboot(void);

/*
 * Type definitions
 */

typedef enum 
{ 
    DISPLAY_MODE_OVERVIEW = 0x00,
    DISPLAY_MODE_DATETIME_OVERVIEW = 0x10,
    DISPLAY_MODE_DATETIME_YEAR = 0x11,
    DISPLAY_MODE_DATETIME_MONTH = 0x12,
    DISPLAY_MODE_DATETIME_DAY = 0x13,
    DISPLAY_MODE_DATETIME_HOURS = 0x14,
    DISPLAY_MODE_DATETIME_MINUTES = 0x15,
    DISPLAY_MODE_DATETIME_SECONDS = 0x16,
    DISPLAY_MODE_USB_CHARGER = 0x20,
    DISPLAY_MODE_OUTPUTS = 0x30,
    DISPLAY_MODE_OUTPUTS_1 = 0x31,
    DISPLAY_MODE_OUTPUTS_2 = 0x32,
    DISPLAY_MODE_OUTPUTS_3 = 0x33,
    DISPLAY_MODE_OUTPUTS_4 = 0x34,
    DISPLAY_MODE_CHARGER_DETAILS = 0x40,
    DISPLAY_MODE_EFFICIENCY = 0x50,
    DISPLAY_MODE_TEMPERATURE = 0x60,
    DISPLAY_MODE_STARTUP = 0x70
} displayMode_t;

typedef enum 
{ 
    CPU_FREQUENCY_32kHz,
    CPU_FREQUENCY_8MHz,
    CPU_FREQUENCY_48MHz
} clockFrequency_t;

typedef enum 
{ 
    BUCK_OFF,
    BUCK_FREQUENCY_62500,
    BUCK_FREQUENCY_93750,
    BUCK_FREQUENCY_187500
} buckFrequency_t;

typedef enum 
{ 
    OUTPUT_1 = 0x01,
    OUTPUT_2 = 0x02,
    OUTPUT_3 = 0x04,
    OUTPUT_4 = 0x08,
    OUTPUT_ALL = 0x0F,
    OUTPUT_USB = 0x10,
    OUTPUT_FAN = 0x20,        
} outputs_t;

typedef enum
{
    BOARD_VOLTAGE_LOW,
    BOARD_VOLTAGE_HIGH 
} boardVoltage_t;

typedef enum
{
    CALIBRATION_INDEX_INPUT_VOLTAGE = 0,
    CALIBRATION_INDEX_OUTPUT_VOLTAGE = 1,
    CALIBRATION_INDEX_INPUT_CURRENT = 2,
    CALIBRATION_INDEX_OUTPUT_CURRENT = 3,
    CALIBRATION_INDEX_ONBOARD_TEMPERATURE = 4,
    CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_1 = 5,
    CALIBRATION_INDEX_EXTERNAL_TEMPERATURE_2 = 6,
    CALIBRATION_INDEX_COUNT = 7
} calibrationIndex_t;

typedef struct
{
    int16_t NeutralOffset;
    int16_t NeutralMultiplier;
    int8_t NeutralShift;
    int16_t Offset;
    int16_t Multiplier;
    int8_t Shift;
    int16_t AutoCalibration;
} calibration_t;

typedef enum
{
    EXTERNAL_MODE_SLAVE,
    EXTERNAL_MODE_MASTER
} externalMode_t;

typedef struct
{
    //SPI settings
    externalMode_t spiMode;
    spiFrequency_t spiFrequency;
    spiPolarity_t spiPolarity;
    //I2C settings
    externalMode_t i2cMode;
    i2cFrequency_t i2cFrequency;
    uint8_t i2cSlaveModeSlaveAddress;
    uint8_t i2cMasterModeSlaveAddress;
} communicationSettings_t;


typedef struct
{
    clockFrequency_t clockFrequency;
    boardVoltage_t boardVoltage;
    buckFrequency_t buckFrequency;
    uint8_t buckDutyCycle;
    int8_t buckLastStep;
    uint8_t outputs;
    volatile int8_t encoderCount;
    volatile int8_t buttonCount;
    volatile uint8_t timeSlot;
    volatile uint8_t done;
    int16_t input_voltage_adc[4];
    //int8_t input_voltage_calibration;
    int16_t input_voltage;
    //int16_t input_voltage_last;
    int16_t input_current_adc[4];
    int8_t input_current_calibration;
    int16_t input_current;
    //int16_t input_current_last;
    int16_t output_voltage_adc[4];
    //int8_t output_voltage_calibration;
    int16_t output_voltage;
    int16_t output_current_adc[4];
    int8_t output_current_calibration;
    int16_t output_current;
    //Temperature measurement
    int16_t temperature_onboard_adc;
    int16_t temperature_onboard;
    int16_t temperature_external_1_adc;
    int16_t temperature_external_1;
    int16_t temperature_external_2_adc;
    int16_t temperature_external_2;
    //Display
    uint8_t display_mode;
    //External communication configuration
    communicationSettings_t communicationSettings;
} os_t;


/*
 * Global variables
 */

os_t os;

/*
 * Function prototypes
 */


void tmr_isr(void);
void system_init(void);
//void system_set_cpu_frequency(clockFrequency_t newFrequency);
void system_delay_ms(uint8_t ms);
//void system_power_save(void);
void system_encoder_enable(void);
void system_encoder_disable(void);



uint8_t system_output_is_on(outputs_t output);
void system_output_on(outputs_t output);
void system_output_off(outputs_t output);
void system_output_toggle(outputs_t output);

/*
void system_buck_set_frequency(buckFrequency_t buckFrequency);
void system_buck_set_dutycycle(uint8_t dutyCycle);
void system_buck_adjust_dutycycle(void);
void system_buck_start(void);
void system_buck_stop(void);
 *  * */

void system_calculate_input_voltage();
void system_calculate_output_voltage();
void system_calculate_input_current();
void system_calculate_output_current();


#endif	/* OS_H */

