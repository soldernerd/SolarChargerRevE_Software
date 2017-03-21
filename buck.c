#include <xc.h>
#include <stdint.h>
#include <plib/pps.h>

#include "buck.h"
#include "os.h"
#include "i2c.h"

buckStatus_t buck_status;
uint8_t buck_dutycycle;
uint8_t buck_dutycycle_last_step;

//Variables for remote controlled operation
uint8_t buck_remote_enable = 0;
uint8_t buck_remote_on = 0;
uint8_t buck_remote_synchronous = 0;
uint8_t buck_remote_dutycycle = 0;

//Main functions
static void buck_operate_local(void);
static void buck_operate_remote(void);

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
    //Put some sane bounds around the duty cycle
    if(dc>BUCK_DUTYCYCLE_MAXIMUM)
        dc = BUCK_DUTYCYCLE_MAXIMUM;
    if(dc<BUCK_DUTYCYCLE_SYNCHRONOUS_MINIMUM)
        dc = BUCK_DUTYCYCLE_SYNCHRONOUS_MINIMUM;
    return (uint8_t) dc;
}

static void _buck_set_dutycycle(uint8_t dutyCycle)
{ 
    //Check limits
    if(dutyCycle>BUCK_DUTYCYCLE_MAXIMUM)
        dutyCycle = BUCK_DUTYCYCLE_MAXIMUM;
    if(buck_status==BUCK_STATUS_SYNCHRONOUS || buck_status==BUCK_STATUS_REMOTE_SYNCHRONOUS)
    {
        if(dutyCycle<BUCK_DUTYCYCLE_SYNCHRONOUS_MINIMUM)
            dutyCycle = BUCK_DUTYCYCLE_SYNCHRONOUS_MINIMUM;
        if(dutyCycle<BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM)
            dutyCycle = BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM;
    }
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
    
    //First of all check if remote control flag is set
    if(buck_remote_enable)
    {
        if(buck_status<BUCK_STATUS_REMOTE_OFF)
        {
            //We are not yet in remote controlled mode
            //Switch to remote controlled mode
            //Copy settings to ensure smooth cutover
            switch(buck_status)
            {
                case BUCK_STATUS_OFF:
                    buck_remote_on = 0;
                    buck_remote_synchronous = 0;
                    break;
                case BUCK_STATUS_ASYNCHRONOUS:
                    buck_remote_on = 1;
                    buck_remote_synchronous = 0;
                    break;
                case BUCK_STATUS_SYNCHRONOUS:
                    buck_remote_on = 1;
                    buck_remote_synchronous = 1;
                    break;  
            }
            buck_remote_dutycycle = buck_dutycycle;
            buck_status += BUCK_STATUS_REMOTE_OFF;
        }
        buck_operate_remote();
    }
    else
    {
       if(buck_status>=BUCK_STATUS_REMOTE_OFF)
        {
            //We are in remote controlled mode
            //Switch to local mode without changing anything else
            buck_status -= BUCK_STATUS_REMOTE_OFF;
        } 
        buck_operate_local();
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

//Actual control functionality
static void buck_operate_local(void)
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
                //Charge bootstrap capacitor
                buck_dutycycle_last_step = 1;
                PIR1bits.TMR2IF = 0;
                _buck_start(_buck_initial_dutycycle());
                while(!PIR1bits.TMR2IF);
                //Switch to asynchronous mode
                _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS);
                _buck_set_dutycycle(BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM);
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
                else if ((os.input_current<BUCK_INPUT_CURRENT_MINIMUM) || (os.input_voltage<os.output_voltage))
                {
                    _buck_stop();
                    buck_status = BUCK_STATUS_OFF;
                }
                //Ensure a minimum voltage difference (otherwise the bootstrap capacitor will lose its charge)
                else if (os.input_voltage - os.output_voltage < BUCK_VOLTAGE_DIFFERENCE_MINIMUM)
                {
                    _buck_set_dutycycle(buck_dutycycle-1);
                    buck_dutycycle_last_step = -1;
                }
                //Switch to synchronous mode if current exceeds threshold
                else if (os.input_current>BUCK_ASYNCHRONOUS_INPUT_CURRENT_MAXIMUM)
                {
                    _buck_set_sync_async(BUCK_MODE_SYNCHRONOUS);
                    _buck_set_dutycycle(_buck_initial_dutycycle()+5);
                    buck_status = BUCK_STATUS_SYNCHRONOUS;
                }
                //Adjust duty cycle in order to track maximum point of power
                else
                {
                    //Current power level
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
                }
            }
            break;
            
        default:
            buck_status = BUCK_STATUS_OFF;
    }
}

