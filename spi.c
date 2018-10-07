
#include <stdint.h>
#include <xc.h>
#include <string.h>
#include "hardware_config.h"
#include "os.h"
#include "spi.h"

/*****************************************************************************
 * Global Variables                                                          *
 *****************************************************************************/
spiConfigurationDetails_t config_internal;
spiConfigurationDetails_t config_external;
spiConfiguration_t active_configuration;

uint8_t _spi_external_tx_buffer[64];
uint8_t _spi_external_rx_buffer[64];
static uint8_t tx_buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};

/*****************************************************************************
 * Utility functions                                                         *
 * These are for internal use only and are used to implement the actual      *  
 * flash functionality                                                       *
 *****************************************************************************/

static void _spi_init(spiConfigurationDetails_t details)
{
    //This function needs to be adapted...
    //Needs to depend on the details provided...
    
    //Associate pins with MSSP module
    PPSUnLock();
    PPS_FUNCTION_SPI2_DATA_INPUT_REGISTER = SPI_MISO_RP_INPUT_VALUE;
    SPI_MOSI_RP_OUTPUT_REGISTER = PPS_FUNCTION_SPI2_DATA_OUTPUT_VALUE;
    //Careful: Clock needs to be mapped as an output AND an input
    SPI_SCLK_RP_OUTPUT_REGISTER = PPS_FUNCTION_SPI2_CLOCK_OUTPUT_VALUE;
    PPS_FUNCTION_SPI2_CLOCK_INPUT_REGISTER = SPI_SCLK_RP_INPUT_VALUE;
    PPSLock();
    
    //Configure and enable MSSP module in master mode
    SSP2STATbits.SMP = 1; //Sample at end
    SSP2STATbits.CKE = 1; //Active to idle
    SSP2CON1bits.CKP = 0; //Idle clock is low
    SSP2CON1bits.SSPM =0b0000; //SPI master mode, Fosc/4
    SSP2CON1bits.SSPEN = 1; //Enable SPI module
}

/*
static void _spi_init_master(void)
{

}

static void _spi_init_slave(void)
{
    //Yet to be implemented
}

static void _switch_mastermode(void)
{
    //Yet to be implemented...
}

static void _switch_slavemode(void)
{
    //Yet to be implemented...
}
 * 
 * */

/*****************************************************************************
 * Public functions                                                          *
 * Only these functions are made accessible via spi.h                        *  
 *****************************************************************************/

uint8_t* spi_get_external_tx_buffer(void)
{
    return _spi_external_tx_buffer;
}

uint8_t* spi_get_external_rx_buffer(void)
{
    return _spi_external_rx_buffer;
}

void spi_set_configurationDetails(spiConfiguration_t configuration, spiConfigurationDetails_t details)
{
    switch(configuration)
    {
        case SPI_CONFIGURATION_INTERNAL:
            config_internal.mode = details.mode;
            config_internal.frequency = details.frequency;
            config_internal.polarity = details.polarity;
            break;
           
        case SPI_CONFIGURATION_EXTERNAL:
            config_external.mode = details.mode;
            config_external.frequency = details.frequency;
            config_external.polarity = details.polarity;
            break;
    }
}

void spi_get_configurationDetails(spiConfiguration_t configuration, spiConfigurationDetails_t details)
{
    switch(configuration)
    {
        case SPI_CONFIGURATION_INTERNAL:
            details.mode = config_internal.mode;
            details.frequency = config_internal.frequency;
            details.polarity = config_internal.polarity;
            break;
           
        case SPI_CONFIGURATION_EXTERNAL:
            details.mode = config_external.mode;
            details.frequency = config_external.frequency;
            details.polarity = config_external.polarity;
            break;
    }
}

//Set up PPS pin mappings and initialize the SPI module
void spi_init(spiConfiguration_t configuration)
{
    switch(configuration)
    {
        case SPI_CONFIGURATION_INTERNAL:
            _spi_init(config_internal);
            break;
           
        case SPI_CONFIGURATION_EXTERNAL:
            _spi_init(config_external);
            break;
    }
    
    active_configuration = configuration;
}

