#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include "shared_data.h"

//aliasy


const struct pwm_dt_spec pwm_left_in1 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_left_in1));
const struct pwm_dt_spec pwm_left_in2 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_left_in2));
const struct pwm_dt_spec pwm_right_in1 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_right_in1));
const struct pwm_dt_spec pwm_right_in2 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_right_in2));

 const struct device *const encoder_left = DEVICE_DT_GET(DT_ALIAS(encoder_left));
 const struct device *const encoder_right = DEVICE_DT_GET(DT_ALIAS(encoder_right));
 const struct i2c_dt_spec imu = I2C_DT_SPEC_GET(DT_ALIAS(imu));

 atomic_t robot_state = ATOMIC_INIT(STATE_OKAY); // inicjalizujemy stan poczatkowy robotaja na ok;

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{
    if (!pwm_is_ready_dt(&pwm_left_in1) || 
    !pwm_is_ready_dt(&pwm_left_in2) || 
    !pwm_is_ready_dt(&pwm_right_in1) || 
    !pwm_is_ready_dt(&pwm_right_in2) || 
    !device_is_ready(encoder_left) || 
    !device_is_ready(encoder_right) || 
    !i2c_is_ready_dt(&imu)) {

        
        return 0;
    }

    
  
   



    return 0;
}




