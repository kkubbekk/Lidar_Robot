
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <time.h>
#include "shared_data.h"
#include <stm_custom_msgs/msg/stm_hardware_state.h>
#include <microros_transports.h> 

#include <rmw_microros/rmw_microros.h>

extern struct k_msgq imu_msg;
extern struct k_msgq motor_msg;

extern atomic_t robot_state;

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/float32_multi_array.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#define STACK_SIZE 2048
#define PRIORITY 4


K_MSGQ_DEFINE(ros_msg, sizeof(ros_data_t), 5, 4);



#include <rclc/executor.h>

rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;
rcl_publisher_t publisher;


rcl_subscription_t subscriber;
rcl_timer_t timer;

std_msgs__msg__Float32MultiArray msg_rx;

stm_custom_msgs__msg__StmHardwareState msg_tx;

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{

    static imu_sample_t last_imu_data = {0};
    static motor_sample_t last_moto_data = {0};

    k_msgq_get(&imu_msg, &last_imu_data, K_NO_WAIT);
    k_msgq_get(&motor_msg, &last_moto_data, K_NO_WAIT);

    msg_tx.v_left_mps = last_moto_data.v_left_mps;
    msg_tx.v_right_mps = last_moto_data.v_right_mps;
    msg_tx.accum_left_ticks = last_moto_data.acumulated_left_encoder_ticks;
    msg_tx.accum_right_ticks = last_moto_data.acumulated_right_encoder_ticks;
    msg_tx.motor_angular_speed = last_moto_data.motor_angular_speed;
    msg_tx.motor_linear_speed = last_moto_data.robot_speed_mps;
    
    msg_tx.imu_yaw = last_imu_data.yaw;
    msg_tx.imu_pitch = last_imu_data.pitch;
    msg_tx.imu_roll = last_imu_data.roll;
    
    msg_tx.robot_state = (uint8_t)atomic_get(&robot_state);
    msg_tx.time_stamp = k_uptime_get_32();

    rcl_publish(&publisher, &msg_tx, NULL);
   
}




void subscription_callback(const void * msg_in)
{
    const std_msgs__msg__Float32MultiArray * msg = (const std_msgs__msg__Float32MultiArray *) msg_in; 
    if(msg->data.size == 2)
    {
        ros_data_t data_motors_ros;
        
        data_motors_ros.v_left = msg->data.data[0];
        data_motors_ros.v_right=msg->data.data[1];


          if(k_msgq_put(&ros_msg,&data_motors_ros,K_NO_WAIT)==0)
           {
            //udalo sie
           }
           else
           {
            //przepelnione naprzyklad
             atomic_set(&robot_state, STATE_ROS_ERROR);
           }

      
    }




}

void microros_task(void *arg1, void *arg2, void *arg3)
{
    // allocator
    allocator = rcl_get_default_allocator();


    rmw_ret_t transport_ret = rmw_uros_set_custom_transport(
    MICRO_ROS_FRAMING_REQUIRED,
    (void *) &default_params,
    zephyr_transport_open,
    zephyr_transport_close,
    zephyr_transport_write,
    zephyr_transport_read);

if (transport_ret != RMW_RET_OK) {
    while(1) { k_msleep(1000); }
}


    while (1) {
        // 1. Czekamy, aż Agent odpowie (Ping)
        while (rmw_uros_ping_agent(100, 1) != RMW_RET_OK) {
            k_msleep(500); 
        }

        // 2. Inicjalizacja ROS 2 
        rclc_support_init(&support, 0, NULL, &allocator);
        rclc_node_init_default(&node, "stm32_motor_node", "", &support);

        // Subskrypcja
        rclc_subscription_init_default(
            &subscriber,
            &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
            "/motor_values"
        );

        // BUFOR Float32MultiArray 
        static float rx_data_buffer[2];
        msg_rx.data.capacity = 2;
        msg_rx.data.size = 0;     
        msg_rx.data.data = rx_data_buffer; 

        static std_msgs__msg__MultiArrayDimension dim_buffer[1];
        msg_rx.layout.dim.capacity = 1;
        msg_rx.layout.dim.size = 0;
        msg_rx.layout.dim.data = dim_buffer;

        // Timer
        rclc_timer_init_default(
            &timer,
            &support,
            RCL_MS_TO_NS(20),
            timer_callback
        );

        // Publisher
        rclc_publisher_init_default(
            &publisher,
            &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(stm_custom_msgs, msg, StmHardwareState),
            "/stm_sensor_values"
        );

        // Executor
        rclc_executor_init(&executor, &support.context, 2, &allocator);
        rclc_executor_add_subscription(&executor, &subscriber, &msg_rx, &subscription_callback, ON_NEW_DATA);
        rclc_executor_add_timer(&executor, &timer);

     
       
        while (rmw_uros_ping_agent(10, 1) == RMW_RET_OK) {
            rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
            k_msleep(10); 
        }

      
        rclc_executor_fini(&executor);
        rcl_publisher_fini(&publisher, &node);
        rcl_subscription_fini(&subscriber, &node);
        rcl_timer_fini(&timer);
        rcl_node_fini(&node);
        rclc_support_fini(&support);
        
       
    }
}

K_THREAD_DEFINE(microros_thread_id, STACK_SIZE, microros_task, NULL, NULL, NULL, PRIORITY, 0, 0);