//Switch between internal and external configuration
void spi_set_configuration(spiConfiguration_t configuration)
{
    uint8_t cntr;
    
    //Do whatever it takes to switch between the two
    switch(configuration)
    {
        case SPI_CONFIGURATION_INTERNAL:
            
            //A poor man's pull-up resistor
            //Force the slave-select pin high for just an instant before reading it
            //This has no effect if a SPI master is present controlling the signal
            //But it makes sure the software does not hang if the pin is left floating
            SPI_SS2_LAT = 1;
            SPI_SS2_TRIS = PIN_OUTPUT;
            SPI_SS2_TRIS = PIN_INPUT;
            //Wait while an external communication is in progress
            while(!SPI_SS2_PORT); //This may fail if no external pull up/down resistors are present
            

            DMACON1bits.DMAEN = 0; //Disable DMA module
            SSP2CON1bits.SSPEN = 0; //Disable SPI module

            //Set pin directions
            SPI_MISO_TRIS = PIN_INPUT;
            SPI_MOSI_TRIS = PIN_OUTPUT;
            SPI_MOSI_PIN = 0;
            SPI_SCLK_TRIS = PIN_OUTPUT;
            SPI_SCLK_PIN = 0;
            SPI_SS1_TRIS = PIN_OUTPUT;
            SPI_SS1_PIN = 1;
            
            //Associate pins with MSSP module
            PPSUnLock();
            //Reset unused PPS registers
            SPI_MISO_RP_OUTPUT_REGISTER = 0b00000;
            PPS_FUNCTION_SPI2_DATA_INPUT_REGISTER = 0b11111;
            PPS_FUNCTION_SPI2_CHIPSELECT_INPUT_REGISTER = 0b11111;
            //Set data pins
            PPS_FUNCTION_SPI2_DATA_INPUT_REGISTER = SPI_MISO_RP_INPUT_VALUE;
            SPI_MOSI_RP_OUTPUT_REGISTER = PPS_FUNCTION_SPI2_DATA_OUTPUT_VALUE;
            //Set clock pin. Careful: Clock needs to be mapped as an output AND an input
            SPI_SCLK_RP_OUTPUT_REGISTER = PPS_FUNCTION_SPI2_CLOCK_OUTPUT_VALUE;
            PPS_FUNCTION_SPI2_CLOCK_INPUT_REGISTER = SPI_SCLK_RP_INPUT_VALUE;
            PPSLock();

            //Configure and enable MSSP module in master mode
            SSP2STATbits.SMP = 1; //Sample at end
            SSP2STATbits.CKE = 1; //Active to idle
            SSP2CON1bits.WCOL = 0; //Clear write collision bit
            SSP2CON1bits.SSPOV = 0; //Clear receive overflow bit  
            SSP2CON1bits.CKP = 0; //Idle clock is low
            SSP2CON1bits.SSPM =0b0000; //SPI master mode, Fosc/4
            SSP2CON1bits.SSPEN = 1; //Enable SPI module

            //Save active configuration and return
            active_configuration = SPI_CONFIGURATION_INTERNAL;
            break;

        case SPI_CONFIGURATION_EXTERNAL:
            
            DMACON1bits.DMAEN = 0; //Disable DMA module
            SSP2CON1bits.SSPEN = 0; //Disable SPI module
            
            //Configure open drain control
            ODCON3bits.SPI2OD = 0; //Disable open drain capability

            //Configure PPS
            PPSUnLock();
            RPOR23 = 10; //MISO output
            RPINR21 = 21; //MOSI input
            RPINR22 = 22; //Clock input
            RPINR23 = 19; //Slave select input
            PPSLock();

            SSP2CON1bits.SSPEN = 0; //Disable SPI module

            #define PIN_INPUT           1
            #define PIN_OUTPUT          0
            #define PIN_DIGITAL         1
            #define PIN_ANALOG          0


            TRISDbits.TRISD5 = PIN_INPUT; //Set SCLK as input
            TRISDbits.TRISD4 = PIN_INPUT; //Set MOSI as input


            SPI_MISO_TRIS = PIN_OUTPUT;
            SPI_MISO_PIN = 1;
            //SPI_MOSI_TRIS = PIN_INPUT;
            //SPI_SCLK_TRIS = PIN_INPUT;
            SPI_SS2_TRIS = PIN_INPUT;

            //Associate pins with MSSP module


            //Configure and enable MSSP module in slave mode
            SSP2STATbits.SMP = 0; //Needs to be cleared in slave mode
            SSP2STATbits.CKE = 1; //Active to idle
            SSP2CON1bits.WCOL = 0; //Clear write collision bit
            SSP2CON1bits.SSPOV = 0; //Clear receive overflow bit        
            SSP2CON1bits.CKP = 0; //Idle clock is low
            SSP2CON1bits.SSPM =0b0100; //SPI Slave mode, clock = SCKx pin; SSx pin control enabled
            PIR3bits.SSP2IF = 0; //Clear interrupt flag
            SSP2CON1bits.SSPEN = 1; //Enable SPI module

            //Configure DMA module
            DMACON1bits.TXINC = 1; //TX address increment enabled
            DMACON1bits.RXINC = 1; //RX address increment enabled
            DMACON1bits.DUPLEX1 = 1; //Full duplex mode
            DMACON1bits.DUPLEX0 = 0; //Full duplex mode
            DMACON1bits.DLYINTEN = 0; //Disable delay interrupt

            DMACON2bits.DLYCYC = 0b0000; //No timeouts
            DMACON2bits.INTLVL = 0b0000; //Interrupt when transfer is complete

            //Set number of bytes to transmit
            DMABCH = HIGH_BYTE((uint16_t) (64-1));
            DMABCL = LOW_BYTE((uint16_t) (64-1));

            //Set TX buffer address
            TXADDRH =  HIGH_BYTE((uint16_t) _spi_external_tx_buffer);
            TXADDRL =  LOW_BYTE((uint16_t) _spi_external_tx_buffer);

            //Set RX buffer address
            RXADDRH =  HIGH_BYTE((uint16_t) _spi_external_rx_buffer);
            RXADDRL =  LOW_BYTE((uint16_t) _spi_external_rx_buffer);
            
            DMACON1bits.DMAEN = 1; //Enable DMA module

//            SSP2CON1bits.SSPEN = 0; //Disable SPI module
//
//            //Set pin directions
//            SPI_MISO_TRIS = PIN_OUTPUT;
//            SPI_MISO_PIN = 0;
//            SPI_MOSI_TRIS = PIN_INPUT;
//            SPI_SCLK_TRIS = PIN_INPUT;
//            SPI_SS2_TRIS = PIN_INPUT;
//
//            //Associate pins with MSSP module
//            PPSUnLock();
//            //Reset unused PPS registers
//            PPS_FUNCTION_SPI2_DATA_INPUT_REGISTER = 0b11111;
//            SPI_MOSI_RP_OUTPUT_REGISTER = 0b00000;
//            //Set data pins
//            SPI_MISO_RP_OUTPUT_REGISTER = PPS_FUNCTION_SPI2_DATA_OUTPUT_VALUE;
//            PPS_FUNCTION_SPI2_DATA_INPUT_REGISTER = SPI_MOSI_RP_INPUT_VALUE;
//            //Set clock pin. Careful: Clock needs to be mapped as an output AND an input
//            SPI_SCLK_RP_OUTPUT_REGISTER = PPS_FUNCTION_SPI2_CLOCK_OUTPUT_VALUE;
//            PPS_FUNCTION_SPI2_CLOCK_INPUT_REGISTER = SPI_SCLK_RP_INPUT_VALUE;
//            //Set chip select pin
//            PPS_FUNCTION_SPI2_CHIPSELECT_INPUT_REGISTER = SPI_SS2_RP_INPUT_VALUE;
//            PPSLock();
//
//            //Configure and enable MSSP module in slave mode
//            SSP2STATbits.SMP = 0; //Needs to be cleared in slave mode
//            SSP2STATbits.CKE = 1; //Active to idle
//            SSP2CON1bits.WCOL = 0; //Clear write collision bit
//            SSP2CON1bits.SSPOV = 0; //Clear receive overflow bit        
//            SSP2CON1bits.CKP = 0; //Idle clock is low
//            SSP2CON1bits.SSPM =0b0100; //SPI Slave mode, clock = SCKx pin; SSx pin control enabled
//            PIR3bits.SSP2IF = 0; //Clear interrupt flag
//            SSP2CON1bits.SSPEN = 1; //Enable SPI module
//            
//            //Configure DMA module
//            DMACON1bits.TXINC = 1; //TX address increment enabled
//            DMACON1bits.RXINC = 1; //RX address increment enabled
//            DMACON1bits.DUPLEX1 = 1; //Full duplex mode
//            DMACON1bits.DUPLEX0 = 0; //Full duplex mode
//            DMACON1bits.DLYINTEN = 0; //Disable delay interrupt
//            
//            DMACON2bits.DLYCYC = 0b0000; //No timeouts
//            DMACON2bits.INTLVL = 0b0000; //Interrupt when transfer is complete
//            
//            //Set number of bytes to transmit
//            DMABCH = HIGH_BYTE((uint16_t) (8-1));
//            DMABCL = LOW_BYTE((uint16_t) (8-1));
//            
//            //Set TX buffer address
//            TXADDRH =  HIGH_BYTE((uint16_t) _spi_external_tx_buffer);
//            TXADDRL =  LOW_BYTE((uint16_t) _spi_external_tx_buffer);
//            
//            //Set RX buffer address
//            RXADDRH =  HIGH_BYTE((uint16_t) _spi_external_rx_buffer);
//            RXADDRL =  LOW_BYTE((uint16_t) _spi_external_rx_buffer);
//            
//            //Dummy data
//            for(cntr=0; cntr<64; ++cntr)
//            {
//                _spi_external_tx_buffer[cntr] = cntr + 0x70;
//                //_spi_external_rx_buffer[cntr] = cntr + 0x80;
//            }
//            
//            DMACON1bits.DMAEN = 1; //Enable DMA module

            //Save active configuration and return
            active_configuration = SPI_CONFIGURATION_EXTERNAL;
            break;
    }   
}

