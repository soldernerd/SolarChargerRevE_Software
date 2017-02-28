
#include <plib/i2c.h>
#include "i2c.h"
#include "os.h"


#define _XTAL_FREQ 8000000

#define I2C_WRITE 0x00
#define I2C_READ 0x01

#define I2C_DISPLAY_SLAVE_ADDRESS 0b01111000
#define I2C_DIGIPOT_SLAVE_ADDRESS 0b01011100
#define I2C_ADC_SLAVE_ADDRESS 0b11010000
#define I2C_EEPROM_SLAVE_ADDRESS 0b10100000

i2cFrequency_t i2c_frequency = I2C_FREQUENCY_100kHz;

/* ****************************************************************************
 * General I2C functionality
 * ****************************************************************************/

void i2c_init(void)
{
    OpenI2C1(MASTER, 1);
    //Set baud rate to 100kHz
    i2c_set_frequency(I2C_FREQUENCY_100kHz);
    //SSP1ADDbits.SSPADD = 4;
}

i2cFrequency_t i2c_get_frequency(void)
{
    return i2c_frequency;
}

void i2c_set_frequency(i2cFrequency_t frequency)
{
    switch(os.clockFrequency)
    {
        case CPU_FREQUENCY_32kHz:
            SSP1ADD = 0;     
            break;
        case CPU_FREQUENCY_8MHz:
            switch(frequency)
            {
                case I2C_FREQUENCY_100kHz:
                    SSP1ADD = 19;
                    break;
                case I2C_FREQUENCY_200kHz:
                    SSP1ADD = 9;
                    break;
                case I2C_FREQUENCY_400kHz:
                    SSP1ADD = 4;     
                    break;
            }
            break;
        case CPU_FREQUENCY_48MHz:
            switch(frequency)
            {
                case I2C_FREQUENCY_100kHz:
                    SSP1ADD = 119;
                    break;
                case I2C_FREQUENCY_200kHz:
                    SSP1ADD = 59;
                    break;
                case I2C_FREQUENCY_400kHz:
                    SSP1ADD = 29; 
                    break;
            }
            break;
    } 
    //Save new frequency
    i2c_frequency = frequency;
}


static void _i2c_write(uint8_t slave_address, uint8_t *data, uint8_t length)
{
    uint8_t cntr;

    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(slave_address);
    IdleI2C();
    
    for(cntr=0; cntr<length; ++cntr)
    {
        WriteI2C(data[cntr]);
        IdleI2C();      
    } 
    
    StopI2C();    
}

static void _i2c_read(uint8_t slave_address, uint8_t *data, uint8_t length)
{
    uint8_t cntr;

    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(slave_address | I2C_READ);
    IdleI2C();
    
    for(cntr=0; cntr<length; ++cntr)
    {
        data[cntr] = ReadI2C();
        AckI2C();       
    } 

    NotAckI2C();
    StopI2C();
}


/* ****************************************************************************
 * I2C Display Functionality
 * ****************************************************************************/

#define DISPLAY_RESET_PIN 0b00000100
#define DISPLAY_INSTRUCTION_REGISTER 0b00000000
#define DISPLAY_DATA_REGISTER 0b01000000
#define DISPLAY_CONTINUE_FLAG 0b10000000
#define DISPLAY_CLEAR 0b00000001
#define DISPLAY_SET_ADDRESS 0b10000000


//void i2c_display_init(void)
//{
//    uint8_t init_sequence[9] = {
//        0x3A, /* 8bit, 4line, RE=1 */
//        //0x1E, /* Set BS1 (1/6 bias) */
//        0b00011110, //0b00011110,
//        0x39, /* 8bit, 4line, IS=1, RE=0 */
//        //0x1C, /* Set BS0 (1/6 bias) + osc */
//        0b00001100, //0b00011100
//        0b1110000, /* Set contrast lower 4 bits */
//        0b1011100, /* Set ION, BON, contrast bits 4&5 */
//        0x6d, /* Set DON and amp ratio */
//        0x0c, /* Set display on */
//        0x01  /* Clear display */
//    };
//    
//    i2c_expander_high(I2C_EXPANDER_USER_INTERFACE);
//    
//    //Open I2C mux port
//    i2c_mux_open(I2C_MUX_DISPLAY);
//    
//    //Let voltage stabilize
//    __delay_ms(50);
//    
//    LATBbits.LATB0 = 0; //Backlight off, reset display
//    
//    //The display needs some start-up time
//    __delay_ms(10);
//    
//    LATBbits.LATB0 = 1; //Backlight on, start up display
//    
//    //The display needs some start-up time again
//    __delay_ms(5); 
//    
//    //Write initialization sequence
//    _i2c_write(I2C_DISPLAY_SLAVE_ADDRESS, &init_sequence[0], 9);
//}


