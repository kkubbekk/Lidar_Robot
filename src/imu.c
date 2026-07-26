#include "imu.h"

#define BNO055_REG_OPR_MODE   0x3D
#define BNO055_MODE_NDOF      0x0C
#define BNO055_REG_CALIB_STAT 0x35
#define BNO055_REG_EULER_BASE 0x1A

extern const struct i2c_dt_spec imu;




bool imu_turn_on(void)
{
 

uint8_t buffer[2] = {BNO055_REG_OPR_MODE, BNO055_MODE_NDOF};
int ret = i2c_write_dt(&imu, buffer, sizeof(buffer));
  if(ret != 0)
  {
    printk("FAILED TO TURN ON IMU\n");
    return false;
  }
  return true;
}

bool imu_if_ready(void)
{
  uint8_t reg_addr = BNO055_REG_CALIB_STAT;
  uint8_t calib_state = 0;

  int ret =  i2c_write_read_dt(&imu,&reg_addr,1, &calib_state, 1);

  if(ret!=0)
  {
    return false;
  }
  //0xFF czyli jesli wszystko dziala gyro,akcelerometr magnetrometr
  if(calib_state < 0xFF)
  {
    return true;
  }
  return false;
}

bool imu_read_data(float *yaw, float *roll,float *pitch)
{
    uint8_t reg_addr = BNO055_REG_EULER_BASE;
    uint8_t data_buff[6] = {0};

    int ret = i2c_write_read_dt(&imu,&reg_addr,1,data_buff,6);

    if(ret !=0)
    {
        printk("FAILED TO WRITE/READ DATA I2C device adress\n");
        return false;
    }

    *yaw   = (float)((int16_t)((data_buff[1] << 8) | data_buff[0]))/16.0f;
    *roll  = (float)((int16_t)((data_buff[3] << 8) | data_buff[2]))/16.0f;
    *pitch = (float)((int16_t)((data_buff[5] << 8) | data_buff[4]))/16.0f;

    return true;
}

