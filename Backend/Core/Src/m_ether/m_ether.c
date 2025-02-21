/*
 * m_ether.c
 *
 *  Created on: Feb 9, 2024
 *      Author: Robyn
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

#include "m_env.h"
#include "m_ether.h"
#include "m_serial.h"
#include "m_remote.h"
#include "app_config_flash.h"
#include "Ethernet.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct _mac_ip_addr
{
	char id[2];
	ip_net_t ip_net;
}mac_ip_addr;

typedef enum{
	eREPLY_OK,
	eREPLY_IP
}eReply_t;

static struct{
	remote_mode_t run_mode;
}m_cfg={
	.run_mode = eREMOTE_NONE,
};
/* Private define ------------------------------------------------------------*/
#define  DEC2BCD(v) (((v/10)<<4) + (v%10));
#define  BCD2DEC(v) ((v>>4)*10 + (v&0x0F));

#define AT24EEP_ADDR		0xA0
#define AT24MAC_ADDR		0xB0
#define EUI48_ADDR			0x9A
#define TCP_DATA_PAYLOAD	0x36

#define MAC_I2C_HANDLE	hi2c1

#define _SET_DEFAULT_IP_TEST	0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId ethernetTaskHandle;

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;

#ifdef _DHCP_
uint8_t dhcpsvrip[4] = { 168, 124, 101, 2 };
uint8_t dnsip[4] = { 0, 0, 0, 0 };
#endif

static uint8_t mymac[6] = { 0x54, 0x57, 0x51, 0x10, 0x05, 0x25 };
mac_ip_addr g_ip_net;

CCMRAM uint8_t eth_buf[ETH_BUFFER_SIZE+1];

uint8_t bResponseData[MAX_RESPONSE_LEN];
static event_queue_observer_t ether_event;

/* Private function prototypes -----------------------------------------------*/
void EthernetTask(void const * argument);

static int read_mac_addr(void)
{
	uint8_t mac_addr[7];

	if(m_env_e2p_read(&MAC_I2C_HANDLE, AT24MAC_ADDR, EUI48_ADDR, 1, mac_addr, 6))
	{
		LOG_ERR("Get MAC Error");
		return 1;
	}

	memcpy(mymac, &mac_addr[1],6);

	LOG_DBG("mac = %x:%x:%x:%x:%x:%x", mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], mac_addr[6]);

	return 0;
}

#if 0
#if 0
static int make_send_data(uint8_t *pPayLoad, eReply_t type, int error)
#else
static int make_send_data(uint8_t *pPayLoad)
#endif
{
	int offset = 0;
#if 0
	if(type == eREPLY_OK){
		if(error){
			sprintf((char *)&pPayLoad[offset], "ERROR\r\n");
			offset = 7;
		}else{
			sprintf((char *)&pPayLoad[offset], "OK\r\n");
			offset = 4;
		}
	}else if(type == eREPLY_IP){
		pPayLoad[offset] = 0xDF;
		offset+=1;

		memcpy(&pPayLoad[offset], g_ip_net.ip_net.ipaddr, 4);
		offset+=4;
		memcpy(&pPayLoad[offset], &g_ip_net.ip_net.port, 2);
		offset+=2;
		pPayLoad[offset] = 0xFD;
	}
#else
	pPayLoad[offset++] = 0x00;
	pPayLoad[offset++] = 0xFF;
	pPayLoad[offset++] = 0xFE;
#endif

	return offset;
}

static int make_reply_data(uint8_t *pPayLoad)
{
	uint16_t offset = MAX_RESPONSE_LEN;

	memcpy(pPayLoad, bResponseData, MAX_RESPONSE_LEN);

	return offset;
}
#endif

static int read_packet(void)
{
	int tmp_cnt;
	uint16_t dat_p;
	uint16_t offset;

	tmp_cnt = Ethernet_PacketReceive(ETH_BUFFER_SIZE, eth_buf);
	dat_p = Ethernet_packetloop_icmp_tcp(eth_buf, tmp_cnt);
	if (dat_p > 0){
		//LOG_HEX_DUMP(eth_buf, tmp_cnt, "Received Data >>");
#if 0
		static uint8_t recv_buf[50];

		memcpy(recv_buf, &eth_buf[TCP_DATA_PAYLOAD],tmp_cnt - dat_p);
		push_event0_param(EVT_received_tcp, recv_buf, tmp_cnt - dat_p);
		offset = make_reply_data(&eth_buf[TCP_DATA_PAYLOAD]);
#else
		//LOG_HEX_DUMP(&eth_buf[TCP_DATA_PAYLOAD], tmp_cnt - dat_p, "Received Data >>");
		offset = m_remote_eth_parser(&eth_buf[TCP_DATA_PAYLOAD], tmp_cnt - dat_p);
#endif
		if(offset <= 0){
			offset = tmp_cnt - dat_p;
		}

		Ethernet_wb_server_reply(eth_buf, offset);
	}

	return 0;
}

