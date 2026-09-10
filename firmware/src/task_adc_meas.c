#define TASK_TIME_SAMPLE_MS 100
#define V_MIN 9000
#define V_MAX 12600

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include "ringbuff.h"
#include "shared_data.h"

void task_imu(void *arg1,void *arg2,void *arg3);

K_THREAD_DEFINE(adc_thread_id, STACK_SIZE, task_adc_meas, NULL, NULL, NULL, PRIORITY, 0, 0);

extern atomic_t robot_state;

// mam dzielnik 33k 10k
int actual_battery_mv(int avg)
{
    return ( (avg*3300)/4095 * 43 / 10 )
}



void task_adc_meas(void *arg1, void *arg2, void *arg3)
{
   
    static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_NODELABEL(adc_battery));

    int err;
    uint16_t buf; 

    if (!adc_is_ready_dt(&adc_channel)) {
    
        return; 
    }


    err = adc_channel_setup_dt(&adc_channel);
    if (err < 0) {
        
        return; 
    }

    struct adc_sequence sequence = {
        .buffer = &buf,
        .buffer_size = sizeof(buf),
    };
  
    adc_sequence_init_dt(&adc_channel, &sequence); 

    //buffer

    RingBuf battery_buff;

    ring_buff_init(&battery_buff);

    int avg_voltage;
    

  
    for(;;) {
     
        err = adc_read_dt(&adc_channel, &sequence);
        
        if (err == 0) {
          ring_buff_write(&battery_buff, buf);
          
          avg_voltage = avg_ring_buff(&battery_buff);

          if(actual_battery_mv(avg_voltage)<V_MIN)
          {
            atomic_set(&robot_state, STATE_LOW_BATTERY);
          }
        
         
        }

        k_msleep(100); 
    }
}