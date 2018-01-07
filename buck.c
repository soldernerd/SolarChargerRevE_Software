#include <xc.h>
#include <stdint.h>
//#include <plib/pps.h>

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
static void _buck_operate_local(void);
static void _buck_operate_remote(void);

//Utility functions
static void _buck_pin_init(void);
static void _buck_timer2_init(void);
static uint8_t _buck_initial_dutycycle(void);
static void _buck_set_dutycycle(uint8_t dutyCycle);
static void _buck_prepare(void);
static void _buck_start(buckMode_t mode, uint8_t dutycycle);
static void _buck_set_sync_async(buckMode_t mode, uint8_t new_dutycycle);
static void _buck_stop(void);

//Calibration parameters
extern calibration_t calibrationParameters[7];

uint8_t idx;
int32_t last;
int32_t now;
int16_t battery_voltage_maximum = 13500;

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
    BUCK_LOWFET_PPS = PPS_FUNCTION_CCP1_OUTPUT_A;
    BUCK_HIGHFET_PPS = PPS_FUNCTION_CCP2_OUTPUT_B;
    PPSLock();
    //PPSOutput(PPS_RP6, PPS_CCP1P1A);
    //PPSOutput(PPS_RP5, PPS_P1B);
}

static void _buck_timer2_init(void)
{
    //Use timer2 for CCP1 module
    //TCLKCONbits.T3CCP1 = 0; //This is PIC18F46J50 code
    CCPTMRS0bits.C1TSEL = 0b000;
    
    //Post scaler = 16
    T2CONbits.T2OUTPS = 0b1111;
    
    //Prescaler = 1
    T2CONbits.T2CKPS1 = 0;
    T2CONbits.T2CKPS0 = 0;
    
    //Disable timer 2
    T2CONbits.TMR2ON = 0;
}

static uint8_t _buck_initial_dutycycle(void)
{
    uint32_t dc;
    //This happens only in testing when no panel is connected
    if(os.input_voltage<os.output_voltage)
        return BUCK_DUTYCYCLE_MAXIMUM;
    dc = (uint32_t) os.output_voltage;
    dc <<= 8;
    dc /= (uint16_t) os.input_voltage;
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
    }
    if(buck_status==BUCK_STATUS_ASYNCHRONOUS || buck_status==BUCK_STATUS_REMOTE_ASYNCHRONOUS)
    {
        if(dutyCycle<BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM)
            dutyCycle = BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM;
    }
    //Save dutyCycle
    buck_dutycycle = dutyCycle;
    //What follows is critical code, we don't want to get interrupted, turn interrupts off
    INTCONbits.GIE = 0;
    //Lowest two bits of duty cycle
    CCP1CONbits.DC1B = (uint8_t) (dutyCycle&0b11);
    //High byte of duty cycle
    CCPR1L = (uint8_t) (dutyCycle>>2);
    //Re-enable interrupts
    INTCONbits.GIE = 1;
}

static void _buck_prepare(void)
{
    uint8_t cntr;
    BUCK_ENABLE_PIN = 1;
    if(buck_status<0x80)
        buck_status = BUCK_STATUS_STARTUP;
    else
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

static void _buck_start(buckMode_t mode, uint8_t dutycycle)
{
    uint8_t startup_dutycycle;
    uint8_t cntr;
    
    //Calibrate current measurements
    calibrationParameters[CALIBRATION_INDEX_INPUT_CURRENT].AutoCalibration = 0;
    calibrationParameters[CALIBRATION_INDEX_OUTPUT_CURRENT].AutoCalibration = 0;
    //os.input_current_calibration = 0;
    //os.output_current_calibration = 0;
    for(cntr=0;cntr<4;++cntr)
    {
        calibrationParameters[CALIBRATION_INDEX_INPUT_CURRENT].AutoCalibration -= os.input_current_adc[cntr];
        calibrationParameters[CALIBRATION_INDEX_OUTPUT_CURRENT].AutoCalibration -= os.output_current_adc[cntr];
        //os.input_current_calibration += os.input_current_adc[cntr];
        //os.output_current_calibration += os.output_current_adc[cntr];
    }
    
    //Prepare timer2
    //Set frequency to 187.5kHz
    PR2 = 63; //
    //Clear timer 2
    TMR2 = 0x00;
    //Post scaler = 16
    T2CONbits.T2OUTPS = 0b1111; 
    //Clear interrupt flag
    PIR1bits.TMR2IF = 0; 
    
    //Set status to appropriate synchronous status (needed for limit-checking...)
    if(buck_status<0x80)
    {
        buck_status = BUCK_STATUS_SYNCHRONOUS;
    }
    else
    {
        buck_status = BUCK_STATUS_REMOTE_SYNCHRONOUS;
    }
    
    //Reset module
    CCP1CON = 0x00;
    //Calculate duty cycle. Buck always starts up with this neutral duty cycle
    startup_dutycycle = _buck_initial_dutycycle();
    //Set duty cycle. 
    _buck_set_dutycycle(startup_dutycycle);
    //What follows is critical code, we don't want to get interrupted, turn interrupts off
    INTCONbits.GIE = 0;
    //Enable timer 2
    T2CONbits.TMR2ON = 1;
    //Both outputs active high
    CCP1CON |= 0b00001100;
    //Half bridge mode
    CCP1CON |= 0b10000000;  
    //Buck is now running. Re-enable interrupts
    INTCONbits.GIE = 1;
    
    //Wait for interrupt flag to get set (i.e. 16 cycles)
    while(!PIR1bits.TMR2IF);
    
    //Check if we need to change operating mode and/or duty cycle
    if(mode==BUCK_MODE_SYNCHRONOUS)
    {
        //We already are in synchronous mode. Change duty cycle if necessary
        if(dutycycle!=startup_dutycycle)
        {
            _buck_set_dutycycle(dutycycle);
        }
    }
    else
    {
        //Need to switch to async mode
        _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS, dutycycle);
    }
}

