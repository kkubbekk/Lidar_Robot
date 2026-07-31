
typedef struct {
    float v_left_mps;  // predkosc lewego kola w metrach na sekunde
    float v_right_mps; // predkosc prawego kola w metrach na sekunde
    int32_t timestamp_ms;     // czas w milisekundach od uruchomienia programu
    int64_t acumulated_left_encoder_ticks;  // skumulowana liczba tickow z lewego enkodera
    int64_t acumulated_right_encoder_ticks; // skumulowana liczba tickow z prawego enkodera
    float motor_angular_speed;
    float robot_speed_mps;
    } motor_sample_t;


typedef struct {
    float yaw;
    float roll;
    float pitch;
} imu_sample_t;

typedef enum{
    STATE_OKAY = 0,
    STATE_TIPPED,
    STATE_SENSOR_ERROR,
    STATE_ROS_ERROR,
    STATE_MOTOR_FAULT,
    STATE_LOW_BATTERY,//jak dojdzie akus kiedys

} robot_state_t;

typedef struct 
{
   float v_left;
   float v_right;
} ros_data_t;
