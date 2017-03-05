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
    BUCK_STATUS_OFF,
    //BUCK_STATUS_STANDBY,
    BUCK_STATUS_STARTUP,
    BUCK_STATUS_ASYNCHRONOUS,
    BUCK_STATUS_SYNCHRONOUS,
    BUCK_STATUS_DEBUGGING
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

#endif	/* BUCK_H */

