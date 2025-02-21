/*
 * fpga_proc.h
 *
 *  Created on: May 7, 2024
 *      Author: catsa
 */

#ifndef SRC_M_SERIAL_FPGA_PROC_H_
#define SRC_M_SERIAL_FPGA_PROC_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
#define MAX_OUTPUT_MODE		2
#define MAX_CHANNEL			8

#define TRIGGER_TIME_OUT	20	// 2sec

#pragma pack(push)
#pragma pack(1)
typedef union{
	uint8_t channel_data[4];
	uint32_t channel_bit;
}uChBit;

typedef union{
	uint8_t array_time[2];
	uint16_t u16_time;
}uChtime;

typedef struct{
	uint8_t header;
	uint8_t addr;
	uint8_t data1;
	uint8_t data2;
}fpga_data_t;

typedef struct{
	uint32_t	input;
	uint32_t	output;
}channel_t;

typedef struct{
	channel_t	one_n;
}out_config_t;

typedef struct
{
	uint8_t 		mode; // strobe only
	uint8_t			out_mode;
	out_config_t	out_cfg;
	uint8_t			trigger_edge;
	uint16_t		delay_time[MAX_CHANNEL];
	uint16_t 		on_time[MAX_CHANNEL];
	uint16_t		duty_time[MAX_CHANNEL];
	uint16_t		period_time[MAX_CHANNEL];
}fpga_config_t;
#pragma pack(pop)
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
uint32_t fpga_proc_init(void);
void fpga_push_buf(uint8_t ch);
int fpga_set_output_channel(uint8_t outmode, uint32_t channel);
int fpga_set_input_channel(uint8_t outmode, uint32_t channel);
void fpga_get_trigger_input(uint8_t *get_data);
uint8_t fpga_set_mode(uint8_t mode);
uint8_t fpga_select_output(uint8_t sel_output, uint8_t ex_type);
uint8_t fpga_select_edge(uint8_t sel_edge);
uint8_t fpga_pwm_reset(void);
int fpga_set_delay_time(uint8_t ch, uint16_t time);
int fpga_set_on_time(uint8_t ch, uint16_t time);
int fpga_set_duty_time(uint8_t ch, uint16_t time);
int fpga_set_period_time(uint8_t ch, uint16_t time);
int fpga_set_trigger_order(uint8_t *ch_order);
void fpga_set_factory_reset(void);
void fpga_get_trigger_status(uint8_t *pBuf);
void fpga_set_last_output_channel(void);
/* Private defines -----------------------------------------------------------*/
#define WRITE_HEADER		0x55
#define READ_HEADER			0xAA
#define TRIGGER_HEADER		0x33

#define INPUT_CH_H_REG		0x01
#define INPUT_CH_L_REG		0x02

#define OUTPUT_CH_H_REG		0x03
#define OUTPUT_CH_L_REG		0x04

#define OPERATION_MODE_REG	0x05
enum{
	eSTROBE_MODE,
	eDIMMING_MODE,
};

#define INOUT_SELECT_REG	0x06
enum{
	eONE_ONE,
	eONE_N,
};

#define TIGGER_INPUT_REG	0x07
enum{
	eTRIGGER_FALLING,
	eTRIGGER_RISING
};

#define DIMMIMG_RESET_REG	0x08

