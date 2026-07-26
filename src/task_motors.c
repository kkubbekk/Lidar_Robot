#define STACK_SIZE 1024
#define PRIORITY 4
#define SAMPLE_PERIOD_MS 10
#define WHEEL_BASE_MM 400
#define WHEEL_RADIUS_MM 65
#define PWM_PERIOD_NS 100000
#define KP 0.15f
#define KI 0.03f
#define KD 0.00f

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include <math.h>
#include "shared_data.h"
#include "task_motors.h"


#ifndef M_PI
#define M_PI 3.14159265358979f
#endif

//thread definition
void motor_task(void *arg1, void *arg2, void *arg3);

K_THREAD_DEFINE(motor_thread_id, STACK_SIZE, motor_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// struct k_thread motor_thread_data;

// k_tid_t motor_thread_id = k_thread_create(&motor_thread_data, motor_thread_stack, STACK_SIZE, motor_task, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

//filtr
static float filtered_v_left = 0.0f;
static float filtered_v_right = 0.0f;

//robot status
extern atomic_t robot_state;

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

//queue definition
K_MSGQ_DEFINE(motor_msg, sizeof(motor_sample_t), 5, 4);

void set_velocity_motor(int32_t target_velocity_ignoruj_to, struct gpio_dt_spec* in1, struct gpio_dt_spec* in2, const struct pwm_dt_spec* pwm, int32_t pwm_pid_value)
{
    
    if(pwm_pid_value > 0) { 
        gpio_pin_set_dt(in1, 1);
        gpio_pin_set_dt(in2, 0);
    }
    else if(pwm_pid_value < 0) { 
        gpio_pin_set_dt(in1, 0);
        gpio_pin_set_dt(in2, 1);
    }
    else {
        gpio_pin_set_dt(in1, 0);
        gpio_pin_set_dt(in2, 0);
    }
    
   
    int32_t duty_cycle = abs(pwm_pid_value) * PWM_PERIOD_NS / 100;
    pwm_set_dt(pwm, PWM_PERIOD_NS, duty_cycle);
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

float calculate_pid_and_set_pwm(float v_now,float v_target,pid_state_t* pid_state,int64_t dt_ms)
{
    float error = v_target-v_now;
    float dt_s = dt_ms/1000.0f;
    //kp
    float k_p = KP*error;

    //ki
    pid_state->integral += error *dt_s;
    
    if(pid_state->integral > 2000.0f) pid_state->integral = 2000.0f;
    if(pid_state->integral < -2000.0f) pid_state->integral = -2000.0f;

    float k_i = KI*pid_state->integral;

    //kd
    float k_d = (dt_s > 0.0f) ? (error - pid_state->prev_error) * KD / dt_s : 0.0f;

    pid_state->prev_error = error;
    float pwm_percent = k_p + k_i + k_d; // BEZ fabs()!

    if(pwm_percent > 100.0f) pwm_percent = 100.0f;
    if(pwm_percent < -100.0f) pwm_percent = -100.0f;

    return pwm_percent;
}


//TASK
void motor_task(void *arg1, void *arg2, void *arg3) 
{

    motor_sample_t motor_data ={0};

    //testy
    static pid_state_t pid_left = {0};
    static pid_state_t pid_right = {0};

   
    //strucy na dane z enkoderow
    struct sensor_value left_position_actual;
    struct sensor_value right_position_actual;

    struct sensor_value left_position_last;
    struct sensor_value right_position_last;

    k_timer_start(&motor_sample_timer, K_MSEC(SAMPLE_PERIOD_MS), K_MSEC(SAMPLE_PERIOD_MS));

    //  pwm_set_dt(&pwma, 100000, 70000); // na testy
    //  pwm_set_dt(&pwmb, 100000, 70000); // na testy

     int64_t last_time_ms = k_uptime_get();

     // Przed pętlą while(1) - odczyt startowy, żeby nie mieć skoku od 0
    
    //zczytanie zeby uniknac skoku od 0
    sensor_sample_fetch(encoder_left);
    sensor_channel_get(encoder_left, SENSOR_CHAN_ROTATION, &left_position_actual);
    sensor_sample_fetch(encoder_right);
    sensor_channel_get(encoder_right, SENSOR_CHAN_ROTATION, &right_position_actual);


while (1) {

        robot_state_t current_state = (robot_state_t)atomic_get(&robot_state);
        
        if(current_state == STATE_TIPPED || current_state == STATE_SENSOR_ERROR || current_state == STATE_MOTOR_FAULT)
        {
            set_velocity_motor(0,&ain1,&ain2,&pwma,0);
            set_velocity_motor(0,&bin1,&bin2,&pwmb,0);
            k_timer_status_sync(&motor_sample_timer);
            continue;
        }

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

        int64_t now_ms = k_uptime_get();
        
        int64_t dt_ms = (now_ms-last_time_ms);

        last_time_ms = now_ms;

        int32_t delta_left  = unwrap_delta_millideg(&left_position_actual, &left_position_last);
        int32_t delta_right = unwrap_delta_millideg(&right_position_actual, &right_position_last);

        //jnesli kola robota stoja 
        //i dojdzie tu if jesli zadana predkosc jest wieksza niz zero ale to juz po implementacji rosowej
        // if(abs(delta_right)<50 || abs(delta_left)<50)
        // {
        //     atomic_set(&robot_state,STATE_MOTOR_FAULT);
        // }

       float raw_v_left = calculate_linear_speed(delta_left, dt_ms);
       float raw_v_right = calculate_linear_speed(delta_right, dt_ms);

    //    filtered_v_left  = (0.2f * raw_v_left)  + (0.8f * filtered_v_left);
    //    filtered_v_right = (0.2f * raw_v_right) + (0.8f * filtered_v_right);

        float target_v_left = -150.5f;   // testowo, na sztywno
        float target_v_right = 200.5f;  // testowo, na sztywno

   
        float pwm_left  = calculate_pid_and_set_pwm(raw_v_left, target_v_left, &pid_left, dt_ms);
        float pwm_right  = calculate_pid_and_set_pwm(raw_v_right, target_v_right, &pid_right, dt_ms);

        set_velocity_motor((int32_t)(target_v_left*1000), &ain1, &ain2, &pwma, (int32_t)pwm_left);
        set_velocity_motor((int32_t)(target_v_right*1000), &bin1, &bin2, &pwmb, (int32_t)pwm_right);

        motor_data.v_left_mps = filtered_v_left;
    motor_data.v_right_mps = filtered_v_right;

        printk("L: now=%.2f target=%.2f pwm=%.0f | R: now=%.2f target=%.2f pwm=%.0f\n",
    (double)motor_data.v_left_mps, (double)target_v_left, (double)pwm_left,
    (double)motor_data.v_right_mps, (double)target_v_right, (double)pwm_right);

    printk("state: %d\n", current_state);

        motor_data.acumulated_left_encoder_ticks  += delta_left;
        motor_data.acumulated_right_encoder_ticks += delta_right;

        motor_data.motor_angular_speed = calculate_angular_speed(motor_data.v_left_mps,motor_data.v_right_mps,dt_ms);

        motor_data.robot_speed_mps = calculate_robot_speed(motor_data.v_left_mps,motor_data.v_right_mps);


        if(k_msgq_put(&motor_msg,&motor_data,K_NO_WAIT)==0)
        {
            //todo
        }
        else
        {
            //todo
        }

      

    // printk("v_l: %f | v_r: %f | ang_v: %f | v_lin: %f | acum_left: %lld | acum_right: %lld |\n",
    //    motor_data.v_left_mps, motor_data.v_right_mps, motor_data.motor_angular_speed,
    //    motor_data.robot_speed_mps, motor_data.acumulated_left_encoder_ticks, motor_data.acumulated_right_encoder_ticks);
       
        
        k_timer_status_sync(&motor_sample_timer);
        
        //TODO: dodac obliczenia predkosci acummutudted counta zaimplentowac pid oraz heading z odometrii(jeszcze imu dojdzie)
}
    }
