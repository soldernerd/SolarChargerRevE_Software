/* 
 * File:   spi.h
 * Author: luke
 *
 * Created on 26. Juli 2018, 15:18
 */

#ifndef SPI_H
#define	SPI_H

#include <stdint.h>

typedef enum
{
    SPI_CONFIGURATION_INTERNAL,
    SPI_CONFIGURATION_EXTERNAL
} spiConfiguration_t;

typedef enum 
{ 
    SPI_MODE_MASTER,
    SPI_MODE_SLAVE
} spiMode_t;

typedef enum
{ 
    SPI_FREQUENCY_12MHZ
} spiFrequency_t;

typedef enum
{
    SPI_POLARITY_ACTIVELOW,
    SPI_POLARITY_ACTIVEHIGH
} spiPolarity_t;

typedef struct
{
    spiMode_t mode;
    spiFrequency_t frequency;
    spiPolarity_t polarity;
} spiConfigurationDetails_t;

//Read or set a certain configuration
void spi_set_configurationDetails(spiConfiguration_t configuration, spiConfigurationDetails_t details);
void spi_get_configurationDetails(spiConfiguration_t configuration, spiConfigurationDetails_t details);

//Initialize and configure SPI interface
void spi_init(spiConfiguration_t configuration);
void spi_set_configuration(spiConfiguration_t configuration);
spiConfiguration_t spi_get_configuration(void);

void spi_tx(uint8_t *data, uint16_t length);
void spi_tx_tx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length);
void spi_tx_rx(uint8_t *command, uint16_t command_length, uint8_t *data, uint16_t data_length);

uint8_t* spi_get_external_tx_buffer(void);
uint8_t* spi_get_external_rx_buffer(void);

#endif	/* SPI_H */

