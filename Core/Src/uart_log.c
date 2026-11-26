/*
 * uart_log.c
 *
 * Lightweight event-driven UART logger over USART2 (115200 baud).
 */

#include "uart_log.h"
#include <stdarg.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;

#define UART_LOG_BUF_SIZE   96
#define UART_LOG_TIMEOUT_MS 10

void uart_log(const char *fmt, ...) {
    if (fmt == NULL) {
        return;
    }

    char buf[UART_LOG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len <= 0) {
        return;
    }
    if (len >= (int)sizeof(buf)) {
        len = (int)sizeof(buf) - 1;
    }

    HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, UART_LOG_TIMEOUT_MS);
}