void i2c_display_send_init_sequence(void)
{
    uint8_t init_sequence[9] = {
        0x3A, // 8bit, 4line, RE=1 
        //0x1E, // Set BS1 (1/6 bias) 
        0b00011110, //0b00011110,
        0x39, // 8bit, 4line, IS=1, RE=0 
        //0x1C, // Set BS0 (1/6 bias) + osc 
        0b00001100, //0b00011100
        0x74, //0b1110000, // Set contrast lower 4 bits 
        0b1011100, //Set ION, BON, contrast bits 4&5 
        0x6d, // Set DON and amp ratio
        0x0c, // Set display on
        0x01  // Clear display
    };
    
    //Set I2C frequency to 200kHz (display doesn't like 400kHz at startup)
    i2c_set_frequency(I2C_FREQUENCY_100kHz);

    //Write initialization sequence
    _i2c_write(I2C_DISPLAY_SLAVE_ADDRESS, &init_sequence[0], 9);
}

void i2c_display_init(void)
{
    uint8_t init_sequence[9] = {
        0x3A, // 8bit, 4line, RE=1
        //0x1E, // Set BS1 (1/6 bias)
        0b00011110, //0b00011110,
        0x39, // 8bit, 4line, IS=1, RE=0
        //0x1C, // Set BS0 (1/6 bias) + osc
        0b00001100, //0b00011100
        0x74, //0b1110000, // Set contrast lower 4 bits
        0b1011100, // Set ION, BON, contrast bits 4&5
        0x6d, // Set DON and amp ratio
        0x0c, // Set display on
        0x01  // Clear display
    };
    
    //Power UI on
    //i2c_expander_high(I2C_EXPANDER_USER_INTERFACE);
    system_delay_ms(10);
    //Enable on
    LATBbits.LATB0 = 1; 
    system_delay_ms(2);
    
    //Talk to digipot to pull display reset signal low
    i2c_digipot_reset_on();
    system_delay_ms(10);
    i2c_digipot_reset_off();
    system_delay_ms(5);
    
    //Set I2C frequency to 200kHz (display doesn't like 400kHz at startup)
    i2c_set_frequency(I2C_FREQUENCY_100kHz);

    //Write initialization sequence
    _i2c_write(I2C_DISPLAY_SLAVE_ADDRESS, &init_sequence[0], 9);
    
    //Turn backlight on
    i2c_digipot_backlight(30); 
}

void _ic2_display_set_address(uint8_t address)
{
    uint8_t data_array[2];
    data_array[0] = DISPLAY_INSTRUCTION_REGISTER;
    data_array[1] = DISPLAY_SET_ADDRESS | address;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_100kHz);
    
    //Set address
    _i2c_write(I2C_DISPLAY_SLAVE_ADDRESS, &data_array[0], 2);
}

void i2c_display_cursor(uint8_t line, uint8_t position)
{
    uint8_t address;
    
    //Figure out display address
    line &= 0b11;
    address = line<<5;
    position &= 0b11111;
    address |= position;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_100kHz);
    
    //Set address
    _ic2_display_set_address(address);
}

void i2c_display_write(char *data)
{
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_100kHz);

    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(I2C_DISPLAY_SLAVE_ADDRESS);
    IdleI2C();
    WriteI2C(DISPLAY_DATA_REGISTER);
    IdleI2C();
    
    //Print entire (zero terminated) string
    while(*data)
    {
        WriteI2C(*data++);
        IdleI2C();        
    } 
    
    StopI2C();
}

void i2c_display_write_fixed(char *data, uint8_t length)
{
    uint8_t pos;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_100kHz);

    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(I2C_DISPLAY_SLAVE_ADDRESS);
    IdleI2C();
    WriteI2C(DISPLAY_DATA_REGISTER);
    IdleI2C();
    
    //Print entire (zero terminated) string
    for(pos=0; pos<length; ++pos)
    {
        WriteI2C(data[pos]);
        IdleI2C();        
    } 
    
    StopI2C();    
}


/* ****************************************************************************
 * I2C Dual Digipot Functionality
 * ****************************************************************************/


#define DIGIPOT_MEMORY_ADDRESS_VOLATILE_WIPER_0 0x00
#define DIGIPOT_MEMORY_ADDRESS_VOLATILE_WIPER_1 0x10
#define DIGIPOT_MEMORY_ADDRESS_NONVOLATILE_WIPER_0 0x20
#define DIGIPOT_MEMORY_ADDRESS_NONVOLATILE_WIPER_1 0x30

#define DIGIPOT_COMMAND_WRITE 0x00
#define DIGIPOT_COMMAND_INCREMENT 0x01
#define DIGIPOT_COMMAND_DECREMENT 0x02
#define DIGIPOT_COMMAND_READ 0x03

