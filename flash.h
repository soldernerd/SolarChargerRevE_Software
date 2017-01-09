/* 
 * File:   flash.h
 * Author: Luke
 *
 * Created on 8. Januar 2017, 17:13
 */

#ifndef FLASH_H
#define	FLASH_H

void flash_init(void);
void flash_dummy_write(void);
void flash_dummy_read();

//void flash_send(uint32_t address, uint8_t *dat, uint16_t len);

#endif	/* FLASH_H */

