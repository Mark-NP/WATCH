
#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ssd1306.h"

#define I2C_PORT       0
#define OLED_SDA       21
#define OLED_SCL       22

#define OLED_ADDRESS   0x3C
#define OLED_WIDTH     128
#define OLED_HEIGHT    64

static const char *TAG = "OLED";

void app_main(void)
{
    // 1. Create I2C bus
    i2c_master_bus_handle_t i2c_bus = NULL;

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .sda_io_num = OLED_SDA,
        .scl_io_num = OLED_SCL,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(
        i2c_new_master_bus(&bus_config, &i2c_bus)
    );

    // 2. Configure I2C communication with OLED
    esp_lcd_panel_io_handle_t io_handle = NULL;

    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = OLED_ADDRESS,
        .scl_speed_hz = 100000,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6,
    };

    ESP_ERROR_CHECK(
        esp_lcd_new_panel_io_i2c(
            i2c_bus,
            &io_config,
            &io_handle
        )
    );

    // 3. Configure SSD1306
    esp_lcd_panel_handle_t panel_handle = NULL;

    esp_lcd_panel_ssd1306_config_t ssd_config = {
        .height = OLED_HEIGHT,
    };

    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
        .vendor_config = &ssd_config,
    };

    ESP_ERROR_CHECK(
        esp_lcd_new_panel_ssd1306(
            io_handle,
            &panel_config,
            &panel_handle
        )
    );

    // 4. Initialize and turn on OLED
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG, "SSD1306 initialized successfully!");
}
