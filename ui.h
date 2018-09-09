/* 
 * File:   ui.h
 * Author: Luke
 *
 * Created on 16. Oktober 2016, 22:31
 */

#ifndef UI_H
#define	UI_H

typedef enum
{
	USER_INTERFACE_STATUS_OFF,
	USER_INTERFACE_STATUS_STARTUP_1,
	USER_INTERFACE_STATUS_STARTUP_2,
	USER_INTERFACE_STATUS_STARTUP_3,
	USER_INTERFACE_STATUS_STARTUP_4,
	USER_INTERFACE_STATUS_ON
} userInterfaceStatus_t;

void ui_init(void);
void ui_run(void);
userInterfaceStatus_t ui_get_status(void);

#endif	/* UI_H */

