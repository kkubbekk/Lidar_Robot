#define STACK_SIZE 1024
#define PRIORITY 4
#define SAMPLE_PERIOD_MS 10
#define WHEEL_BASE_MM 400
#define WHEEL_RADIUS_MM 65


#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include <math.h>
#include "shared_data.h"


#ifndef M_PI
#define M_PI 3.14159265358979f
#endif

//thread definition
void motor_task(void *arg1, void *arg2, void *arg3);

K_THREAD_DEFINE(motor_thread_id, STACK_SIZE, motor_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// struct k_thread motor_thread_data;

// k_tid_t motor_thread_id = k_thread_create(&motor_thread_data, motor_thread_stack, STACK_SIZE, motor_task, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

//encoder structs
extern const struct device *const encoder_left;
extern const struct device *const encoder_right;

//motor pwm struct

extern const struct pwm_dt_spec pwma;
extern const struct pwm_dt_spec pwmb;

//ain1,ain2,bin1,bin2
extern const struct gpio_dt_spec ain1;
extern const struct gpio_dt_spec ain2;
extern const struct gpio_dt_spec bin1;
extern const struct gpio_dt_spec bin2;
//TIMER  to better synchornize 
K_TIMER_DEFINE(motor_sample_timer,NULL,NULL);

void set_velocity_motor(int32_t target_velocity, struct gpio_dt_spec* in1, struct gpio_dt_spec* in2, const struct pwm_dt_spec* pwm)
{
 if(target_velocity >0)
 {
    gpio_pin_set_dt(in1, 1);
    gpio_pin_set_dt(in2, 0);
   
 }
 else if(target_velocity <0)
 {
    gpio_pin_set_dt(in1, 0);
    gpio_pin_set_dt(in2, 1);
   
 }
 else
 {
    gpio_pin_set_dt(in1, 0);
    gpio_pin_set_dt(in2, 0);
 }
    int32_t duty_cycle = target_velocity*100000/100; // Assuming target_velocity is in percentage (0-100)
    pwm_set_dt(pwm, 100000, abs(duty_cycle));
}

int32_t unwrap_delta_millideg(struct sensor_value* actual_val, struct sensor_value* last_val)
{
    int32_t current_pos = actual_val->val1 * 1000 + actual_val->val2 / 1000;
    int32_t last_pos = last_val->val1 * 1000 + last_val->val2 / 1000;

    int32_t delta_pos = current_pos - last_pos;

    if (delta_pos > 180000)  delta_pos -= 360000;
    if (delta_pos < -180000) delta_pos += 360000;

    return delta_pos;
}

float calculate_linear_speed(int32_t delta_millideg, int64_t time_period_ms)
{
    return (float)delta_millideg / 360000.0f * 1000.0f / time_period_ms * 2 * WHEEL_RADIUS_MM * M_PI;
}


float calculate_angular_speed(float v_left,float v_right,int64_t time_period)
{
    return(v_right-v_left)/WHEEL_BASE_MM;
}
float calculate_robot_speed(float v_left,float v_right)
{
    return (v_left+v_right)/2.0f;
}


//TASK
void motor_task(void *arg1, void *arg2, void *arg3) 
{

    motor_sample_t motor_data ={0};

   
    //strucy na dane z enkoderow
    struct sensor_value left_position_actual;
    struct sensor_value right_position_actual;

    struct sensor_value left_position_last;
    struct sensor_value right_position_last;

    k_timer_start(&motor_sample_timer, K_MSEC(SAMPLE_PERIOD_MS), K_MSEC(SAMPLE_PERIOD_MS));

     pwm_set_dt(&pwma, 100000, 70000); // na testy
     pwm_set_dt(&pwmb, 100000, 70000); // na testy

     int64_t last_time_us = k_uptime_get() *1000;

     // Przed pętlą while(1) - odczyt startowy, żeby nie mieć skoku od 0
    
    //zczytanie zeby uniknac skoku od 0
    sensor_sample_fetch(encoder_left);
    sensor_channel_get(encoder_left, SENSOR_CHAN_ROTATION, &left_position_actual);
    sensor_sample_fetch(encoder_right);
    sensor_channel_get(encoder_right, SENSOR_CHAN_ROTATION, &right_position_actual);


while (1) {

        
        

        left_position_last.val1 = left_position_actual.val1;
        left_position_last.val2 = left_position_actual.val2;
        
        right_position_last.val1 = right_position_actual.val1;
        right_position_last.val2 = right_position_actual.val2;

        // ODCZYT LEWEGO
        int err_left = sensor_sample_fetch(encoder_left);
        if(err_left == 0) {
            sensor_channel_get(encoder_left, SENSOR_CHAN_ROTATION, &left_position_actual);
        } else {
            printk("Blad odczytu LEWEGO enkodera\n");
        }

        // ODCZYT PRAWEGO
        int err_right = sensor_sample_fetch(encoder_right);
        if(err_right == 0) {
            sensor_channel_get(encoder_right, SENSOR_CHAN_ROTATION, &right_position_actual);
        } else {
            printk("Blad odczytu PRAWEGO enkodera\n");
        }
        

        // na debug
        printk("Left Encoder: Val1: %d | Val2: %d\n", left_position_actual.val1, left_position_actual.val2);
        printk("Right Encoder: Val1: %d | Val2: %d\n", right_position_actual.val1, right_position_actual.val2);

        int64_t now_us = k_uptime_get()*1000;
        
        int64_t dt_ms = (now_us-last_time_us)/1000;

        last_time_us = now_us;

        int32_t delta_left  = unwrap_delta_millideg(&left_position_actual, &left_position_last);
        int32_t delta_right = unwrap_delta_millideg(&right_position_actual, &right_position_last);

        motor_data.v_left_mps  = calculate_linear_speed(delta_left, dt_ms);
        motor_data.v_right_mps  = calculate_linear_speed(delta_right, dt_ms);


        motor_data.acumulated_left_encoder_ticks  += delta_left;
        motor_data.acumulated_right_encoder_ticks += delta_right;

        motor_data.motor_angular_speed = calculate_angular_speed(motor_data.v_left_mps,motor_data.v_right_mps,dt_ms);

        motor_data.robot_speed_mps = calculate_robot_speed(motor_data.v_left_mps,motor_data.v_right_mps);

      

    printk("v_l: %f | v_r: %f | ang_v: %f | v_lin: %f | acum_left: %lld | acum_right: %lld |\n",
       motor_data.v_left_mps, motor_data.v_right_mps, motor_data.motor_angular_speed,
       motor_data.robot_speed_mps, motor_data.acumulated_left_encoder_ticks, motor_data.acumulated_right_encoder_ticks);
       
        
        k_timer_status_sync(&motor_sample_timer);
        
        //TODO: dodac obliczenia predkosci acummutudted counta zaimplentowac pid oraz heading z odometrii(jeszcze imu dojdzie)
}
    }
