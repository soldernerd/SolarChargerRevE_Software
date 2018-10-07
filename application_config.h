/* 
 * File:   application_config.h
 * Author: luke
 *
 * Created on 3. Oktober 2018, 17:16
 */

#ifndef APPLICATION_CONFIG_H
#define	APPLICATION_CONFIG_H

/*
 * A random generated signature to distinguish between bootloader and normal firmware
 */

#define FIRMWARE_SIGNATURE 0x62A1

/*
 * Firmware version
 */

#define FIRMWARE_VERSION_MAJOR 0x00
#define FIRMWARE_VERSION_MINOR 0x01
#define FIRMWARE_VERSION_FIX 0x00

/*
 * Application specific settings
 */

#define REAL_TIME_CLOCK_AVAILABLE
#define ANALOG_DIGITAL_CONVERTER_AVAILABLE
#define I2C_TASK_SCHEDULING_AVAILABLE

#define NUMBER_OF_TIMESLOTS 16

#define USB_CHARGING_VOLTAGE_MINIMUM 12000
#define POWER_OUTPUTS_VOLTAGE_MINIMUM -1
#define BUCK_DUTYCYCLE_ASYNCHRONOUS_MINIMUM 40
#define BUCK_DUTYCYCLE_SYNCHRONOUS_MINIMUM 150
#define BUCK_DUTYCYCLE_MAXIMUM 242
#define BUCK_BATTERY_VOLTAGE_MAXIMUM 13500

//Page 0 (0x00 to 0xFF) reserved for parameters such as time and date
#define EEPROM_RTCC_ADDRESS 0x040

//Page 1 (0x100 to 0x1FF) reserved for bootloader
#define EEPROM_BOOTLOADER_BYTE_ADDRESS 0x100
#define BOOTLOADER_BYTE_FORCE_BOOTLOADER_MODE 0x94
#define BOOTLOADER_BYTE_FORCE_NORMAL_MODE 0x78

//Page 2 (0x200 to 0x2FF) reserved for calibration
#define EEPROM_CALIBRATION_ADDRESS 0x0200

#define OS_USER_INTERFACE_TIMEOUT 2000

#endif	/* APPLICATION_CONFIG_H */