static void _buck_set_sync_async(buckMode_t mode, uint8_t new_dutycycle)
{
    switch(mode)
    {
        //Switch to asynchronous mode
        //Disable lower fet first, then change dutycycle
        case BUCK_MODE_ASYNCHRONOUS:
            //Wait for a cycle to complete
            //This is probably unnecessary but I want to make absolutely sure the output behaves exactly the same every time
            T2CONbits.T2OUTPS = 0b0000; //Post scaler = 1
            //What follows is critical code, we don't want to get interrupted, turn interrupts off
            INTCONbits.GIE = 0;
            //Clear interrupt flag
            PIR1bits.TMR2IF = 0; 
            //Wait for interrupt flag to get set
            while(!PIR1bits.TMR2IF);
            //Single output mode
            CCP1CONbits.P1M1 = 0;
            CCP1CONbits.P1M0 = 0;
            //Set new buck status (do this first, it's needed for limit checking)
            if(buck_status<0x80)
            {
                buck_status = BUCK_STATUS_ASYNCHRONOUS;
            }
            else
            {
                buck_status = BUCK_STATUS_REMOTE_ASYNCHRONOUS;
            }
            //Now set new dutycycle
            _buck_set_dutycycle(new_dutycycle);
            //Re-enable interrupts
            INTCONbits.GIE = 1;
            break;
            
        //Switch to synchronous mode    
        //Change dutycycle first, then enable low fet on
        case BUCK_MODE_SYNCHRONOUS:
            //Set new buck status (do this first, it's needed for limit checking)
            if(buck_status<0x80)
            {
                buck_status = BUCK_STATUS_SYNCHRONOUS;
            }
            else
            {
                buck_status = BUCK_STATUS_REMOTE_SYNCHRONOUS;
            }
            //Wait for a cycle to complete
            //This is probably unnecessary but I want to make absolutely sure the output behaves exactly the same every time
            T2CONbits.T2OUTPS = 0b0000; //Post scaler = 1
            //What follows is critical code, we don't want to get interrupted, turn interrupts off
            INTCONbits.GIE = 0;
            //Clear interrupt flag
            PIR1bits.TMR2IF = 0; 
            //Wait for interrupt flag to get set
            while(!PIR1bits.TMR2IF);
            //Set dutycycle first
            _buck_set_dutycycle(new_dutycycle);
            //Half-bridge mode
            CCP1CONbits.P1M1 = 1;
            CCP1CONbits.P1M0 = 0;
            //Re-enable interrupts
            INTCONbits.GIE = 1;
            break;
    }
}

static void _buck_stop(void)
{
    //Wait for a cycle to complete
    //This is probably unnecessary but I want to make absolutely sure the output behaves exactly the same every time
    T2CONbits.T2OUTPS = 0b0000; //Post scaler = 1
    PIR1bits.TMR2IF = 0; //Clear interrupt flag
    while(!PIR1bits.TMR2IF);//Wait for interrupt flag to get set
    //Turn outputs off
    CCP1CONbits.CCP1M = 0b0000;
    //Disable timer 2
    T2CONbits.TMR2ON = 0;
    //Disconnect power & panel
    BUCK_ENABLE_PIN = 0;
    //Change buck status
    if(buck_status<0x80)
    {
        buck_status = BUCK_STATUS_OFF;
    }
    else
    {
        buck_status = BUCK_STATUS_REMOTE_OFF;
    }
}

