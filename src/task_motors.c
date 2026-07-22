#define STACK_SIZE 1024
#define PRIORITY 4
#define SAMPLE_PERIOD_MS 10

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include "shared_data.h"


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



void motor_task(void *arg1, void *arg2, void *arg3) {

    //ostatnie wartosci enkoderow 
    int64_t last_left_position = 0;
    int64_t last_right_position = 0;
   
    //strucy na dane z enkoderow
    struct sensor_value left_position;
    struct sensor_value right_position;


     pwm_set_dt(&pwma, 100000, 40000); // na testy
     pwm_set_dt(&pwmb, 100000, 40000); // na testy


while (1) {
        last_left_position = left_position.val1;
        last_right_position = right_position.val1;

        // ODCZYT LEWEGO
        int err_left = sensor_sample_fetch(encoder_left);
        if(err_left == 0) {
            sensor_channel_get(encoder_left, SENSOR_CHAN_ROTATION, &left_position);
        } else {
            printk("Blad odczytu LEWEGO enkodera\n");
        }

        // ODCZYT PRAWEGO
        int err_right = sensor_sample_fetch(encoder_right);
        if(err_right == 0) {
            sensor_channel_get(encoder_right, SENSOR_CHAN_ROTATION, &right_position);
        } else {
            printk("Blad odczytu PRAWEGO enkodera\n");
        }

        // na debug
        printk("Left Encoder: Val1: %d | Val2: %d\n", left_position.val1, left_position.val2);
        printk("Right Encoder: Val1: %d | Val2: %d\n", right_position.val1, right_position.val2);
        
        k_msleep(10);
        
        //TODO: dodac obliczenia predkosci acummutudted counta zaimplentowac pid oraz heading z odometrii(jeszcze imu dojdzie)
}
    }