static int ethernet_init(void)
{
	uint8_t rev;

	Ethernet_Init(mymac);

	rev = Ethernet_Revision();
	if (rev == 0){
		// Failed to access ENC28J60
		//while (1);    // Just loop here
		LOG_ERR("Ethernet_Revision Error!!Rev[%d]", rev);
		return 1;
	}

	LOG_INF("Ethernet_Revision : %d", Ethernet_Revision());
	LOG_INF("mymac : %d:%d:%d:%d:%d:%d", mymac[0], mymac[1], mymac[2], mymac[3], mymac[4], mymac[5] );

#ifdef _DHCP_
	// Get IP Address details
	if (Ethernet_allocate_ip_address(eth_buf, ETH_BUFFER_SIZE, mymac, 80, g_ip_net.ip_net.ipaddr, g_ip_net.ip_net.submask, g_ip_net.ip_net.gateway, dhcpsvrip, dnsip) > 0){
		// Display the results:
		LOG_DBG("Success IP Resource binding");
	}else{
		// Failed to get IP address"
		LOG_ERR("Failed to get IP address");
		return 1;
	}
#else
	Ethernet_init_ip_arp_udp_tcp(mymac, g_ip_net.ip_net.ipaddr, g_ip_net.ip_net.port);
	Ethernet_client_set_gwip(g_ip_net.ip_net.gateway);
#endif

	LOG_DBG("myip = %d.%d.%d.%d \r\n", g_ip_net.ip_net.ipaddr[0],g_ip_net.ip_net.ipaddr[1],
									g_ip_net.ip_net.ipaddr[2],g_ip_net.ip_net.ipaddr[3]);

	return 0;
}

void ENC28J60_DelayUs(uint32_t delay) {
	do {
		asm volatile (	"MOV R0,%[loops]\n\t"\
				"1: \n\t"\
				"SUB R0, #1\n\t"\
				"CMP R0, #0\n\t"\
				"BNE 1b \n\t" : : [loops] "r" (20*delay) : "memory"\
			      ); // test logic analyzer, target 100us : 20->97us 21->127us
	} while(0);
}

void ENC28J60_DelayMs(uint32_t delay) {
	osDelay(delay);
}

uint32_t ENC28J60_GetMs(void) {
	return HAL_GetTick();
}

void ENC28J60_EnableChip(void) {
	HAL_GPIO_WritePin(ETH_NSS_GPIO_Port, ETH_NSS_Pin, GPIO_PIN_RESET);
}

void ENC28J60_DisableChip(void) {
	HAL_GPIO_WritePin(ETH_NSS_GPIO_Port, ETH_NSS_Pin, GPIO_PIN_SET);
}

uint8_t ENC28J60_TransceiveByte(uint8_t data) {
	uint8_t received;
	HAL_StatusTypeDef error;

	error = HAL_SPI_TransmitReceive(&hspi1, &data, &received, 1, 1000);
	if (error == HAL_OK) {
		return received;
	}

	LOG_ERR("Error[%x]", error);
	return 0;
}

int m_eth_write_mac_ipaddr(ip_net_t *ip_addr)
{
	uint8_t buffer[32]={0,}, size;
	uint16_t mem_address;

	mem_address = 0;
	size = sizeof(g_ip_net);
	g_ip_net.id[0] = 'Y';
	g_ip_net.id[1] = 'S';
	memcpy(&g_ip_net.ip_net, ip_addr, sizeof(ip_net_t));
	memcpy(buffer, &g_ip_net, sizeof(mac_ip_addr));
	LOG_HEX_DUMP(buffer, size, "Write E2p");
	if(m_env_e2p_write(&MAC_I2C_HANDLE, AT24EEP_ADDR, mem_address, 1, buffer, size)){
		LOG_ERR("Set IP Address from MacIC Error");
		return 1;
	}

	Ethernet_init_ip_arp_udp_tcp(mymac, g_ip_net.ip_net.ipaddr, g_ip_net.ip_net.port);
	Ethernet_client_set_gwip(g_ip_net.ip_net.gateway);
	return 0;
}

