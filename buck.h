/* 
 * File:   buck.h
 * Author: Luke
 *
 * Created on 30. September 2016, 21:59
 */

#ifndef BUCK_H
#define	BUCK_H

#define BUCK_VOLTAGE_DIFFERENCE_MINIMUM 1000
#define BUCK_INPUT_CURRENT_MINIMUM 5
#define BUCK_ASYNCHRONOUS_INPUT_CURRENT_MAXIMUM 600
#define BUCK_SYNCHRONOUS_INPUT_CURRENT_MINIMUM 200

typedef enum 
{ 
    BUCK_STATUS_OFF = 0x00,
    BUCK_STATUS_STARTUP = 0x01,
    BUCK_STATUS_ASYNCHRONOUS = 0x02,
    BUCK_STATUS_SYNCHRONOUS = 0x03,
    BUCK_STATUS_REMOTE_OFF = 0x80,
    BUCK_STATUS_REMOTE_STARTUP = 0x81,
    BUCK_STATUS_REMOTE_ASYNCHRONOUS = 0x82,
    BUCK_STATUS_REMOTE_SYNCHRONOUS = 0x83
} buckStatus_t;

typedef enum 
{ 
    BUCK_MODE_ASYNCHRONOUS,
    BUCK_MODE_SYNCHRONOUS
} buckMode_t;

void buck_init(void);
void buck_operate(void);

uint8_t buck_get_dutycycle(void);
buckStatus_t buck_get_mode(void);

void buck_remote_set_enable(uint8_t remote);
void buck_remote_set_on(uint8_t on);
void buck_remote_set_synchronous(uint8_t synchronous);
void buck_remote_set_dutycycle(uint8_t dutycycle);
void buck_remote_change_dutycycle(int8_t change);

uint8_t buck_remote_get_status();
uint8_t buck_remote_get_dutycycle();

#endif	/* BUCK_H */

