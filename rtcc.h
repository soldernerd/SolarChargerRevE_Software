/* 
 * File:   rtcc.h
 * Author: Luke
 *
 * Created on 25. September 2016, 13:32
 */

#ifndef RTCC_H
#define	RTCC_H

#include <stdint.h>

void rtcc_init(void);
void rtcc_correct_day(void);

//Year
uint8_t rtcc_get_year(void);
void rtcc_set_year(uint8_t year);
void rtcc_increment_year(void);
void rtcc_decrement_year(void);

//Month
uint8_t rtcc_get_month(void);
void rtcc_set_month(uint8_t month);
void rtcc_increment_month(void);
void rtcc_decrement_month(void);

//Day
uint8_t rtcc_get_day(void);
void rtcc_set_day(uint8_t day);
void rtcc_increment_day(void);
void rtcc_decrement_day(void);

//Hours
uint8_t rtcc_get_hours(void);
void rtcc_set_hours(uint8_t hours);
void rtcc_increment_hours(void);
void rtcc_decrement_hours(void);

//Minutes
uint8_t rtcc_get_minutes(void);
void rtcc_set_minutes(uint8_t minutes);
void rtcc_increment_minutes(void);
void rtcc_decrement_minutes(void);

//Seconds
uint8_t rtcc_get_seconds(void);
void rtcc_set_seconds(uint8_t seconds);
void rtcc_increment_seconds(void);
void rtcc_decrement_seconds(void);

//Read and write date and time from/to EEPROM
void rtcc_read_eeprom(void);
void rtcc_write_eeprom(void);

#endif	/* RTCC_H */

