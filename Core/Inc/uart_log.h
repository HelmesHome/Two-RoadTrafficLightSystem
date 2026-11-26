/*
 * uart_log.h
 *
 * Lightweight event-driven UART logger over USART2 (115200 baud).
 */

#ifndef INC_UART_LOG_H_
#define INC_UART_LOG_H_

#include "main.h"

void uart_log(const char *fmt, ...);

#endif /* INC_UART_LOG_H_ */