spiConfiguration_t spi_get_configuration(void)
{
    return active_configuration;
}

//Transmits a number of bytes via SPI using the DMA module
//Typically used to send a command
//Caution: this function does NOT check if the flash is busy or in low-power mode
void spi_tx(uint8_t *data, uint16_t length)
{
    //Slave Select not controlled by DMA module
    DMACON1bits.SSCON1 = 0;
    DMACON1bits.SSCON0 = 0; 
    //Do increment TX address
    DMACON1bits.TXINC = 1; 
    //Do not increment RX address
    DMACON1bits.RXINC = 0; 
    //Half duplex, transmit only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 1;
    //Disable delay interrupts
    DMACON1bits.DLYINTEN = 0; 
    //1 cycle delay only
    DMACON2bits.DLYCYC = 0b0000; 
    //Only interrupt after transfer is completed
    DMACON2bits.INTLVL = 0b0000; 
    
    //Set TX buffer address
    TXADDRH =  HIGH_BYTE((uint16_t) data);
    TXADDRL =  LOW_BYTE((uint16_t) data);
    
    //Set number of bytes to transmit
    DMABCH = HIGH_BYTE((uint16_t) (length-1));
    DMABCL = LOW_BYTE((uint16_t) (length-1));
    
    //Perform actual transfer
    SPI_SS1_PIN = 0; //Enable slave select pin 
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete
    SPI_SS1_PIN = 1; //Disable slave select pin 
}

