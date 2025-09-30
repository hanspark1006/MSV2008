/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)

typedef struct _ip_net_t
{
	uint8_t ipaddr[4];
	uint16_t port;
}ip_net_t;

typedef struct _config_t{
	uint32_t	valid;
	uint8_t		remote_mode;
	uint8_t		out_mode;
#if	ENABLE_DIMMING_MODE
	uint8_t		op_mode;
#endif
	uint8_t		dev_id;
	uint8_t		ch_num;
	uint16_t	on_time[MAX_CHANNEL+1];   // Add all channel + 1
	uint16_t	delay_time[MAX_CHANNEL+1];
	uint8_t		edge;
#if	ENABLE_DIMMING_MODE
	uint8_t		duty_level[MAX_CHANNEL+1];
	uint16_t	period_time;
#endif
	ip_net_t	ether;
	uint8_t		company[16]; // LINE_CHARACTER_MAX 16
	uint8_t		frontVer;
	uint8_t		backendVer;
}config_t;  // 72byte

#pragma pack(pop)
/* Exported constants --------------------------------------------------------*/
extern config_t	*m_app_config;
/* Exported macro ------------------------------------------------------------*/
enum{
	e2CH_MODE = 0,
	e4CH_MODE,
	e6CH_MODE,
	e8CH_MODE,
	SEL_MAX_CH
};

enum{
	eONE_ONE,
	eONE_N
};

enum{
	eFALLING,
	eRISING
};

enum{
	eSTROBE_MODE
#if	ENABLE_DIMMING_MODE
	, eDIMMING_MODE
#endif
};

enum{
	eREMOTE_NONE,
	eREMOTE_RS232,
	eREMOTE_ETHER
};
/* Exported functions ------------------------------------------------------- */
void app_config_init(void);
int app_save_config(void);
uint8_t app_read_config(config_t *pConfig);
//void app_set_default_config(void);
void app_set_default_config(uint8_t sel_ch);
#endif /* __APP_CONFIG_H */
