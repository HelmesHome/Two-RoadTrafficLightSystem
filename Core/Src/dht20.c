/*
 * dht20.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#include "dht20.h"
#include "main.h"
#include "scheduler.h"
#include "uart_log.h"

extern I2C_HandleTypeDef hi2c1;

#define DHT20_ADDR          (0x38)
#define DHT20_I2C_TIMEOUT   20
#define DHT20_MAX_ERR_STALE 2

DHT20_Data_t DHT20_Data = { 0.0f, 0.0f, 0, 0 };

static uint8_t dht20_calc_crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ 0x31);
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

static void dht20_reset_reg(uint8_t reg) {
    uint8_t tx[3] = { reg, 0x00, 0x00 };
    uint8_t rx[3] = { 0, 0, 0 };
    if (HAL_I2C_Master_Transmit(&hi2c1, (DHT20_ADDR << 1), tx, 3, DHT20_I2C_TIMEOUT) != HAL_OK) {
        return;
    }
    HAL_Delay(5);
    if (HAL_I2C_Master_Receive(&hi2c1, (DHT20_ADDR << 1), rx, 3, DHT20_I2C_TIMEOUT) != HAL_OK) {
        return;
    }
    HAL_Delay(10);
    tx[0] = (uint8_t)(0xB0 | reg);
    tx[1] = rx[1];
    tx[2] = rx[2];
    HAL_I2C_Master_Transmit(&hi2c1, (DHT20_ADDR << 1), tx, 3, DHT20_I2C_TIMEOUT);
    HAL_Delay(10);
}

static void dht20_mark_error(const char *reason) {
    if (DHT20_Data.error_count < 255) {
        DHT20_Data.error_count++;
    }
    if (DHT20_Data.error_count == DHT20_MAX_ERR_STALE) {
        DHT20_Data.valid = 0;
        uart_log("[DHT20] Sensor stale (%s)\r\n", reason);
    }
}

void DHT20_Init(void) {
    uint8_t cmd = 0x71;
    uint8_t status = 0;
    HAL_Delay(20);

    if (HAL_I2C_Master_Transmit(&hi2c1, (DHT20_ADDR << 1), &cmd, 1, DHT20_I2C_TIMEOUT) != HAL_OK ||
        HAL_I2C_Master_Receive(&hi2c1, (DHT20_ADDR << 1), &status, 1, DHT20_I2C_TIMEOUT) != HAL_OK) {
        uart_log("[DHT20] Init probe failed\r\n");
        return;
    }

    if ((status & 0x18) != 0x18) {
        dht20_reset_reg(0x1B);
        dht20_reset_reg(0x1C);
        dht20_reset_reg(0x1E);
        uart_log("[DHT20] Calibration reset done (status=0x%02X)\r\n", status);
    } else {
        uart_log("[DHT20] Ready (status=0x%02X)\r\n", status);
    }
}

void Task_DHT20_Trigger(void) {
    uint8_t tx_data[3] = {0xAC, 0x33, 0x00};
    if (HAL_I2C_Master_Transmit(&hi2c1, (DHT20_ADDR << 1), tx_data, 3, DHT20_I2C_TIMEOUT) == HAL_OK) {
        SCH_Add_Task(Task_DHT20_Fetch_And_Print, 8, 0);
    } else {
        dht20_mark_error("I2C TX fail");
    }
}

void Task_DHT20_Fetch_And_Print(void) {
    uint8_t rx_data[7];
    if (HAL_I2C_Master_Receive(&hi2c1, (DHT20_ADDR << 1), rx_data, 7, DHT20_I2C_TIMEOUT) != HAL_OK) {
        dht20_mark_error("I2C RX fail");
        return;
    }

    if ((rx_data[0] & 0x80) != 0) {
        dht20_mark_error("busy");
        return;
    }

    if (dht20_calc_crc8(rx_data, 6) != rx_data[6]) {
        dht20_mark_error("CRC mismatch");
        return;
    }

    uint32_t raw_humidity = rx_data[1];
    raw_humidity = (raw_humidity << 8) | rx_data[2];
    raw_humidity = (raw_humidity << 8) | rx_data[3];
    raw_humidity = raw_humidity >> 4;
    float hum = (float)raw_humidity * 100.0f / 1048576.0f;
    if (hum < 0.0f) hum = 0.0f;
    if (hum > 100.0f) hum = 100.0f;

    uint32_t raw_temp = rx_data[3] & 0x0F;
    raw_temp = (raw_temp << 8) | rx_data[4];
    raw_temp = (raw_temp << 8) | rx_data[5];
    float temp = (float)raw_temp * 200.0f / 1048576.0f - 50.0f;

    DHT20_Data.Humidity = hum;
    DHT20_Data.Temperature = temp;
    DHT20_Data.valid = 1;
    DHT20_Data.error_count = 0;

    int32_t t10 = (int32_t)(temp * 10.0f + ((temp >= 0.0f) ? 0.5f : -0.5f));
    int32_t h10 = (int32_t)(hum * 10.0f + 0.5f);
    int32_t abs_t10 = (t10 < 0) ? -t10 : t10;
    uart_log("[DHT20] T=%s%ld.%ldC H=%ld.%ld%%\r\n",
             (t10 < 0) ? "-" : "",
             (long)(abs_t10 / 10), (long)(abs_t10 % 10),
             (long)(h10 / 10), (long)(h10 % 10));
}
