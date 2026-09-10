#include <zephyr/drivers/i2c.h>
#include "stdlib.h"

bool imu_turn_on(void);


bool imu_if_ready(void);
bool imu_read_data(float *yaw, float *roll, float *pitch);