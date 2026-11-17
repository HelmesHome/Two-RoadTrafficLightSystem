/*
 * dht20.c
 *
 *  Created on: Nov 13, 2026
 *      Author: Admin
 */


#include <lcd.h> // Thêm thư viện LCD để in ra màn hình
#include "dht20.h"
#include "main.h"
#include "scheduler.h"
#include <stdio.h>   // Thêm thư viện này để dùng hàm sprintf

extern I2C_HandleTypeDef hi2c1;

#define DHT20_ADDR (0x38 << 1)

DHT20_Data_t DHT20_Data;

// Hàm này được cấu hình để chạy mỗi 2 giây: SCH_Add_Task(Task_DHT20_Trigger, 0, 200);
void Task_DHT20_Trigger(void) {
    uint8_t tx_data[3] = {0xAC, 0x33, 0x00};

    // Chỉ gửi lệnh đo lường, KHÔNG CHỜ
    HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, tx_data, 3, 100);

    // Hẹn Scheduler 80ms (8 ticks) sau tự động gọi hàm Fetch
    SCH_Add_Task(Task_DHT20_Fetch_And_Print, 8, 0);
}

// Hàm này tự động được Scheduler gọi 80ms sau khi Trigger
void Task_DHT20_Fetch_And_Print(void) {
    uint8_t rx_data[7];
    char buffer[16];

    // Đọc 7 byte data từ DHT20
    if (HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, rx_data, 7, 100) == HAL_OK) {

        // Kiểm tra bit trạng thái: Nếu bit 7 của byte 0 là 1 -> Cảm biến chưa đo xong
        if ((rx_data[0] & 0x80) == 0x80) {
            return; // Thoát ra, đợi chu kỳ 2 giây tiếp theo đo lại
        }

        // --- CÔNG THỨC DỊCH BIT GIẢI MÃ ĐỘ ẨM (20-bit) ---
        uint32_t raw_humidity = rx_data[1];
        raw_humidity = (raw_humidity << 8) | rx_data[2];
        raw_humidity = (raw_humidity << 8) | rx_data[3];
        raw_humidity = raw_humidity >> 4; // Bỏ đi 4 bit cuối (thuộc về nhiệt độ)

        DHT20_Data.Humidity = (float)raw_humidity * 100 / 1048576.0f; // 2^20 = 1048576

        // --- CÔNG THỨC DỊCH BIT GIẢI MÃ NHIỆT ĐỘ (20-bit) ---
        uint32_t raw_temp = rx_data[3] & 0x0F; // Lấy 4 bit cuối của byte 3
        raw_temp = (raw_temp << 8) | rx_data[4];
        raw_temp = (raw_temp << 8) | rx_data[5];

        DHT20_Data.Temperature = (float)raw_temp * 200 / 1048576.0f - 50.0f;

        // --- HIỂN THỊ RA DÒNG 2 CỦA MÀN HÌNH LCD ---
        lcd_put_cur(1, 0); // Định vị con trỏ ở dòng 2 (Row 1), Cột đầu tiên
        // Định dạng in ra 1 chữ số thập phân, ví dụ: T:38.1C H:42.7%
        sprintf(buffer, "T:%.1fC H:%.1f%%", DHT20_Data.Temperature, DHT20_Data.Humidity);
        lcd_send_string(buffer);
    }
}
