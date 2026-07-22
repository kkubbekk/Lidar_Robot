#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>

//aliasy
 static const struct gpio_dt_spec ain1 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_ain1), gpios);
 static const struct gpio_dt_spec ain2 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_ain2), gpios);
 static const struct gpio_dt_spec bin1 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_bin1), gpios);
 static const struct gpio_dt_spec bin2 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_bin2), gpios);

 const struct pwm_dt_spec pwma = PWM_DT_SPEC_GET(DT_ALIAS(motor_pwma));
 const struct pwm_dt_spec pwmb = PWM_DT_SPEC_GET(DT_ALIAS(motor_pwmb));
 const struct device *const encoder_left = DEVICE_DT_GET(DT_ALIAS(encoder_left));
 const struct device *const encoder_right = DEVICE_DT_GET(DT_ALIAS(encoder_right));



int main(void)
{
    if (!gpio_is_ready_dt(&ain1) || !gpio_is_ready_dt(&ain2) || !pwm_is_ready_dt(&pwma) || !device_is_ready(encoder_left) || !device_is_ready(encoder_right) || !gpio_is_ready_dt(&bin1) || !gpio_is_ready_dt(&bin2) || !pwm_is_ready_dt(&pwmb)) {
        printk("BLAD: Sprzet nie jest gotowy!\n");
        return 0;
    }

    gpio_pin_configure_dt(&ain1, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&ain2, GPIO_OUTPUT_INACTIVE);


    gpio_pin_configure_dt(&bin1, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&bin2, GPIO_OUTPUT_INACTIVE);

   

    printk("Main konczy prace, oddaje sterowanie do taskow!\n");

    return 0;
}