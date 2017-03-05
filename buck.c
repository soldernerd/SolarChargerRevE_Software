#include <xc.h>
#include <stdint.h>
#include <plib/pps.h>

#include "buck.h"
#include "os.h"
#include "i2c.h"

buckStatus_t buck_status;
uint8_t buck_dutycycle;
uint8_t buck_dutycycle_last_step;

uint8_t idx;
int32_t last;
int32_t now;
uint16_t battery_voltage_maximum = 13500;

static uint8_t _buck_initial_dutycycle()
{
    uint32_t dc;
    dc = (uint32_t) os.output_voltage;
    dc <<= 8;
    dc /= os.input_voltage;
    return (uint8_t) dc;
}

static void _buck_set_dutycycle(uint8_t dutyCycle)
{ 
    //Check limits
    if(dutyCycle<BUCK_DUTYCYCLE_MINIMUM)
        dutyCycle = BUCK_DUTYCYCLE_MINIMUM;
    if(dutyCycle>BUCK_DUTYCYCLE_MAXIMUM)
        dutyCycle = BUCK_DUTYCYCLE_MAXIMUM;
    
    //Save dutyCycle
    buck_dutycycle = dutyCycle;

    //Lowest two bits of duty cycle
    CCP1CONbits.DC1B = (dutyCycle&0b11);
    //High byte of duty cycle
    CCPR1L = dutyCycle >> 2;
}

static void _buck_set_sync_async(buckMode_t mode)
{
    if(mode==BUCK_MODE_ASYNCHRONOUS)
    {
        //Single output mode
        CCP1CONbits.P1M1 = 0;
        CCP1CONbits.P1M0 = 0;
    }
    if(mode==BUCK_MODE_SYNCHRONOUS)
    {
        //Half-bridge mode
        CCP1CONbits.P1M1 = 1;
        CCP1CONbits.P1M0 = 0;
    }
}

static void _buck_start(uint8_t dutycycle)
{
    //Period register
    PR2 = 63; //187.5kHz
    
    //Set dutycycle
    _buck_set_dutycycle(dutycycle);
    
    //Clear timer 2
    TMR2 = 0x00;
    //Enable timer 2
    T2CONbits.TMR2ON = 1;

    //Both outputs active high
    CCP1CON |= 0b00001100;
    
    //Half bridge mode
    CCP1CON |= 0b10000000;  
}

static void _buck_stop(void)
{
    //_buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS);
    CCP1CONbits.CCP1M3 = 0;
    CCP1CONbits.CCP1M2 = 0;
    CCP1CONbits.CCP1M1 = 0;
    CCP1CONbits.CCP1M0 = 0;
    
    //Disable timer 2
    T2CONbits.TMR2ON = 0;
    
    //Disconnect power & panel
    BUCK_ENABLE_PIN = 0;
}

static void _buck_timer2_init(void)
{
    //Use timer2 for both ECCP modules
    TCLKCONbits.T3CCP2 = 0;
    TCLKCONbits.T3CCP1 = 0;
    
    //Post scaler = 16
    T2CONbits.T2OUTPS3 = 1;
    T2CONbits.T2OUTPS2 = 1;
    T2CONbits.T2OUTPS1 = 1;
    T2CONbits.T2OUTPS0 = 1;
    
    //Prescaler = 1
    T2CONbits.T2CKPS1 = 0;
    T2CONbits.T2CKPS0 = 0;
    
    //Disable timer 2
    T2CONbits.TMR2ON = 0;
}

static void _buck_pin_init(void)
{
    //Configure pins as outputs
    BUCK_ENABLE_PIN = 0;
    BUCK_ENABLE_TRIS = PIN_OUTPUT;
    BUCK_LOWFET_PIN = 0;
    BUCK_LOWFET_TRIS = PIN_OUTPUT;
    BUCK_HIGHFET_PIN = 0;
    BUCK_HIGHFET_TRIS = PIN_OUTPUT;
    
    //Both pins low
    LATBbits.LATB2 = 0;
    LATBbits.LATB1 = 0;
    
    //Assign via peripheral pin select (PPS)
    PPSUnLock();
    PPSOutput(PPS_RP6, PPS_CCP1P1A);
    PPSOutput(PPS_RP5, PPS_P1B);
    PPSLock();
}



