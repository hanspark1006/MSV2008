/*
 * apps.h
 *
 *  Created on: Feb 6, 2025
 *      Author: catsa
 */

#ifndef SRC_APPS_APPS_H_
#define SRC_APPS_APPS_H_

#define KEY_ENTER_PIN		0x1
#define KEY_UP_PIN			0x2
#define KEY_DOWN_PIN		0x4
#define KEY_MODE_PIN		0x8
#define KEY_CUR_UP_PIN		(KEY_MODE_PIN | KEY_UP_PIN)
#define KEY_CUR_DOWN_PIN	(KEY_MODE_PIN | KEY_DOWN_PIN)
#define KEY_FACTORY_PIN		(KEY_ENTER_PIN | KEY_MODE_PIN)
#define KEY_USER_PIN		(KEY_ENTER_PIN | KEY_UP_PIN)
#define KEY_VERSION_PIN		(KEY_ENTER_PIN | KEY_DOWN_PIN)
#define KEY_CHSEL_PIN		(KEY_UP_PIN | KEY_DOWN_PIN)


typedef enum{
	eKey_Idle = 0,
	eKey_Enter,
	eKey_Up,
	eKey_User = 0x03, 		// Enter + Up
	eKey_Down = 0x04,
	eKey_Version = 0x05, //Enter + Down
	eKey_ChSel = 0x06, // Up+Down		5sec
	eKey_Mode = 0x08,
	eKey_Factory = 0x09, // Mode + Enter 5 sec
	eKey_CursorUp = 0x0A, // Mode+Up 5sec
	eKey_CursorDown = 0x0C, // Mode + Down
	eKey_SetMode = 0x10,
	eKey_ExitRemote=0x20
}Key_t;

int apps_init(void);
void apps_set_blink_enable(uint8_t enable);
void apps_set_remote_mode(void);
void AppsTask(void);
void key_handler(void);
#endif /* SRC_APPS_APPS_H_ */
