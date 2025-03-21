/*
 * m_front.h
 *
 *  Created on: Feb 12, 2025
 *      Author: catsa
 */

#ifndef SRC_M_SERIAL_M_FRONT_H_
#define SRC_M_SERIAL_M_FRONT_H_

void front_push_buf(uint8_t ch);
uint8_t m_front_init(void);
void m_front_update_trigger_status(uint8_t trigger);
void m_front_response(uint8_t *data);
#endif /* SRC_M_SERIAL_M_FRONT_H_ */
