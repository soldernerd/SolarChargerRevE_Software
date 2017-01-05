/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

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

#include "system.h"


#include "usb.h"
#include "usb_device_msd.h"

#include "internal_flash.h"


//The LUN variable definition is critical to the MSD function driver.  This
//  array is a structure of function pointers that are the functions that
//  will take care of each of the physical media.  For each additional LUN
//  that is added to the system, an entry into this array needs to be added
//  so that the stack can know where to find the physical layer functions.
//  In this example the media initialization function is named
//  "MediaInitialize", the read capacity function is named "ReadCapacity",
//  etc.
LUN_FUNCTIONS LUN[MAX_LUN + 1] =
{
    {
        (FILEIO_MEDIA_INFORMATION* (*)(void *))&FILEIO_InternalFlash_MediaInitialize,
        (uint32_t (*)(void *))&FILEIO_InternalFlash_CapacityRead,
        (uint16_t (*)(void *))&FILEIO_InternalFlash_SectorSizeRead,
        (bool  (*)(void *))&FILEIO_InternalFlash_MediaDetect,
        (uint8_t  (*)(void *, uint32_t, uint8_t*))&FILEIO_InternalFlash_SectorRead,
        (uint8_t  (*)(void *))&FILEIO_InternalFlash_WriteProtectStateGet,
        (uint8_t  (*)(void *, uint32_t, uint8_t *, uint8_t))&FILEIO_InternalFlash_SectorWrite,
        (void *)NULL
    }
};

/* Standard Response to INQUIRY command stored in ROM 	*/
const InquiryResponse inq_resp = {
	0x00,		// peripheral device is connected, direct access block device
	0x80,           // removable
	0x04,	 	// version = 00=> does not conform to any standard, 4=> SPC-2
	0x02,		// response is in format specified by SPC-2
	0x20,		// n-4 = 36-4=32= 0x20
	0x00,		// sccs etc.
	0x00,		// bque=1 and cmdque=0,indicates simple queuing 00 is obsolete,
			// but as in case of other device, we are just using 00
	0x00,		// 00 obsolete, 0x80 for basic task queuing
	{'M','i','c','r','o','c','h','p'},    // this is the T10 assigned Vendor ID
	{'M','a','s','s',' ','S','t','o','r','a','g','e',' ',' ',' ',' '},
	{'0','0','0','1'}
};


/*********************************************************************
* Function: void APP_DeviceMSDInitialize(void);
*
* Overview: Initializes the Custom HID demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceMSDInitialize()
{
    #if (MSD_DATA_IN_EP == MSD_DATA_OUT_EP)
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    #else
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
        USBEnableEndpoint(MSD_DATA_OUT_EP,USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    #endif

    USBMSDInit();
}

/*********************************************************************
* Function: void APP_DeviceMSDTasks(void);
*
* Overview: Keeps the Custom HID demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceMSDInitialize() and APP_DeviceMSDStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceMSDTasks()
{
    /* If the USB device isn't configured yet, we can't really do anything
     * else since we don't have a host to talk to.  So jump back to the
     * top of the while loop. */
    if( USBGetDeviceState() < CONFIGURED_STATE )
    {
        return;
    }

    /* If we are currently suspended, then we need to see if we need to
     * issue a remote wakeup.  In either case, we shouldn't process any
     * keyboard commands since we aren't currently communicating to the host
     * thus just continue back to the start of the while loop. */
    if( USBIsDeviceSuspended()== true )
    {
        return;
    }
    
    MSDTasks();
}