#define CH1_DELAY_TIME_REG	0x10
#define CH2_DELAY_TIME_REG	0x11
#define CH3_DELAY_TIME_REG	0x12
#define CH4_DELAY_TIME_REG	0x13
#define CH5_DELAY_TIME_REG	0x14
#define CH6_DELAY_TIME_REG	0x15
#define CH7_DELAY_TIME_REG	0x16
#define CH8_DELAY_TIME_REG	0x17
#define CH9_DELAY_TIME_REG	0x18
#define CH10_DELAY_TIME_REG	0x19
#define CH11_DELAY_TIME_REG	0x1A
#define CH12_DELAY_TIME_REG	0x1B
#define CH13_DELAY_TIME_REG	0x1C
#define CH14_DELAY_TIME_REG	0x1D
#define CH15_DELAY_TIME_REG	0x1E
#define CH16_DELAY_TIME_REG	0x1F
#define CH17_DELAY_TIME_REG	0x20
#define CH18_DELAY_TIME_REG	0x21
#define CH19_DELAY_TIME_REG	0x22
#define CH20_DELAY_TIME_REG	0x23
#define CH21_DELAY_TIME_REG	0x24
#define CH22_DELAY_TIME_REG	0x25
#define CH23_DELAY_TIME_REG	0x26
#define CH24_DELAY_TIME_REG	0x27
#define CH25_DELAY_TIME_REG	0x28
#define CH26_DELAY_TIME_REG	0x29
#define CH27_DELAY_TIME_REG	0x2A
#define CH28_DELAY_TIME_REG	0x2B
#define CH29_DELAY_TIME_REG	0x2C
#define CH30_DELAY_TIME_REG	0x2D
#define CH31_DELAY_TIME_REG	0x2E
#define CH32_DELAY_TIME_REG	0x2F

#define CH1_ON_TIME_REG		0x30
#define CH2_ON_TIME_REG		0x31
#define CH3_ON_TIME_REG		0x32
#define CH4_ON_TIME_REG		0x33
#define CH5_ON_TIME_REG		0x34
#define CH6_ON_TIME_REG		0x35
#define CH7_ON_TIME_REG		0x36
#define CH8_ON_TIME_REG		0x37
#define CH9_ON_TIME_REG		0x38
#define CH10_ON_TIME_REG	0x39
#define CH11_ON_TIME_REG	0x3A
#define CH12_ON_TIME_REG	0x3B
#define CH13_ON_TIME_REG	0x3C
#define CH14_ON_TIME_REG	0x3D
#define CH15_ON_TIME_REG	0x3E
#define CH16_ON_TIME_REG	0x3F
#define CH17_ON_TIME_REG	0x40
#define CH18_ON_TIME_REG	0x41
#define CH19_ON_TIME_REG	0x42
#define CH20_ON_TIME_REG	0x43
#define CH21_ON_TIME_REG	0x44
#define CH22_ON_TIME_REG	0x45
#define CH23_ON_TIME_REG	0x46
#define CH24_ON_TIME_REG	0x47
#define CH25_ON_TIME_REG	0x48
#define CH26_ON_TIME_REG	0x49
#define CH27_ON_TIME_REG	0x4A
#define CH28_ON_TIME_REG	0x4B
#define CH29_ON_TIME_REG	0x4C
#define CH30_ON_TIME_REG	0x4D
#define CH31_ON_TIME_REG	0x4E
#define CH32_ON_TIME_REG	0x4F

#define CH1_DUTY_TIME_REG	0x50
#define CH2_DUTY_TIME_REG	0x51
#define CH3_DUTY_TIME_REG	0x52
#define CH4_DUTY_TIME_REG	0x53
#define CH5_DUTY_TIME_REG	0x54
#define CH6_DUTY_TIME_REG	0x55
#define CH7_DUTY_TIME_REG	0x56
#define CH8_DUTY_TIME_REG	0x57
#define CH9_DUTY_TIME_REG	0x58
#define CH10_DUTY_TIME_REG	0x59
#define CH11_DUTY_TIME_REG	0x5A
#define CH12_DUTY_TIME_REG	0x5B
#define CH13_DUTY_TIME_REG	0x5C
#define CH14_DUTY_TIME_REG	0x5D
#define CH15_DUTY_TIME_REG	0x5E
#define CH16_DUTY_TIME_REG	0x5F
#define CH17_DUTY_TIME_REG	0x60
#define CH18_DUTY_TIME_REG	0x61
#define CH19_DUTY_TIME_REG	0x62
#define CH20_DUTY_TIME_REG	0x63
#define CH21_DUTY_TIME_REG	0x64
#define CH22_DUTY_TIME_REG	0x65
#define CH23_DUTY_TIME_REG	0x66
#define CH24_DUTY_TIME_REG	0x67
#define CH25_DUTY_TIME_REG	0x68
#define CH26_DUTY_TIME_REG	0x69
#define CH27_DUTY_TIME_REG	0x6A
#define CH28_DUTY_TIME_REG	0x6B
#define CH29_DUTY_TIME_REG	0x6C
#define CH30_DUTY_TIME_REG	0x6D
#define CH31_DUTY_TIME_REG	0x6E
#define CH32_DUTY_TIME_REG	0x6F

