/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SCREEN_H
#define __SCREEN_H

/* Includes ------------------------------------------------------------------*/
#include "apps.h"
/* Exported types ------------------------------------------------------------*/
typedef enum{
	eMAIN_SC,
	eCHANNEL_SC,
	eSET_ON_TIME_SC,
	eSET_DELAY_TIME_SC,
	eSET_TRIGGER_SC,
#if	ENABLE_DIMMING_MODE
	eSET_LEVEL_SC,
#endif
	eOUTMODE_SC,
#if	ENABLE_DIMMING_MODE
	eOPMODE_SC,
#endif
	eUARTMODE_SC,
	eETH_IP_SC,
	eETH_PORT_SC,
	//eTESTMODE_SC,
	eREMOTE_SC,
	eUSERMODE_SC,
	eFACTORY_SC,
	eCHSELECT_SC,
	eVERSION_SC,
	eMAX_SCREEN_ID,
	eERROR_SC
}ScreenID_t;
/* Exported constants --------------------------------------------------------*/
enum{
	eCH1,
	eCH2,
	eCH3,
	eCH4,
	eCH5,
	eCH6,
	eCH7,
	eCH8,
	eCHALL,
	eCH_MAX
};

enum{
	eCH_INIT,
	eCH_SET_ON,
	eCH_SET_DELAY,
	eCH_SET_EDGE
};
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void screen_init(void);
ScreenID_t screen_process(ScreenID_t screen_id, Key_t key);
void screen_send_config(void);
void screen_blink(void);
void screen_sync_complete(uint8_t MainVer, uint8_t FpgaVer);
#endif /* __SCREEN_H */