void i2c_digipot_set_defaults(void)
{
    uint8_t data_array[2];
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    data_array[0] = DIGIPOT_MEMORY_ADDRESS_NONVOLATILE_WIPER_0 | DIGIPOT_COMMAND_WRITE;
    data_array[1] = 0x00;
    
    _i2c_write(I2C_DIGIPOT_SLAVE_ADDRESS, &data_array[0], 2);
    
    system_delay_ms(10);

    data_array[0] = DIGIPOT_MEMORY_ADDRESS_NONVOLATILE_WIPER_1 | DIGIPOT_COMMAND_WRITE;
    data_array[1] = 0x00;
    
    _i2c_write(I2C_DIGIPOT_SLAVE_ADDRESS, &data_array[0], 2);  
}

void i2c_digipot_reset_on(void)
{
    uint8_t data_array[2];
    data_array[0] = DIGIPOT_MEMORY_ADDRESS_VOLATILE_WIPER_1 | DIGIPOT_COMMAND_WRITE;
    //data_array[0] = 0b00010000;
    data_array[1] = 0x00;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    _i2c_write(I2C_DIGIPOT_SLAVE_ADDRESS, &data_array[0], 2);
}

void i2c_digipot_reset_off(void)
{
    uint8_t data_array[2];
    data_array[0] = DIGIPOT_MEMORY_ADDRESS_VOLATILE_WIPER_1 | DIGIPOT_COMMAND_WRITE;
    data_array[1] = 0x80;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    _i2c_write(I2C_DIGIPOT_SLAVE_ADDRESS, &data_array[0], 2);  
}

void i2c_digipot_backlight(uint8_t level)
{
    uint8_t data_array[2];
    data_array[0] = DIGIPOT_MEMORY_ADDRESS_VOLATILE_WIPER_0 | DIGIPOT_COMMAND_WRITE;
    data_array[1] = level>>1; //There are only 128 levels on this digipot
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    _i2c_write(I2C_DIGIPOT_SLAVE_ADDRESS, &data_array[0], 2);
}

/* ****************************************************************************
 * I2C ADC Functionality
 * ****************************************************************************/


void i2c_adc_start(i2cAdcPort_t channel, i2cAdcResolution_t resolution, i2cAdcGain_t gain)
 {
     uint8_t configuration_byte;
     configuration_byte = 0b10000000;
     configuration_byte |= (channel<<5);
     configuration_byte |= (resolution<<2);
     configuration_byte |= gain;
     
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
     
     _i2c_write(I2C_ADC_SLAVE_ADDRESS, &configuration_byte, 1);
 }
 
 int16_t i2c_adc_read(void)
 {
    int16_t result;

    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);

    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(I2C_ADC_SLAVE_ADDRESS | I2C_READ);
    IdleI2C();
    result = ReadI2C();
    result <<= 8;
    AckI2C();
    result |= ReadI2C();
    NotAckI2C();
    StopI2C(); 
    
    return result;
 };
 
/* ****************************************************************************
 * I2C EEPROM Functionality
 * ****************************************************************************/
 
void i2c_eeprom_writeByte(uint16_t address, uint8_t data)
{
    uint8_t slave_address;
    uint8_t dat[2];
    
    slave_address = I2C_EEPROM_SLAVE_ADDRESS | ((address&0b0000011100000000)>>7);
    dat[0] = address & 0xFF;
    dat[1] = data;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    _i2c_write(slave_address, &dat[0], 2);
}

uint8_t i2c_eeprom_readByte(uint16_t address)
{
    uint8_t slave_address;
    uint8_t addr;
    slave_address = I2C_EEPROM_SLAVE_ADDRESS | ((address&0b0000011100000000)>>7);
    addr = address & 0xFF;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    _i2c_write(slave_address, &addr, 1);
    _i2c_read(slave_address, &addr, 1);
    return addr;
}


void i2c_eeprom_write(uint16_t address, uint8_t *data, uint8_t length)
{
    uint8_t cntr;
    uint8_t slave_address;
    uint8_t dat[17];

    slave_address = I2C_EEPROM_SLAVE_ADDRESS | ((address&0b0000011100000000)>>7);
    dat[0] = address & 0xFF;

    length &= 0b00001111;
    for(cntr=0; cntr<length; ++cntr)
    {
        dat[cntr+1] = data[cntr];
    }
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    _i2c_write(slave_address, &dat[0], length+1);
}

void i2c_eeprom_read(uint16_t address, uint8_t *data, uint8_t length)
{
    uint8_t slave_address;
    uint8_t addr;
    slave_address = I2C_EEPROM_SLAVE_ADDRESS | ((address&0b0000011100000000)>>7);
    addr = address & 0xFF;
    
    //Set I2C frequency to 400kHz
    i2c_set_frequency(I2C_FREQUENCY_400kHz);
    
    _i2c_write(slave_address, &addr, 1);
    _i2c_read(slave_address, &data[0], length);
}
