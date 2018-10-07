/* 
 * File:   flash.h
 * Author: Luke
 *
 * Created on 8. Januar 2017, 17:13
 * 
 * Implements the communication with our AT45DB321E flash chip
 * This module needs to make sure we are the SPI_CONFIGURATION_INTERNAL configuration
 * Since this module is the only one using that configuration, it always sets the configuration back to SPI_CONFIGURATION_EXTERNAL
 * Configuration switching is implemented at the public function level, i.e the functions defined here
 * Static functions may safely assume that the configuration is already set correctly
 * 
 */

#ifndef FLASH_H
#define	FLASH_H

#include <stdint.h>


#define FLASH_PAGE_SIZE 512
#define FLASH_NUMBER_OF_PAGES 8192

typedef enum 
{ 
    FLASH_POWER_STATE_NORMAL,
    FLASH_POWER_STATE_DEEP_POWER_DOWN,
    FLASH_POWER_STATE_ULTRA_DEEP_POWER_DOWN
} flashPowerState_t;

//Initialize flash (and corresponding SPI interface)
void flash_init(void);

//Some utility functions
uint8_t flash_is_busy(void);
flashPowerState_t flash_get_power_state(void);
void flash_set_power_state(flashPowerState_t new_power_state);

//Read or write an entire page
void flash_sector_read(uint16_t page, uint8_t *data);
void flash_sector_write(uint16_t page, uint8_t *data);

//Read or write only part of a page
void flash_partial_read(uint16_t page, uint16_t start, uint16_t length, uint8_t *data);
void flash_partial_write(uint16_t page, uint16_t start, uint16_t length, uint8_t *data);

//Read or write access via FLASH_BUFFER_2
void flash_copy_page_to_buffer(uint16_t page);
void flash_write_page_from_buffer(uint16_t page);
void flash_read_from_buffer(uint16_t start, uint16_t length, uint8_t *data);
void flash_write_to_buffer(uint16_t start, uint16_t length, uint8_t *data);

#endif	/* FLASH_H */

