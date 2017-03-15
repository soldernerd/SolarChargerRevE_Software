// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2015 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/
//DOM-IGNORE-END

#ifndef  _FILEIO_MEDIA_H
#define  _FILEIO_MEDIA_H

// Enumeration to define media error types
typedef enum
{
    MEDIA_NO_ERROR,                     // No errors
    MEDIA_DEVICE_NOT_PRESENT,           // The requested device is not present
    MEDIA_CANNOT_INITIALIZE             // Cannot initialize media
} FILEIO_MEDIA_ERRORS;

// Media information flags.  The driver's MediaInitialize function will return a pointer to one of these structures.
typedef struct
{
    FILEIO_MEDIA_ERRORS errorCode;              // The status of the initialization FILEIO_MEDIA_ERRORS
    // Flags
    union
    {
        uint8_t    value;
        struct
        {
            uint8_t    sectorSize  : 1;         // The sector size parameter is valid.
            uint8_t    maxLUN      : 1;         // The max LUN parameter is valid.
        }   bits;
    } validityFlags;

    uint16_t    sectorSize;                     // The sector size of the target device.
    uint8_t    maxLUN;                          // The maximum Logical Unit Number of the device.
} FILEIO_MEDIA_INFORMATION;

#endif