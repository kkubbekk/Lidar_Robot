#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>

#define STACK_SIZE 1024
#define PRIORITY 4

LOG_MODULE_REGISTER(task_meas, LOG_LEVEL_INF); 

#define ADC_NODE DT_PATH(zephyr_user)

 const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(ADC_NODE);

void meas_task(void *arg1, void *arg2, void *arg3);


K_THREAD_DEFINE(meas_thread_id, STACK_SIZE, meas_task, NULL, NULL, NULL, PRIORITY, 0, 0);

void meas_task(void *arg1, void *arg2, void *arg3)
{
    int err;
    uint16_t data_buffer;

    struct adc_sequence sequence = {
        .buffer = &data_buffer,
        .buffer_size = sizeof(data_buffer),
    };

    if(!adc_is_ready_dt(&adc_channel))
    {
        return;
    }
    err = adc_channel_setup_dt(&adc_channel);

    if(err < 0) {
        // printk("BLAD KONFIGURACJI ADC");
        return;
    }
    
    err = adc_sequence_init_dt(&adc_channel, &sequence);
    if (err < 0) {
    
        return;
    }
      
    
    

    for(;;)
    {
       err = adc_read(adc_channel.dev, &sequence);
        if (err < 0) {
            // LOG_ERR("Błąd odczytu: %d", err);
        } else {
           
            int32_t val_mv = data_buffer;
            
        
            err = adc_raw_to_millivolts_dt(&adc_channel, &val_mv);
            if (err < 0) {
                // LOG_WRN("Nie udało się przeliczyć na mV, surowy wynik: %d", buf);
            } else {
                printk("Odczyt ADC: %d mV (surowo: %d)", val_mv, data_buffer);
            }
        }

        
        k_sleep(K_MSEC(500));
    }
}