/*
 * lcd.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#include "lcd.h"
#include "dht20.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define SLAVE_ADDRESS_LCD   (0x21)
#define LCD_I2C_TIMEOUT_MS  10
#define LCD_RETRY_BACKOFF_MS 500
#define LCD_COLS            16
#define LCD_ROWS            2

static char prev_rows[LCD_ROWS][LCD_COLS + 1] = { "", "" };
static uint8_t lcd_i2c_backoff = 0;
static uint32_t lcd_backoff_until_ms = 0;

static HAL_StatusTypeDef lcd_i2c_write_4(uint8_t *data_t) {
    return HAL_I2C_Master_Transmit(&hi2c1, (SLAVE_ADDRESS_LCD << 1), data_t, 4, LCD_I2C_TIMEOUT_MS);
}

static HAL_StatusTypeDef lcd_send_cmd(char cmd) {
    char data_u = (cmd & 0xf0);
    char data_l = ((cmd << 4) & 0xf0);
    uint8_t data_t[4];
    data_t[0] = data_u | 0x0C;  // en=1, rs=0
    data_t[1] = data_u | 0x08;  // en=0, rs=0
    data_t[2] = data_l | 0x0C;  // en=1, rs=0
    data_t[3] = data_l | 0x08;  // en=0, rs=0
    return lcd_i2c_write_4(data_t);
}

static HAL_StatusTypeDef lcd_send_data(char data) {
    char data_u = (data & 0xf0);
    char data_l = ((data << 4) & 0xf0);
    uint8_t data_t[4];
    data_t[0] = data_u | 0x0D;  // en=1, rs=1
    data_t[1] = data_u | 0x09;  // en=0, rs=1
    data_t[2] = data_l | 0x0D;  // en=1, rs=1
    data_t[3] = data_l | 0x09;  // en=0, rs=1
    return lcd_i2c_write_4(data_t);
}

static int clamp_int(int val, int min_val, int max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

static void lcd_write_row_padded(uint8_t row, const char *text) {
    if (row >= LCD_ROWS || text == NULL) {
        return;
    }

    char padded[LCD_COLS + 1];
    uint8_t i = 0;
    while (i < LCD_COLS && text[i] != '\0') {
        padded[i] = text[i];
        i++;
    }
    while (i < LCD_COLS) {
        padded[i++] = ' ';
    }
    padded[LCD_COLS] = '\0';

    if (strcmp(padded, prev_rows[row]) == 0) {
        return;
    }

    uint32_t now = HAL_GetTick();
    if (lcd_i2c_backoff && (int32_t)(now - lcd_backoff_until_ms) < 0) {
        return;
    }

    uint8_t cmd = (row == 0) ? 0x80 : 0xC0;
    if (lcd_send_cmd((char)cmd) != HAL_OK) {
        lcd_i2c_backoff = 1;
        lcd_backoff_until_ms = now + LCD_RETRY_BACKOFF_MS;
        return;
    }

    for (uint8_t col = 0; col < LCD_COLS; col++) {
        if (lcd_send_data(padded[col]) != HAL_OK) {
            lcd_i2c_backoff = 1;
            lcd_backoff_until_ms = now + LCD_RETRY_BACKOFF_MS;
            return;
        }
    }

    lcd_i2c_backoff = 0;
    memcpy(prev_rows[row], padded, sizeof(padded));
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
    prev_rows[0][0] = '\0';
    prev_rows[1][0] = '\0';
}

void lcd_put_cur(int row, int col) {
    if (row < 0 || row >= LCD_ROWS || col < 0 || col >= LCD_COLS) {
        return;
    }
    uint8_t addr = (row == 0) ? (uint8_t)(0x80 | col) : (uint8_t)(0xC0 | col);
    lcd_send_cmd((char)addr);
}

void lcd_init(void) {
    HAL_Delay(50);
    lcd_send_cmd(0x30); HAL_Delay(5);
    lcd_send_cmd(0x30); HAL_Delay(1);
    lcd_send_cmd(0x30); HAL_Delay(10);
    lcd_send_cmd(0x20); HAL_Delay(10);

    lcd_send_cmd(0x28); HAL_Delay(1);
    lcd_send_cmd(0x08); HAL_Delay(1);
    lcd_send_cmd(0x01); HAL_Delay(2);
    lcd_send_cmd(0x06); HAL_Delay(1);
    lcd_send_cmd(0x0C); HAL_Delay(1);

    prev_rows[0][0] = '\0';
    prev_rows[1][0] = '\0';
}

void lcd_send_string(char *str) {
    if (str == NULL) {
        return;
    }
    for (uint8_t i = 0; i < LCD_COLS && str[i] != '\0'; i++) {
        if (lcd_send_data(str[i]) != HAL_OK) {
            break;
        }
    }
}

void LCD_Update_Traffic_Info(uint8_t mode, int time1, int time2) {
    char buffer[24];
    int t1 = clamp_int(time1, 0, 99);
    int t2 = clamp_int(time2, 0, 99);

    if (mode == 1) {
        snprintf(buffer, sizeof(buffer), "AUTO T1:%02d T2:%02d", t1, t2);
    } else if (mode == 2) {
        switch (time1) {
            case 1: snprintf(buffer, sizeof(buffer), "MANUAL: RED-GRN"); break;
            case 2: snprintf(buffer, sizeof(buffer), "MANUAL: RED-AMB"); break;
            case 3: snprintf(buffer, sizeof(buffer), "MANUAL: GRN-RED"); break;
            case 4: snprintf(buffer, sizeof(buffer), "MANUAL: AMB-RED"); break;
            default: snprintf(buffer, sizeof(buffer), "MANUAL: ERR ST"); break;
        }
    } else if (mode == 3) {
        snprintf(buffer, sizeof(buffer), "TUNE RED:    %02d", t1);
    } else if (mode == 4) {
        snprintf(buffer, sizeof(buffer), "TUNE YELLOW: %02d", t1);
    } else if (mode == 5) {
        snprintf(buffer, sizeof(buffer), "TUNE GREEN:  %02d", t1);
    } else if (mode == 6) {
        if (time1 == 0) {
            snprintf(buffer, sizeof(buffer), "NIGHT: BLINK AMB");
        } else {
            snprintf(buffer, sizeof(buffer), "WARN: ALL RED");
        }
    } else {
        snprintf(buffer, sizeof(buffer), "ERR: UNKNOWN MD");
    }

    lcd_write_row_padded(0, buffer);
}

void LCD_Show_Tuning_Drafts(uint8_t active_mode, int r, int a, int g) {
    char row0[24];
    int rr = clamp_int(r, 0, 99);
    int aa = clamp_int(a, 0, 99);
    int gg = clamp_int(g, 0, 99);

    if (active_mode == 3) {
        snprintf(row0, sizeof(row0), ">R%02d< A%02d G%02d", rr, aa, gg);
    } else if (active_mode == 4) {
        snprintf(row0, sizeof(row0), "R%02d >A%02d< G%02d", rr, aa, gg);
    } else if (active_mode == 5) {
        snprintf(row0, sizeof(row0), "R%02d A%02d >G%02d<", rr, aa, gg);
    } else {
        snprintf(row0, sizeof(row0), "ERR: UNKNOWN MD");
    }
    lcd_write_row_padded(0, row0);
}

void LCD_Show_Tuning_Step_Confirmed(uint8_t mode, int val) {
    char row0[24];
    int v = clamp_int(val, 0, 99);
    if (mode == 3) {
        snprintf(row0, sizeof(row0), "SET RED:     %02d", v);
    } else if (mode == 4) {
        snprintf(row0, sizeof(row0), "SET YELLOW:  %02d", v);
    } else {
        snprintf(row0, sizeof(row0), "SET GREEN:   %02d", v);
    }
    lcd_write_row_padded(0, row0);
}

void LCD_Show_Tuning_Error(const char *reason, uint8_t remaining_sec) {
    char row1[24];
    uint8_t sec = (remaining_sec > 99) ? 99 : remaining_sec;
    lcd_write_row_padded(0, (reason != NULL) ? reason : "ERR: INVALID");
    snprintf(row1, sizeof(row1), "FIX:%02us B1:EDIT", (unsigned int)sec);
    lcd_write_row_padded(1, row1);
}

void LCD_Show_Tuning_Saved(uint8_t r, uint8_t g, uint8_t a) {
    char row0[24];
    uint8_t rr = (r > 99) ? 99 : r;
    uint8_t gg = (g > 99) ? 99 : g;
    uint8_t aa = (a > 99) ? 99 : a;
    snprintf(row0, sizeof(row0), "SAVED R%02uG%02uA%02u", (unsigned int)rr, (unsigned int)gg, (unsigned int)aa);
    lcd_write_row_padded(0, row0);
}

void LCD_Update_Environment(void) {
    char row1[24];
    if (!DHT20_Data.valid) {
        lcd_write_row_padded(1, "T:--.-C H:--.-%");
        return;
    }

    float temp = DHT20_Data.Temperature;
    float hum = DHT20_Data.Humidity;
    if (temp < -40.0f) temp = -40.0f;
    if (temp > 99.9f) temp = 99.9f;
    if (hum < 0.0f) hum = 0.0f;
    if (hum > 100.0f) hum = 100.0f;

    int32_t t10 = (int32_t)(temp * 10.0f + ((temp >= 0.0f) ? 0.5f : -0.5f));
    int32_t h10 = (int32_t)(hum * 10.0f + 0.5f);
    if (t10 < -400) t10 = -400;
    if (t10 > 999) t10 = 999;
    if (h10 < 0) h10 = 0;
    if (h10 > 1000) h10 = 1000;

    const char *sign = (t10 < 0) ? "-" : "";
    int32_t abs_t10 = (t10 < 0) ? -t10 : t10;

    snprintf(row1, sizeof(row1), "T:%s%ld.%ld H:%ld.%ld%%",
             sign,
             (long)(abs_t10 / 10), (long)(abs_t10 % 10),
             (long)(h10 / 10), (long)(h10 % 10));
    lcd_write_row_padded(1, row1);
}
