
#ifndef LOG_H
#define	LOG_H

#include <xc.h>
#include <stdint.h>

#include "os.h"
#include "fat16.h"

#define LOG_FILENAME "LOG     "
#define LOG_EXTENSION "TXT"
#define LOG_PERIOD 450

/* Log structure *
 * 32 bytes per entry, 1 entry every 7.5 minutes
 * Bytes 0-3: Date and time
 *      Year:       6 bits (2-digit year, i.e year - 2000)
 *      Month       4 bits
 *      Day         5 bits
 *      Hours       5 bits
 *      Minutes     6 bits
 *      Seconds:    6 bits
 * Bytes 4-5: Input voltage: uint16_t in millivolts
 * Bytes 6-7: Input current: uint16_t in milliamps
 * Bytes 8-9: Output voltage: uint16_t in millivolts
 * Bytes 10-11: Output current: uint16_t in milliamps
 * Bytes 12-13: Input power: uint16_t in 2 milliwatts
 * Bytes 14-15: Output power: uint16_t in 2 milliwatts
 * Bytes 16-17: Input capacity: uint16_t in Ws (watt-seconds)
 * Bytes 18-19: Output capacity: uint16_t in Ws (watt-seconds)
 * Byte 20: On-board temperature: uint8_t in 0.5 degrees centigrade above -40
 * Byte 21: External temperature 1: uint8_t in 0.5 degrees centigrade above -40
 * Byte 22: External temperature 2: uint8_t in 0.5 degrees centigrade above -40
 * Byte 23: Charger on-time: uint8_t in 2 seconds
 * Byte 24: Low-power state time: uint8_t in 2 seconds
 * ...
 * Byte 30: Status byte:
 *      Bit 7: Output 1 on
 *      Bit 6: Output 2 on
 *      Bit 5: Output 3 on
 *      Bit 4: Output 4 on
 *      Bit 3: USB charging on
 *      Bit 2: Fan on
 *      Bit 1: USB connected
 *      Bit 0: unused
 * Byte 31: Checksum: All other bytes XOR-ed
 * ****************************************************************************/

typedef struct
{
    uint32_t dateTime;
    uint16_t inputVoltage;
    uint16_t inputCurrent;
    uint16_t outputVoltage;
    uint16_t outputCurrent;
    uint16_t inputPower;
    uint16_t outputPower;
    uint16_t inputCapacity;
    uint16_t outputCapacity;
    uint8_t temperatureOnboard;
    uint8_t temperatureExternal1;
    uint8_t temperatureExternal2;
    uint8_t chargerOnTime;
    uint8_t lowPowerTime;
    uint8_t unused[5];
    uint8_t status;
    uint8_t checksum;
} logEntry_t;
 
void log_start_new(void);
void log_collect_data(void);
void log_generate_entry(logEntry_t *log_entry);
void log_write_to_disk(void);
uint16_t log_get_number_of_samples(void);

#endif	/* LOG_H */