//Actual control functionality
static void _buck_operate_local(void)
{
    uint8_t cntr;
    uint8_t tmp_dutycycle;
    
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
                _buck_start(BUCK_MODE_ASYNCHRONOUS, _buck_initial_dutycycle());
                
            } 
            break;
            
        case BUCK_STATUS_ASYNCHRONOUS:
            if(os.timeSlot&0b00010000)
            {
                //Ensure that maximum battery voltage is not exceeded
                if(os.output_voltage>battery_voltage_maximum)
                {
                    _buck_set_dutycycle((uint8_t)(buck_dutycycle-1));
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
                    _buck_set_dutycycle((uint8_t)(buck_dutycycle-1));
                    buck_dutycycle_last_step = -1;
                }
                //Switch to synchronous mode if current exceeds threshold
                else if (os.input_current>BUCK_ASYNCHRONOUS_INPUT_CURRENT_MAXIMUM)
                {
                    tmp_dutycycle = (uint8_t) (_buck_initial_dutycycle()+5);
                    _buck_set_sync_async(BUCK_MODE_SYNCHRONOUS, tmp_dutycycle);
                    //_buck_set_dutycycle();
                    //buck_status = BUCK_STATUS_SYNCHRONOUS;
                }
                //Adjust duty cycle in order to track maximum point of power
                else
                {
                    //Current power level
                    idx = (uint8_t) (os.timeSlot & 0b00110000);
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
                    tmp_dutycycle = buck_dutycycle-5;
                    _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS, tmp_dutycycle);
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

static void _buck_operate_remote(void)
{
    switch(buck_status)
    {
        case BUCK_STATUS_REMOTE_OFF:
            if((os.timeSlot&0b00110000)==0b00110000)
            {
               if(buck_remote_on)
                {
                   //Apply power and do some clean-up
                   //New status will be BUCK_STATUS_REMOTE_STARTUP
                   _buck_prepare();
                }  
            } 
            break;
            
        case BUCK_STATUS_REMOTE_STARTUP:
            if((os.timeSlot&0b00110000)==0b00110000)
            {
                //Turn buck on and save dutycycle to remote
                if(buck_remote_synchronous)
                {
                    //New status will be BUCK_STATUS_REMOTE_SYNCHRONOUS
                    _buck_start(BUCK_MODE_SYNCHRONOUS, _buck_initial_dutycycle());
                    buck_remote_dutycycle = buck_dutycycle;
                }
                else
                {
                    //New status will be BUCK_STATUS_REMOTE_ASYNCHRONOUS
                    _buck_start(BUCK_MODE_ASYNCHRONOUS, BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM);
                    buck_remote_dutycycle = buck_dutycycle;
                }
            } 
            break;
            
        case BUCK_STATUS_REMOTE_ASYNCHRONOUS:
            //Check if we should turn the buck off
            if(!buck_remote_on)
            {
                //New status will be BUCK_STATUS_REMOTE_OFF
                _buck_stop();
            }
            //Check if we should switch to synchronous mode
            else if(buck_remote_synchronous)
            {
                //New status will be BUCK_STATUS_REMOTE_ASYNCHRONOUS
                //Always switch to synchronous mode with a neutral duty cycle. Overwrite buck_remote_dutycycle
                buck_remote_dutycycle = _buck_initial_dutycycle();
                _buck_set_sync_async(BUCK_MODE_SYNCHRONOUS, buck_remote_dutycycle); 
            }
            else
            {
                //Check if duty cycle needs to be changed
                if(buck_dutycycle!=buck_remote_dutycycle)
                {
                   _buck_set_dutycycle(buck_remote_dutycycle); 
                }
            }
            break;
            
        case BUCK_STATUS_REMOTE_SYNCHRONOUS:
            _buck_set_dutycycle(buck_remote_dutycycle);
            if(!buck_remote_on)
            {
                //New status will be BUCK_STATUS_REMOTE_OFF
                _buck_stop();
            }
            //Check if we should switch to asynchronous mode
            else if(!buck_remote_synchronous)
            {
                //New status will be BUCK_STATUS_REMOTE_ASYNCHRONOUS
                buck_remote_dutycycle = _buck_initial_dutycycle()-5;
                _buck_set_sync_async(BUCK_MODE_ASYNCHRONOUS, buck_remote_dutycycle);
            }
            else
            {
                //Check if duty cycle needs to be changed
                if(buck_dutycycle!=buck_remote_dutycycle)
                {
                   _buck_set_dutycycle(buck_remote_dutycycle); 
                }
            }
            break;
            
        default:
            //New status will be BUCK_STATUS_REMOTE_OFF
            _buck_stop();
    }    
}

void buck_init(void)
{
    buck_status = BUCK_STATUS_OFF;   
    _buck_timer2_init();
    _buck_pin_init();
}

void buck_operate(void)
{  
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
        _buck_operate_remote();
    }
    else
    {
       if(buck_status>=BUCK_STATUS_REMOTE_OFF)
        {
            //We are in remote controlled mode
            //Switch to local mode without changing anything else
            buck_status -= BUCK_STATUS_REMOTE_OFF;
        } 
        _buck_operate_local();
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