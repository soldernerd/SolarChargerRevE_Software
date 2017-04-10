
#include <xc.h>
#include <stdint.h>
#include "os.h"
#include "adc.h"

extern calibration_t calibrationParameters[7];

void adc_init(void)
{
    ADCON0bits.VCFG1 = 0; // negative reference = GND
    ADCON0bits.VCFG0 = 1; // positive reference = AN3
    ADCON0bits.CHS = TEMPERATURE1_CHANNEL; // On-board sensor on AN12
    ADCON0bits.ADON = 1; // Enable ADC module
    ADCON1bits.ADFM = 0; // left justified
    ADCON1bits.ADCAL = 0; // no calibration
    ADCON1bits.ADCS = 0b110; // T_ad = Fosc/64
}

void adc_calibrate(void)
{
    ADCON1bits.ADCAL = 1; // Calibrate
    //Start conversion
    ADCON0bits.GO_NOT_DONE = 1;
    //Wait for calibration to complete
    while(ADCON0bits.GO_NOT_DONE);
    ADCON1bits.ADCAL = 0; // Calibrate off
}

uint16_t adc_read(adcChannel_t channel)
{
    uint16_t adc_value;
    
    switch(channel)
    {
        case ADC_CHANNEL_TEMPERATURE_ONBOARD:
            ADCON0bits.CHS = TEMPERATURE1_CHANNEL;
            break;
        case ADC_CHANNEL_TEMPERATURE_EXTERNAL_1:
            ADCON0bits.CHS = TEMPERATURE2_CHANNEL;
            break;
        case ADC_CHANNEL_TEMPERATURE_EXTERNAL_2:
            ADCON0bits.CHS = TEMPERATURE3_CHANNEL;
            break;
        default:
            return 0xFFFF;
    }
    
    //Wait for acquisition time
    switch(os.clockFrequency)
    {
        case CPU_FREQUENCY_8MHz:
            __delay_us(15);
            break;
        case CPU_FREQUENCY_48MHz:
            __delay_us(90);
            break;
    }
    
    //Start conversion
    ADCON0bits.GO_NOT_DONE = 1;
    
    //Wait for result to be ready
    while(ADCON0bits.GO_NOT_DONE);
    
    //Get result
    adc_value = ADRESH;
    adc_value <<= 2;
    adc_value |= ADRESL;
    
    return adc_value;
}

int16_t adc_calculate_temperature(uint16_t adc_value, calibrationIndex_t calibration)
{
    int32_t tmp = (int32_t) adc_value;
    tmp += calibrationParameters[calibration].Offset; 
    tmp *= calibrationParameters[calibration].Multiplier;
    tmp >>= calibrationParameters[calibration].Shift - 1;
    tmp += 1;
    tmp >>= 1;
    return (int16_t) tmp;
}