void buck_init(void)
{
    buck_status = BUCK_STATUS_OFF;   
    _buck_timer2_init();
    _buck_pin_init();
}


void buck_operate(void)
{
    uint8_t cntr;
    switch(buck_status)
    {
        case BUCK_STATUS_OFF:
            if((os.timeSlot&0b00110000)==0b00110000)
            {
               if((os.input_voltage-500)>os.output_voltage)
                {
                    BUCK_ENABLE_PIN = 1;
                    buck_status = BUCK_STATUS_STARTUP;
                    //Zero old measurements
                    os.input_current = 0;
                    os.output_current = 0;
                    for(cntr=0;cntr<4;++cntr)
                    {
                        os.input_current_adc[cntr] = 0;
                        os.output_current_adc[cntr] = 0;
                    }
                }  
            } 
            break;
            
        case BUCK_STATUS_STARTUP:
            if((os.timeSlot&0b00110000)==0b00110000)
            {
                //Calibrate current measurements
                os.input_current_calibration = 0;
                os.output_current_calibration = 0;
                for(cntr=0;cntr<4;++cntr)
                {
                    os.input_current_calibration += os.input_current_adc[cntr];
                    os.output_current_calibration += os.output_current_adc[cntr];
                }
                //Initialize variables
                buck_dutycycle_last_step = 1;
                //Charge bootstrap capacitor
                PIR1bits.TMR2IF = 0;
                _buck_start(_buck_initial_dutycycle());
                while(!PIR1bits.TMR2IF);
                //Switch to asynchronous mode
                _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS);
                _buck_set_dutycycle(BUCK_DUTYCYCLE_MINIMUM);
                buck_status = BUCK_STATUS_ASYNCHRONOUS;
            } 
            break;
            
        case BUCK_STATUS_ASYNCHRONOUS:
            if(os.timeSlot&0b00010000)
            {
                //Ensure that maximum battery voltage is not exceeded
                if(os.output_voltage>battery_voltage_maximum)
                {
                    _buck_set_dutycycle(buck_dutycycle-1);
                    buck_dutycycle_last_step = -1;
                }
                //Shut down if current has fallen to (or almost to) zero or panel voltage falls below battery voltage
//                else if ((os.input_current<BUCK_INPUT_CURRENT_MINIMUM) || (os.input_voltage<os.output_voltage))
//                {
//                    _buck_stop();
//                    buck_status = BUCK_STATUS_OFF;
//                }
                //Ensure a minimum voltage difference (otherwise the bootstrap capacitor will lose its charge)
                else if (os.input_voltage - os.output_voltage < BUCK_VOLTAGE_DIFFERENCE_MINIMUM)
                {
                    _buck_set_dutycycle(buck_dutycycle-1);
                    buck_dutycycle_last_step = -1;
                }
                //Switch to synchronous mode if current exceeds threshold
//                else if (os.input_current>BUCK_ASYNCHRONOUS_INPUT_CURRENT_MAXIMUM)
//                {
//                    _buck_set_sync_async(BUCK_MODE_SYNCHRONOUS);
//                    _buck_set_dutycycle(_buck_initial_dutycycle()+5);
//                    buck_status = BUCK_STATUS_SYNCHRONOUS;
//                }
                //Adjust duty cycle in order to track maximum point of power
                else
                {
                    //Current power level
                    /*
                    idx = os.timeSlot & 0b00110000;
                    idx >>= 4;
                    now = (int32_t)os.input_voltage_adc[idx];
                    now *= os.input_current_adc[idx];
                    //Previous power level
                    idx += 2;
                    idx &= 0b11;
                    last = (int32_t)os.input_voltage_adc[idx];
                    last *= os.input_current_adc[idx];
                    */
                    /*
                    idx = os.timeSlot & 0b00110000;
                    now = os.output_voltage_adc[idx];
                    idx += 2;
                    idx &= 0b11;
                    last = os.output_voltage_adc[idx];
                    */
                    idx = os.timeSlot & 0b00110000;
                    idx >>= 4;
                    now = (int32_t)os.input_voltage_adc[idx];
                    now *= (int32_t) os.input_current_adc[idx];
                    //Previous power level
                    idx += 2;
                    idx &= 0b11;
                    last = (int32_t)os.input_voltage_adc[idx];
                    last *= (int32_t) os.input_current_adc[idx];
                    //Adjust duty cycle
                    if(os.input_voltage>17000)
                    {
                        _buck_set_dutycycle(buck_dutycycle+1);
                    }
                    else
                    {
                        _buck_set_dutycycle(buck_dutycycle-1);
                    }    
                        
                    /*
                    if(now>=last)
                    {
                        if(buck_dutycycle_last_step>0)
                        {
                            _buck_set_dutycycle(buck_dutycycle+1);
                            buck_dutycycle_last_step = +1;
                        }
                        else
                        {
                            _buck_set_dutycycle(buck_dutycycle-1);
                            buck_dutycycle_last_step = -1;
                        }
                    }
                    else
                    {
                        if (buck_dutycycle_last_step>0)
                        {
                            _buck_set_dutycycle(buck_dutycycle-1);
                            buck_dutycycle_last_step = -1;
                        }
                        else
                        {
                            _buck_set_dutycycle(buck_dutycycle+1);
                            buck_dutycycle_last_step = +1;
                        }
                    }
                    */
                }
            }
            break;
            
        case BUCK_STATUS_SYNCHRONOUS:
            if (os.timeSlot & 0b00010000)
            {
                //Ensure that maximum battery voltage is not exceeded
                if (os.output_voltage>battery_voltage_maximum)
                {
                    _buck_set_dutycycle(buck_dutycycle-1);
                    buck_dutycycle_last_step = -1;
                }
                //Switch to asynchronous mode if current falls below threshold
                else if (os.input_current<BUCK_SYNCHRONOUS_INPUT_CURRENT_MINIMUM)
                {
                    _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS);
                    _buck_set_dutycycle(buck_dutycycle-5);
                    buck_status = BUCK_STATUS_ASYNCHRONOUS;
                }
                //Adjust duty cycle in order to track maximum point of power
                else
                {
                    //Current power level
                    /*
                    idx = os.timeSlot & 0b00110000;
                    idx >>= 4;
                    now = (int32_t)os.input_voltage_adc[idx];
                    now *= os.input_current_adc[idx];
                    //Previous power level
                    idx += 2;
                    idx &= 0b11;
                    last = (int32_t)os.input_voltage_adc[idx];
                    last *= os.input_current_adc[idx];
                    */
                    /*
                    idx = os.timeSlot & 0b00110000;
                    now = os.output_voltage_adc[idx];
                    idx += 2;
                    idx &= 0b11;
                    last = os.output_voltage_adc[idx];
                    */
                    idx = os.timeSlot & 0b00110000;
                    idx >>= 4;
                    now = (int32_t)os.input_voltage_adc[idx];
                    now *= (int32_t) os.input_current_adc[idx];
                    //Previous power level
                    idx += 2;
                    idx &= 0b11;
                    last = (int32_t)os.input_voltage_adc[idx];
                    last *= (int32_t) os.input_current_adc[idx];
                    //Adjust duty cycle
                    if(os.input_voltage>17000)
                    {
                        _buck_set_dutycycle(buck_dutycycle+1);
                    }
                    else
                    {
                        _buck_set_dutycycle(buck_dutycycle-1);
                    } 
                    /*
                    if (now>last)
                    {
                        if (buck_dutycycle_last_step>0)
                        {
                            _buck_set_dutycycle(buck_dutycycle+1);
                            buck_dutycycle_last_step = +1;
                        }
                        else
                        {
                            _buck_set_dutycycle(buck_dutycycle-1);
                            buck_dutycycle_last_step = -1;
                        }
                    }
                    else
                    {
                        if (buck_dutycycle_last_step>0)
                        {
                            _buck_set_dutycycle(buck_dutycycle-1);
                            buck_dutycycle_last_step = -1;
                        }
                        else
                        {
                            _buck_set_dutycycle(buck_dutycycle+1);
                            buck_dutycycle_last_step = +1;
                        }
                    }*/
                }
            }
            break;
            
        default:
            buck_status = BUCK_STATUS_OFF;
    }
}
    
