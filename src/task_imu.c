#define  STACK_SIZE 1024
#define PRIORITY 4
#define SAMPLE_PERIOD_MS 10

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include "shared_data.h"
#include <zephyr/drivers/i2c.h>

#include "imu.h"




//thread definition
void task_imu(void *arg1,void *arg2,void *arg3);

//task definition
K_THREAD_DEFINE(imu_thread_id, STACK_SIZE, task_imu, NULL, NULL, NULL, PRIORITY, 0, 0);

//timer to better synchronize
K_TIMER_DEFINE(imu_sample_timer,NULL,NULL);

//i2c struct
extern const struct i2c_dt_spec imu;

void task_imu(void *arg1,void *arg2,void *arg3)
{
  

    k_msleep(50);

    k_timer_start(&imu_sample_timer, K_MSEC(SAMPLE_PERIOD_MS), K_MSEC(SAMPLE_PERIOD_MS));

    //variables to store imu data (yaw,pitch,roll)
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;

    if(!imu_turn_on())
    {
      //doda sie state machine ogolnie 
      printk("Blad wlaczenia czujnika!\n");
    }

     
    
   

    

    for(;;)
    {
        
      if(imu_if_ready()){
      
          if(imu_read_data(&yaw,&roll,&pitch))
      {
          if(abs(roll)<30.0f || abs(pitch)<30.0f)
          {
            //test
            printk("yaw: %d,roll: %d,pitch: %d\n",(int)yaw, (int)roll, (int)pitch);
            
          }
          else
          {
             printk("robot sie wyjebal");
             //TODO: dodanie maszyny stanow
          }
      }
    }
     
      
       
       

    k_timer_status_sync(&imu_sample_timer);
      
  
  
  }
    
    
}
