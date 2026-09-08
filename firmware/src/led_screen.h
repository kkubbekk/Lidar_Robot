#ifndef LED_SCREEN_H
#define LED_SCREEN_H

#include <zephyr/drivers/i2c.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef struct {
    struct i2c_dt_spec dev_i2c;
    uint8_t buffer[1024];
} ssd1306_t;


bool ssd1306_init(ssd1306_t *display);
void ssd1306_clear(ssd1306_t *display);
void ssd1306_update(ssd1306_t *display);
void ssd1306_draw_pixel(ssd1306_t *display, int x, int y, bool color);
void ssd1306_draw_string(ssd1306_t *display, int x, int y, const char *str);
void ssd1306_draw_bitmap(ssd1306_t *display, int x, int y, int w, int h, const uint8_t *bitmap);
void ssd1306_fill_rect(ssd1306_t *display, int x, int y, int width, int height);

#endif