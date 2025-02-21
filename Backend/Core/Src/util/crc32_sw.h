/*
 * crc32_sw.h
 *
 *  Created on: Jul 15, 2024
 *      Author: catsa
 */

#ifndef SRC_UTIL_CRC32_SW_H_
#define SRC_UTIL_CRC32_SW_H_
/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
uint32_t crc32_ieee(const uint8_t *data, size_t len);
/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif
#endif /* SRC_UTIL_CRC32_SW_H_ */