static void buck_operate_remote(void)
{
    uint8_t cntr;
    switch(buck_status)
    {
        case BUCK_STATUS_REMOTE_OFF:
            if((os.timeSlot&0b00110000)==0b00110000)
            {
               if(buck_remote_on)
                {
                    BUCK_ENABLE_PIN = 1;
                    buck_status = BUCK_STATUS_REMOTE_STARTUP;
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
            
        case BUCK_STATUS_REMOTE_STARTUP:
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
                //Charge bootstrap capacitor
                PIR1bits.TMR2IF = 0;
                _buck_start(_buck_initial_dutycycle());
                while(!PIR1bits.TMR2IF);
                //Switch to asynchronous mode unless requested otherwise
                if(buck_remote_synchronous)
                {
                    buck_status = BUCK_STATUS_REMOTE_SYNCHRONOUS;
                }
                else
                {
                    //Switch to asynchronous mode
                    _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS);
                    _buck_set_dutycycle(BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM);
                    buck_remote_dutycycle = buck_dutycycle;
                    buck_status = BUCK_STATUS_REMOTE_ASYNCHRONOUS; 
                }
                
            } 
            break;
            
        case BUCK_STATUS_REMOTE_ASYNCHRONOUS:
            _buck_set_dutycycle(buck_remote_dutycycle);
            if(!buck_remote_on)
            {
                _buck_stop();
                buck_status = BUCK_STATUS_REMOTE_OFF;
            }
            //Check if we should switch to synchronous mode
            else if(buck_remote_synchronous)
            {
                _buck_set_sync_async(BUCK_MODE_SYNCHRONOUS);
                _buck_set_dutycycle(_buck_initial_dutycycle()+5);
                buck_remote_dutycycle = buck_dutycycle;
                buck_status = BUCK_STATUS_REMOTE_SYNCHRONOUS;
            }
            
            break;
            
        case BUCK_STATUS_REMOTE_SYNCHRONOUS:
            _buck_set_dutycycle(buck_remote_dutycycle);
            if(!buck_remote_on)
            {
                _buck_stop();
                buck_status = BUCK_STATUS_REMOTE_OFF;
            }
            //Check if we should switch to asynchronous mode
            else if(!buck_remote_synchronous)
            {
                _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS);
                _buck_set_dutycycle(_buck_initial_dutycycle()-5);
                buck_remote_dutycycle = buck_dutycycle;
                buck_status = BUCK_STATUS_REMOTE_ASYNCHRONOUS;
            }
            break;
            
        default:
            _buck_stop();
            buck_status = BUCK_STATUS_REMOTE_OFF;
    }    
}

//Remote control functionality
void buck_remote_set_enable(uint8_t remote)
{
    if(remote)
        buck_remote_enable = 1;
    else
        buck_remote_enable = 0;
}

void buck_remote_set_on(uint8_t on)
{
    if(on)
        buck_remote_on = 1;
    else
        buck_remote_on = 0;
}

void buck_remote_set_synchronous(uint8_t synchronous)
{
    if(synchronous)
        buck_remote_synchronous = 1;
    else
        buck_remote_synchronous = 0;
}

void buck_remote_set_dutycycle(uint8_t dutycycle)
{
    if(dutycycle>BUCK_DUTYCYCLE_MAXIMUM)
        buck_remote_dutycycle = BUCK_DUTYCYCLE_MAXIMUM;
    else if(dutycycle<BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM)
        buck_remote_dutycycle = BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM;
    else
        buck_remote_dutycycle = (uint8_t) dutycycle;
}

void buck_remote_change_dutycycle(int8_t change)
{
    int16_t new_dutycycle = buck_dutycycle + change;
    if(new_dutycycle>BUCK_DUTYCYCLE_MAXIMUM)
        buck_remote_dutycycle = BUCK_DUTYCYCLE_MAXIMUM;
    else if(new_dutycycle<BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM)
        buck_remote_dutycycle = BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM;
    else
        buck_remote_dutycycle = (uint8_t) new_dutycycle;
}

uint8_t buck_remote_get_status()
{
    uint8_t retval = 0x00;
    if(buck_remote_enable)
        retval |= 0x01;
    if(buck_remote_on)
        retval |= 0x02;
    if(buck_remote_synchronous)
        retval |= 0x03;
    return retval;
}

uint8_t buck_remote_get_dutycycle()
{
    return buck_remote_dutycycle;
}