#define CH1_PERIOD_TIME_REG		0x70
#define CH2_PERIOD_TIME_REG		0x71
#define CH3_PERIOD_TIME_REG		0x72
#define CH4_PERIOD_TIME_REG		0x73
#define CH5_PERIOD_TIME_REG		0x74
#define CH6_PERIOD_TIME_REG		0x75
#define CH7_PERIOD_TIME_REG		0x76
#define CH8_PERIOD_TIME_REG		0x77
#define CH9_PERIOD_TIME_REG		0x78
#define CH10_PERIOD_TIME_REG	0x79
#define CH11_PERIOD_TIME_REG	0x7A
#define CH12_PERIOD_TIME_REG	0x7B
#define CH13_PERIOD_TIME_REG	0x7C
#define CH14_PERIOD_TIME_REG	0x7D
#define CH15_PERIOD_TIME_REG	0x7E
#define CH16_PERIOD_TIME_REG	0x7F
#define CH17_PERIOD_TIME_REG	0x80
#define CH18_PERIOD_TIME_REG	0x81
#define CH19_PERIOD_TIME_REG	0x82
#define CH20_PERIOD_TIME_REG	0x83
#define CH21_PERIOD_TIME_REG	0x84
#define CH22_PERIOD_TIME_REG	0x85
#define CH23_PERIOD_TIME_REG	0x86
#define CH24_PERIOD_TIME_REG	0x87
#define CH25_PERIOD_TIME_REG	0x88
#define CH26_PERIOD_TIME_REG	0x89
#define CH27_PERIOD_TIME_REG	0x8A
#define CH28_PERIOD_TIME_REG	0x8B
#define CH29_PERIOD_TIME_REG	0x8C
#define CH30_PERIOD_TIME_REG	0x8D
#define CH31_PERIOD_TIME_REG	0x8E
#define CH32_PERIOD_TIME_REG	0x8F

#define CH_1_2_TRIGGER_ORDER_REG	0x90
#define CH_3_4_TRIGGER_ORDER_REG	0x91
#define CH_5_6_TRIGGER_ORDER_REG	0x92
#define CH_7_8_TRIGGER_ORDER_REG	0x93
#define CH_9_10_TRIGGER_ORDER_REG	0x94
#define CH_11_11_TRIGGER_ORDER_REG	0x95
#define CH_13_14_TRIGGER_ORDER_REG	0x96
#define CH_15_16_TRIGGER_ORDER_REG	0x97
#define CH_17_18_TRIGGER_ORDER_REG	0x98
#define CH_19_20_TRIGGER_ORDER_REG	0x99
#define CH_21_22_TRIGGER_ORDER_REG	0x9A
#define CH_23_24_TRIGGER_ORDER_REG	0x9B
#define CH_25_26_TRIGGER_ORDER_REG	0x9C
#define CH_27_28_TRIGGER_ORDER_REG	0x9D
#define CH_29_30_TRIGGER_ORDER_REG	0x9E
#define CH_31_32_TRIGGER_ORDER_REG	0x9F

#ifdef __cplusplus
}
#endif
#endif /* SRC_M_SERIAL_FPGA_PROC_H_ */