uint8_t buck_get_dutycycle(void)
{
    return buck_dutycycle;
}

buckStatus_t buck_get_mode(void)
{
    return buck_status;
}
            
    /*        
    if(system_buck_is_on())
    {
        if(os.input_voltage<14500)
        {
            system_buck_stop();
            i2c_expander_low(I2C_EXPANDER_CHARGER_ENABLE);
            i2c_expander_low(I2C_EXPANDER_FAN);
        } 

        //Adjust duty cycle
        system_buck_adjust_dutycycle();
    }
    else
    {
        if(os.input_voltage>15500)
        {
            //Enable buck
            i2c_expander_high(I2C_EXPANDER_CHARGER_ENABLE);
            i2c_expander_high(I2C_EXPANDER_FAN);
            //system_buck_set_frequency(BUCK_FREQUENCY_93750);
            system_buck_set_frequency(BUCK_FREQUENCY_187500);

            system_buck_set_dutycycle(system_buck_initial_dutycycle()); //Neutral duty cycle
            system_buck_start(); 
        }
    }
    */

/*
 
 * void system_buck_set_frequency(buckFrequency_t buckFrequency)
{
    switch(buckFrequency)
    {
        case BUCK_FREQUENCY_62500:
            system_set_cpu_frequency(CPU_FREQUENCY_8MHz);
            PR2 = 31;
            os.buckFrequency = BUCK_FREQUENCY_62500;
            break;
        case BUCK_FREQUENCY_93750:
            system_set_cpu_frequency(CPU_FREQUENCY_48MHz);
            PR2 = 127;
            os.buckFrequency = BUCK_FREQUENCY_93750;
            break;
        case BUCK_FREQUENCY_187500:
            system_set_cpu_frequency(CPU_FREQUENCY_48MHz);
            PR2 = 63;
            os.buckFrequency = BUCK_FREQUENCY_187500;
            break;
    }
}

void system_buck_set_dutycycle(uint8_t dutyCycle)
{
    uint16_t dc = dutyCycle;
    
    //Check limits
    if(dutyCycle<BUCK_DUTYCYCLE_MINIMUM)
        dutyCycle = dutyCycle<BUCK_DUTYCYCLE_MINIMUM;
    if(dutyCycle>BUCK_DUTYCYCLE_MAXIMUM)
        dutyCycle = dutyCycle<BUCK_DUTYCYCLE_MAXIMUM;
    
    //Save dutyCycle
    os.buckDutyCycle = dutyCycle;
            
    switch(os.buckFrequency)
    {
        case BUCK_FREQUENCY_62500:
            dc >>= 1;
            break;
        case BUCK_FREQUENCY_93750:
            dc <<= 1;
            break;
        case BUCK_FREQUENCY_187500:
            //do nothing
            break;
    }    
    
    //Lowest two bits of duty cycle
    CCP1CONbits.DC1B = (dc&0b11);
    //High byte of duty cycle
    CCPR1L = dc >> 2;
}

void system_buck_adjust_dutycycle(void)
{
    int32_t power;
    int32_t power_last;
    
    power = os.input_voltage * os.input_current;
    power_last = os.input_voltage_last * os.input_current_last;
    
    if(os.output_voltage>BUCK_BATTERY_VOLTAGE_MAXIMUM)
    {
        system_buck_set_dutycycle(os.buckDutyCycle-1);
        os.buckLastStep = -1;
    }
    else if(power>power_last)
    {
        if(os.buckLastStep>1)
        {
            system_buck_set_dutycycle(os.buckDutyCycle+1);
            os.buckLastStep = 1;
        }
        else
        {
            system_buck_set_dutycycle(os.buckDutyCycle-1);
            os.buckLastStep = -1;
        }
    }
    else
    {
        if(os.buckLastStep>1)
        {
            system_buck_set_dutycycle(os.buckDutyCycle-1);
            os.buckLastStep = -1;
        }
        else
        {
            system_buck_set_dutycycle(os.buckDutyCycle+1);
            os.buckLastStep = 1;
        }
    }
}



uint8_t system_buck_is_on(void)
{
    return T2CONbits.TMR2ON;
}
 
 */