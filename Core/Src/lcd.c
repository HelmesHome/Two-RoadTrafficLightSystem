/*
 * i2c-lcd.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */
#include <lcd.h>
#include"main.h"
#include <stdio.h>
extern I2C_HandleTypeDef hi2c1;  // Lấy biến I2C1 từ file main.c


#define SLAVE_ADDRESS_LCD (0x21)

void lcd_send_cmd (char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C;  // en=1, rs=0
    data_t[1] = data_u | 0x08;  // en=0, rs=0
    data_t[2] = data_l | 0x0C;  // en=1, rs=0
    data_t[3] = data_l | 0x08;  // en=0, rs=0
    HAL_I2C_Master_Transmit(&hi2c1, (SLAVE_ADDRESS_LCD << 1), (uint8_t *) data_t, 4, 100);
}

void lcd_send_data (char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D;  // en=1, rs=1
    data_t[1] = data_u | 0x09;  // en=0, rs=1
    data_t[2] = data_l | 0x0D;  // en=1, rs=1
    data_t[3] = data_l | 0x09;  // en=0, rs=1
    HAL_I2C_Master_Transmit(&hi2c1, (SLAVE_ADDRESS_LCD << 1), (uint8_t *) data_t, 4, 100);
}

void lcd_clear (void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_put_cur(int row, int col) {
    switch (row) {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }
    lcd_send_cmd(col);
}

void lcd_init (void) {
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

}

void lcd_send_string (char *str) {
    while (*str) lcd_send_data(*str++);
}

void LCD_Update_Traffic_Info(uint8_t mode, int time1, int time2) {
	char buffer[17];

	lcd_put_cur(0, 0);

	if (mode == 1) {
		sprintf(buffer, "AUTO T1:%02d T2:%02d", time1, time2);
	}
	else if (mode == 2) {
		if (time1 == 1)      sprintf(buffer, "MANUAL: RED-GRN ");
		else if (time1 == 2) sprintf(buffer, "MANUAL: RED-AMB ");
		else if (time1 == 3) sprintf(buffer, "MANUAL: GRN-RED ");
		else if (time1 == 4) sprintf(buffer, "MANUAL: AMB-RED ");
	}
	else if (mode == 3) {
		sprintf(buffer, "TUNE RED: %02d   ", time1);
	}
	else if (mode == 4) {
		sprintf(buffer, "TUNE YELLOW: %02d ", time1);
	}
	else if (mode == 5) {
		sprintf(buffer, "TUNE GREEN: %02d ", time1);
	}
	lcd_send_string(buffer);
}
















