#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_gc9a01.h"
#include "stdlib.h"
#include "esp_lvgl_port.h"
#include "../ui/ui.h"
#include "../dial_power/dial_power.h"
#include "../nvs_data/nvs_data.h"
/* LCD size */
#define EXAMPLE_LCD_H_RES   (240)
#define EXAMPLE_LCD_V_RES   (240)

/* LCD settings */
#define EXAMPLE_LCD_SPI_NUM         (SPI2_HOST)
#define EXAMPLE_LCD_PIXEL_CLK_HZ    (40 * 1000 * 1000)
#define EXAMPLE_LCD_CMD_BITS        (8)
#define EXAMPLE_LCD_PARAM_BITS      (8)
#define EXAMPLE_LCD_COLOR_SPACE     (ESP_LCD_COLOR_SPACE_BGR)
#define EXAMPLE_LCD_BITS_PER_PIXEL  (16)
#define EXAMPLE_LCD_DRAW_BUFF_DOUBLE (1)
#define EXAMPLE_LCD_DRAW_BUFF_HEIGHT (50)

/* LCD pins */
#define EXAMPLE_LCD_GPIO_SCLK       (GPIO_NUM_12)
#define EXAMPLE_LCD_GPIO_MOSI       (GPIO_NUM_11)
#define EXAMPLE_LCD_GPIO_RST        (GPIO_NUM_9)
#define EXAMPLE_LCD_GPIO_DC         (GPIO_NUM_14)
#define EXAMPLE_LCD_GPIO_CS         (GPIO_NUM_10)
#define EXAMPLE_LCD_GPIO_BL         (GPIO_NUM_13)

#define LEDC_TIMER              LEDC_TIMER_1
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          EXAMPLE_LCD_GPIO_BL // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_4
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT // Set duty resolution to 10 bits
#define LEDC_DUTY               (512) // Set duty to 50%. (2 ** 10) * 50% = 512
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

void lvgl_display_init(void);
void display_init(void);
/**
 * @brief Value duty 0-100
 *
 */
void set_screen_light(uint8_t duty);
/**
 * @brief Value 0\1\2\3
 *
 */
void set_screen_rotation(uint8_t val);