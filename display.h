/* 
 * File:   display.h
 * Author: Luke
 *
 * Created on 11. September 2016, 13:52
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

void display_prepare(uint8_t mode);
void display_update(void);

uint8_t display_get_character(uint8_t line, uint8_t position);

#endif	/* DISPLAY_H */

