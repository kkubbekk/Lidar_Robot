#define STACK_SIZE 1024
#define PRIORITY 5
#define SAMPLE_PERIOD_MS 20


#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include "led.screen.h"

void task_screen(void *arg1,void *arg2,void *arg3);

K_THREAD_DEFINE(screen_thead_id, STACK_SIZE, task_screen, NULL, NULL, NULL, PRIORITY, 0, 0);

extern const struct i2c_dt_spec screen;

 uint8_t buffer[1024];

 void task_screen(void *arg1, void *arg2, void *arg3)
{
    /
    ssd1306_t my_display = {
        .dev_i2c = screen, 
        .buffer = {0}
    };

    // 2. Próba uruchomienia hardware'u
    if (!ssd1306_init(&my_display)) {
        return; 
    }

    ssd1306_clear(&my_display);
    ssd1306_draw_string(&my_display, 10, 10, "TEST OLED OK!");
    ssd1306_update(&my_display);

    
    for(;;)
    {
   
        ssd1306_draw_pixel(&my_display, 110, 10, true);
        ssd1306_update(&my_display);
        k_msleep(500);

        ssd1306_draw_pixel(&my_display, 110, 10, false);
        ssd1306_update(&my_display);
        k_msleep(500);
    }
}