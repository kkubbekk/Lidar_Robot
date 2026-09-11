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

//LUT
typedef struct {
    uint16_t voltage_mv;
    uint8_t percentage;
} lipo_lut_t;

const lipo_lut_t bashing3s_lut[] =
{
    {12600, 100}, // 4.20V 
    {12300, 90},  // 4.10V 
    {11700, 70},  // 3.90V 
    {11400, 60},  // 3.80V  
    {11100, 40},  // 3.70V 
    {10800, 20},  // 3.60V
    {10500, 10},  // 3.50V 
    {9900,  5},   // 3.30V  
    {9600,  0}    // 3.20V  CUTOFF 
};

int batterry_percentege_lut(int actual_mv_bat, lipo_lut_t* bashing_lut)
{
    if (actual_mv_bat >= lut[0].voltage_mv) {
        return lut[0].percentage;
    }

    if (actual_mv_bat <= lut[lut_size - 1].voltage_mv) {
        return lut[lut_size - 1].percentage;
    }
    
    for (size_t i = 0; i < lut_size - 1; i++) {

            uint16_t v_high = lut[i].voltage_mv;
            uint16_t v_low  = lut[i + 1].voltage_mv;

            if (actual_mv_bat <= v_high &&
                actual_mv_bat >= v_low) {

                uint8_t p_high = lut[i].percentage;
                uint8_t p_low  = lut[i + 1].percentage;

                uint16_t v_diff = v_high - v_low;
                uint8_t p_diff = p_high - p_low;

                uint8_t exact_percentage = p_low + ((actual_mv_bat - v_low) * p_diff) / v_diff;

                return exact_percentage;
            }
        }
    
}

#define LUT_SIZE (sizeof(bashing3s_lut) / sizeof(bashing3s_lut[0]))



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

    uint8_t batterry_percentege;
    

  
    for(;;) {
     
        err = adc_read_dt(&adc_channel, &sequence);
        
        if (err == 0) {
          ring_buff_write(&battery_buff, buf);
          
        batterry_percentege = batterry_percentege_lut(actual_battery_mv(avg_ring_buff(&battery_buff)));

          if(batterry_percentege < 5)
          {
            atomic_set(&robot_state, STATE_LOW_BATTERY);
          }
        
         
        }

        k_msleep(100); 
    }
}