//Transmits a number of bytes via SPI using the DMA module
//Similar to _flash_spi_tx but uses two different data sources
//Typically used to send a command followed by data
//Caution: this function does NOT check if the flash is busy or in low-power mode
void spi_tx_tx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length)
{
    //Slave Select not controlled by DMA module
    DMACON1bits.SSCON1 = 0;
    DMACON1bits.SSCON0 = 0; 
    //Do increment TX address
    DMACON1bits.TXINC = 1; 
    //Do not increment RX address
    DMACON1bits.RXINC = 0; 
    //Half duplex, transmit only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 1;
    //Disable delay interrupts
    DMACON1bits.DLYINTEN = 0; 
    //1 cycle delay only
    DMACON2bits.DLYCYC = 0b0000; 
    //Only interrupt after transfer is completed
    DMACON2bits.INTLVL = 0b0000; 
    
    //Set TX buffer address for command
    TXADDRH =  HIGH_BYTE((uint16_t) command);
    TXADDRL =  LOW_BYTE((uint16_t) command);
    
    //Set number of bytes to transmit for command
    DMABCH = HIGH_BYTE((uint16_t) (command_length-1));
    DMABCL = LOW_BYTE((uint16_t) (command_length-1));
    
    //Enable slave select pin 
    SPI_SS1_PIN = 0; 
    
    //Perform transfer of command
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete  
    
    //Set TX buffer address for data
    TXADDRH =  HIGH_BYTE((uint16_t) data);
    TXADDRL =  LOW_BYTE((uint16_t) data);
    
    //Set number of bytes to transmit for actual data
    DMABCH = HIGH_BYTE((uint16_t) (data_length-1));
    DMABCL = LOW_BYTE((uint16_t) (data_length-1));
    
    //Perform transfer of data
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete
    
    //Disable slave select pin 
    SPI_SS1_PIN = 1;  
}

