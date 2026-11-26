/*
 * lcd.h
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

void lcd_init(void);
void lcd_put_cur(int row, int col);
void lcd_send_string(char *str);
void lcd_clear(void);

void LCD_Update_Traffic_Info(uint8_t mode, int time1, int time2);
void LCD_Show_Tuning_Drafts(uint8_t active_mode, int r, int a, int g);
void LCD_Show_Tuning_Step_Confirmed(uint8_t mode, int val);
void LCD_Show_Tuning_Error(const char *reason, uint8_t remaining_sec);
void LCD_Show_Tuning_Saved(uint8_t r, uint8_t g, uint8_t a);
void LCD_Update_Environment(void);

#endif /* INC_LCD_H_ */
