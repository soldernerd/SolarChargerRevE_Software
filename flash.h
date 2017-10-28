/* 
 * File:   flash.h
 * Author: Luke
 *
 * Created on 8. Januar 2017, 17:13
 */

#ifndef FLASH_H
#define	FLASH_H


#define FLASH_PAGE_SIZE 512
#define FLASH_NUMBER_OF_PAGES 4096

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
void flash_page_read(uint16_t page, uint8_t *data);
void flash_page_write(uint16_t page, uint8_t *data);

//Read or write only part of a page
void flash_partial_read(uint16_t page, uint16_t start, uint16_t length, uint8_t *data);
void flash_partial_write(uint16_t page, uint16_t start, uint16_t length, uint8_t *data);

#endif	/* FLASH_H */