static int read_ipnet(void)
{
	uint8_t buffer[32]={0,}, size;
	uint16_t mem_address;

	mem_address = 0;
	size = sizeof(mac_ip_addr);
	if(m_env_e2p_read(&MAC_I2C_HANDLE, AT24EEP_ADDR, mem_address, 1, buffer, size)){
		LOG_ERR("Get IP Address from MacIC Error\r\n");
		return 1;
	}
	LOG_HEX_DUMP(buffer, size, "Read E2p");
	memcpy(&g_ip_net, buffer, sizeof(mac_ip_addr));
	if(memcmp(g_ip_net.id, "YS", 2)==0){
		LOG_INF("Read Size [%d] IP Addr [%03d.%03d.%03d.%03d] Port[%d]\r\n", size, g_ip_net.ip_net.ipaddr[0], g_ip_net.ip_net.ipaddr[1],
							g_ip_net.ip_net.ipaddr[2], g_ip_net.ip_net.ipaddr[3], g_ip_net.ip_net.port);
#if _SET_DEFAULT_IP_TEST
		return 1;
#endif
		return 0;
	}else{
		LOG_ERR("Not found IP Address from MacIC!!\r\n");
		return 1;
	}

	return 0;
}

static void onWriteIPAddress(void *pData, uint32_t size)
{
	ip_net_t ipnet;
	int error;
	model_ip_net_t *Data = pData;

	memcpy(ipnet.submask, Data->submask, sizeof(ipnet.submask));
	memcpy(ipnet.ipaddr, Data->ipaddr, sizeof(ipnet.ipaddr));

	ipnet.gateway[0] = Data->ipaddr[0];
	ipnet.gateway[1] = Data->ipaddr[1];
	ipnet.gateway[2] = 0;
	ipnet.gateway[3] = 1;

	ipnet.port = Data->port;


	LOG_DBG("IP Addr[%03d.%03d.%03d.%03d : %d]", ipnet.ipaddr[0], ipnet.ipaddr[1], ipnet.ipaddr[2], ipnet.ipaddr[3], ipnet.port)
	error = m_eth_write_mac_ipaddr(&ipnet);
	REPORT_IF_ERROR(error);
	ethernet_init();
}

static void On_set_mode(remote_mode_t mode)
{
	if(mode == eREMOTE_ETHER){
		LOG_DBG("Start Ether Control");
	}
	m_cfg.run_mode = mode;
}

void m_eth_set_trigger(uint8_t *response)
{
	memcpy(bResponseData, response, MAX_RESPONSE_LEN);
}

int m_eth_read_mac_ipaddr(ip_net_t *ip_addr)
{
	memcpy(ip_addr, &g_ip_net.ip_net, sizeof(ip_net_t));

	return 0;
}

void EthernetTask(void const * argument)
{
	LOG_DBG("Start Ethernet task");

	while(1){
		if(m_cfg.run_mode == eREMOTE_ETHER){
			read_packet();
			osDelay(10);
		}else{
			osDelay(1000);
		}
	}
}

static void evt_handler(event_t const* evt, void* p_context)
{
	switch (evt->event)
	{
		case EVT_Set_ip:
			onWriteIPAddress(evt->p_event_data, evt->event_data_size);
			break;
		case EVT_Get_ip:
			break;
		case EVT_remote_mode:
			On_set_mode(evt->param1);
			break;
		default:
			break;
	}
}

int EthernetInit(void)
{
	int time_out = 0, error;
	ip_net_t default_ip;

	LOG_DBG("%s Init", __func__);

	osThreadDef(ethernetTask, EthernetTask, osPriorityNormal, 0, 2048);
	ethernetTaskHandle = osThreadCreate(osThread(ethernetTask), NULL);
	if(ethernetTaskHandle == NULL){
		LOG_ERR("Ethernet task Create Error");
		return -1;
	}
	while(time_out++ < 10){
		if(read_mac_addr() == 0)
			break;
		//HAL_Delay(10);
		osDelay(10); // ??
	}

	if(read_ipnet()){
		// default ip address
		default_ip.ipaddr[0] = 192;
		default_ip.ipaddr[1] = 168;
		default_ip.ipaddr[2] = 0;
		default_ip.ipaddr[3] = 5;
		default_ip.port = 5050;

		default_ip.gateway[0] = 192;
		default_ip.gateway[1] = 168;
		default_ip.gateway[2] = 0;
		default_ip.gateway[3] = 1;

		LOG_INF("Set Default IP Addr [%03d.%03d.%03d.%03d] Port[%d]\r\n", default_ip.ipaddr[0], default_ip.ipaddr[1],
										default_ip.ipaddr[2], g_ip_net.ip_net.ipaddr[3], default_ip.port);
		m_eth_write_mac_ipaddr(&default_ip);
	}

	error = ethernet_init();
	RETURN_IF_ERROR(error);

	ether_event.handler = evt_handler;
	ether_event.p_context = NULL;
	error = m_event_register(&ether_event);
	RETURN_IF_ERROR(error);

	return 0;
}
