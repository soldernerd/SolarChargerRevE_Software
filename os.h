/* 
 * File:   system.h
 * Author: Luke
 *
 * Created on 5. September 2016, 21:17
 */

#ifndef OS_H
#define	OS_H

#include <stdint.h>

/*
 * Configuration bits
 */

/*
// CONFIG1L
#pragma config WDTEN = OFF      // Watchdog Timer (Disabled - Controlled by SWDTEN bit)
#pragma config PLLDIV = 2       // PLL Prescaler Selection bits (Divide by 2 (8 MHz oscillator input))
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset  (Enabled)
#pragma config XINST = OFF       // Extended Instruction Set (Enabled)

// CONFIG1H
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)
//#pragma config CPUDIV = OSC2_PLL2// CPU System Clock Postscaler (CPU system clock divide by 2)
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)

// CONFIG2L
#pragma config OSC = HSPLL      // Oscillator (HS+PLL, USB-HS+PLL)
#pragma config T1DIG = ON       // T1OSCEN Enforcement (Secondary Oscillator clock source may be selected)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator (High-power operation)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = ON        // Internal External Oscillator Switch Over Mode (Enabled)

// CONFIG2H
#pragma config WDTPS = 32768    // Watchdog Postscaler (1:32768)

// CONFIG3L
#pragma config DSWDTOSC = INTOSCREF// DSWDT Clock Select (DSWDT uses INTRC)
#pragma config RTCOSC = T1OSCREF// RTCC Clock Select (RTCC uses T1OSC/T1CKI)
#pragma config DSBOREN = ON     // Deep Sleep BOR (Enabled)
#pragma config DSWDTEN = ON     // Deep Sleep Watchdog Timer (Enabled)
#pragma config DSWDTPS = G2     // Deep Sleep Watchdog Postscaler (1:2,147,483,648 (25.7 days))

// CONFIG3H
#pragma config IOL1WAY = OFF    // IOLOCK One-Way Set Enable bit (The IOLOCK bit (PPSCON<0>) can be set and cleared as needed)
#pragma config MSSP7B_EN = MSK7 // MSSP address masking (7 Bit address masking mode)

// CONFIG4L
#pragma config WPFP = PAGE_63   // Write/Erase Protect Page Start/End Location (Write Protect Program Flash Page 63)
#pragma config WPEND = PAGE_WPFP// Write/Erase Protect Region Select (valid when WPDIS = 0) (Page WPFP<5:0> through Configuration Words erase/write protected)
#pragma config WPCFG = OFF      // Write/Erase Protect Configuration Region (Configuration Words page not erase/write-protected)

// CONFIG4H
#pragma config WPDIS = OFF      // Write Protect Disable bit (WPFP<5:0>/WPEND region ignored)
*/

/*
 * General definitions
 */

#define _XTAL_FREQ 8000000

#define PIN_INPUT           1
#define PIN_OUTPUT          0
#define PIN_DIGITAL         1
#define PIN_ANALOG          0

#define VCC_HIGH_TRIS TRISDbits.TRISD0
#define VCC_HIGH_PORT LATDbits.LD0
#define PWR_GOOD_TRIS TRISDbits.TRISD1
#define PWR_GOOD_PORT PORTDbits.RD1
#define DISP_EN_TRIS TRISCbits.TRISC2
#define DISP_EN_PORT LATCbits.LC2

#define USBCHARGER_EN_TRIS TRISDbits.TRISD3
#define USBCHARGER_EN_PORT LATDbits.LD3

#define SPI_MISO_TRIS TRISDbits.TRISD5
#define SPI_MISO_PORT LATDbits.LD5
#define SPI_MISO_PPS PPS_RP22
#define SPI_MOSI_TRIS TRISDbits.TRISD6
#define SPI_MOSI_PORT LATDbits.LD6
#define SPI_MOSI_PPS PPS_RP23
#define SPI_SCLK_TRIS TRISDbits.TRISD4
#define SPI_SCLK_PORT LATDbits.LD4
#define SPI_SCLK_PPS PPS_RP24
#define SPI_SS_TRIS TRISDbits.TRISD7
#define SPI_SS_PORT LATDbits.LD7
#define SPI_SS_PPS PPS_RP21


#define VOLTAGE_REFERENCE_TRIS TRISAbits.TRISA3
#define TEMPERATURE1_TRIS TRISBbits.TRISB0
#define TEMPERATURE1_ANCON ANCON1bits.PCFG12
#define TEMPERATURE1_CHANNEL 0b1100
#define TEMPERATURE2_TRIS TRISAbits.TRISA1
#define TEMPERATURE2_ANCON ANCON0bits.PCFG1
#define TEMPERATURE2_CHANNEL 0b0001
#define TEMPERATURE3_TRIS TRISAbits.TRISA2
#define TEMPERATURE3_ANCON ANCON0bits.PCFG2
#define TEMPERATURE3_CHANNEL 0b0010

#define NUMBER_OF_TIMESLOTS 16

#define PUSHBUTTON_BIT PORTAbits.RA0
#define ENCODER_A_BIT PORTBbits.RB7
#define ENCODER_B_BIT PORTBbits.RB6

#define USB_CHARGING_VOLTAGE_MINIMUM 12000
#define POWER_OUTPUTS_VOLTAGE_MINIMUM 12000
#define BUCK_DUTYCYCLE_MINIMUM 40
#define BUCK_DUTYCYCLE_MAXIMUM 242
#define BUCK_BATTERY_VOLTAGE_MAXIMUM 13500

#define EEPROM_RTCC_ADDRESS 0x0040

#define OS_USER_INTERFACE_TIMEOUT 2000


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
    DISPLAY_MODE_TEMPERATURE = 0x60
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
    OUTPUT_USB = 0x10
} outputs_t;

typedef enum
{
    BOARD_VOLTAGE_LOW,
    BOARD_VOLTAGE_HIGH 
} boardVoltage_t;

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
} os_t;


/*
 * Global variables
 */

os_t os;


/*
 * Function prototypes
 */


void tmr_isr(void);
//void system_init(void);
//void system_set_cpu_frequency(clockFrequency_t newFrequency);
void system_delay_ms(uint8_t ms);
//void system_power_save(void);
void system_encoder_enable(void);
void system_encoder_disable(void);

/*

uint8_t system_output_is_on(outputs_t output);
void system_output_on(outputs_t output);
void system_output_off(outputs_t output);
void system_output_toggle(outputs_t output);

void system_buck_set_frequency(buckFrequency_t buckFrequency);
void system_buck_set_dutycycle(uint8_t dutyCycle);
void system_buck_adjust_dutycycle(void);
void system_buck_start(void);
void system_buck_stop(void);

void system_calculate_input_voltage();
void system_calculate_output_voltage();
void system_calculate_input_current();
void system_calculate_output_current();
 * */

#endif	/* OS_H */

