
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
    int16_t yaw;
    int16_t roll;
    int16_t pitch;
} imu_sample_t;

typedef enum{
    STATE_OKAY = 0,
    STATE_ERROR,
    STATE_ROS_ERROR,

} robot_state_t;