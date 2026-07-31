
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/posix/time.h>
#include "shared_data.h"
#include <stm_custom_msgs/msg/stm_hardware_state.h>

extern struct k_msgq imu_msg;
extern struct k_msgq motor_msg;

extern atomic_t robot_state;

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/float32_multi_array.h>

#include <rclc/rclc.h>
#include <rclc/executor.h>

rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;
rcl_publisher_t publisher;


rcl_subscription_t subscriber;
rcl_timer_t timer;

std_msgs__msg__Float32MultiArray msg_rx;

std_msgs__msg__Float32MultiArray msg_tx;



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
void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
    imu_sample_t imu_data={0};
    motor_sample_t moto_data={0};
    
    int ret = k_msgq_get(&imu_msg, &imu_data, 100);
    int ret1 = k_msgq_get(&motor_msg,&moto_data,100);
    if(ret1 == 0 || ret == 0)
    {
        msg_tx.data.
    }
    else{

    }


}

void main(void)
{

    K_MSGQ_DEFINE(ros_msg, sizeof(ros_data_t), 5, 4);

 //alocator
    allocator = rcl_get_default_allocator();


    rclc_support_init(&support,0,NULL,&allocator);

    rclc_node_init_default(&node,"stm32_motor_node","",&support);
//subcriber definition
    rclc_subcription_init_default(
        &subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
        "/motor_values");

    static float rx_data_buffer[2];
    msg_rx.data.capacity = 2;
    msg_rx.data.size = 0;     
    msg_rx.data.data = rx_data_buffer; 

 


    rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(20),
        timer_callback);

    rclc_executor_init(&executor, &support.context, 2, &allocator);

    rclc_executor_add_subscription(&executor, &subscriber, &msg_rx, &subscription_callback, ON_NEW_DATA);
    rclc_executor_add_timer(&executor, &timer);

    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(stm_custom_msgs, msg, StmHardwareState),
       "/stm_sensor_values" //name of ros topic that i will put info on
);

    


   while (1) {
       
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
        
       
        k_msleep(10); 
    }
}

 