//Transmits and then receives a number of bytes via SPI using the DMA module
//Typically used to send a command in order to receive data
//Caution: this function does NOT check if the flash is busy or in low-power mode
void spi_tx_rx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length)
{
    //Slave Select not controlled by DMA module
    DMACON1bits.SSCON1 = 0;
    DMACON1bits.SSCON0 = 0; 
    //Do increment TX address
    DMACON1bits.TXINC = 1; 
    //Do not increment RX address
    DMACON1bits.RXINC = 0; 
    //Half duplex, transmit only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 1;
    //Disable delay interrupts
    DMACON1bits.DLYINTEN = 0; 
    //1 cycle delay only
    DMACON2bits.DLYCYC = 0b0000; 
    //Only interrupt after transfer is completed
    DMACON2bits.INTLVL = 0b0000; 
    
    //Set TX buffer address
    TXADDRH =  HIGH_BYTE((uint16_t) command);
    TXADDRL =  LOW_BYTE((uint16_t) command);
    
    //Set number of bytes to transmit
    DMABCH = HIGH_BYTE((uint16_t) (command_length-1));
    DMABCL = LOW_BYTE((uint16_t) (command_length-1));
    
    //Enable slave select pin 
    SPI_SS1_PIN = 0; 
    
    //Perform transfer of command
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete  
    
    //Do not increment TX address
    DMACON1bits.TXINC = 0; 
    //Do increment RX address
    DMACON1bits.RXINC = 1; 
    //Half duplex, receive only
    DMACON1bits.DUPLEX1 = 0;
    DMACON1bits.DUPLEX0 = 0;
    
    //Set RX buffer address
    RXADDRH =  HIGH_BYTE((uint16_t) data);
    RXADDRL =  LOW_BYTE((uint16_t) data);
    
    //Set number of bytes to transmit
    DMABCH = HIGH_BYTE((uint16_t) (data_length-1));
    DMABCL = LOW_BYTE((uint16_t) (data_length-1));
    
    //Perform transfer of data
    DMACON1bits.DMAEN = 1; //Start transfer
    while(DMACON1bits.DMAEN); //Wait for transfer to complete
    
    //Disable slave select pin 
    SPI_SS1_PIN = 1;
}