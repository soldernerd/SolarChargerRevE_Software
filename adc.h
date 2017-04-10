/* 
 * File:   adc.h
 * Author: Luke
 *
 * Created on 9. September 2016, 22:57
 */

#ifndef ADC_H
#define	ADC_H

#include <stdint.h>
#include "system.h"
#include "os.h"

typedef enum
{
    ADC_CHANNEL_TEMPERATURE_ONBOARD,
    ADC_CHANNEL_TEMPERATURE_EXTERNAL_1,
    ADC_CHANNEL_TEMPERATURE_EXTERNAL_2
} adcChannel_t;

void adc_init(void);
void adc_calibrate(void);
uint16_t adc_read(adcChannel_t channel);
int16_t adc_calculate_temperature(uint16_t adc_value, calibrationIndex_t calibration);

#endif	/* ADC_H */

