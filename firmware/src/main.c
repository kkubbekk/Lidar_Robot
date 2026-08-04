#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include "shared_data.h"

//aliasy
 const struct gpio_dt_spec ain1 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_ain1), gpios);
 const struct gpio_dt_spec ain2 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_ain2), gpios);
 const struct gpio_dt_spec bin1 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_bin1), gpios);
 const struct gpio_dt_spec bin2 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_bin2), gpios);

 const struct pwm_dt_spec pwma = PWM_DT_SPEC_GET(DT_ALIAS(motor_pwma));
 const struct pwm_dt_spec pwmb = PWM_DT_SPEC_GET(DT_ALIAS(motor_pwmb));
 const struct device *const encoder_left = DEVICE_DT_GET(DT_ALIAS(encoder_left));
 const struct device *const encoder_right = DEVICE_DT_GET(DT_ALIAS(encoder_right));
 const struct i2c_dt_spec imu = I2C_DT_SPEC_GET(DT_ALIAS(imu));

 atomic_t robot_state = ATOMIC_INIT(STATE_OKAY); // inicjalizujemy stan poczatkowy robotaja na ok;

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{
    if (!gpio_is_ready_dt(&ain1) || !gpio_is_ready_dt(&ain2) || !pwm_is_ready_dt(&pwma) || !device_is_ready(encoder_left) || !device_is_ready(encoder_right) || !i2c_is_ready_dt(&imu) || !gpio_is_ready_dt(&bin1) || !gpio_is_ready_dt(&bin2) || !pwm_is_ready_dt(&pwmb)) {
        // printk("BLAD: Sprzet nie jest gotowy!\n");

        
        return 0;
    }

    
    gpio_pin_configure_dt(&ain1, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&ain2, GPIO_OUTPUT_INACTIVE);


    gpio_pin_configure_dt(&bin1, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&bin2, GPIO_OUTPUT_INACTIVE);

   



    return 0;
}





void heartbeat_task(void *a1, void *a2, void *a3)
{
    if (!gpio_is_ready_dt(&led)) {
        for(;;) { k_msleep(1000); } // nawet tu nie dojdzie jesli hardfault wczesniej
    }
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    for (;;) {
        gpio_pin_toggle_dt(&led);
        k_msleep(300);
    }
}
K_THREAD_DEFINE(heartbeat_id, 512, heartbeat_task, NULL, NULL, NULL, 5, 0, 0);