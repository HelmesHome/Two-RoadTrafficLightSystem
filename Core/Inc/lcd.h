/*
 * i2c-lcd.h
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include "main.h"
// Hàm khởi tạo màn hình LCD
void lcd_init(void);

// Hàm di chuyển con trỏ tới vị trí (row: 0-1, col: 0-15)
void lcd_put_cur(int row, int col);

// Hàm gửi một chuỗi ký tự ra màn hình
void lcd_send_string(char *str);

// Hàm xóa toàn bộ màn hình
void lcd_clear(void);


void LCD_Update_Traffic_Info(uint8_t mode, int time1, int time2);
#endif /* INC_LCD_H_ */
