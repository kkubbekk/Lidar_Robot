
typedef struct {
    float v_left_mps;  // predkosc lewego kola w metrach na sekunde
    float v_right_mps; // predkosc prawego kola w metrach na sekunde
    int32_t timestamp_ms;     // czas w milisekundach od uruchomienia programu
    int64_t acumulated_left_encoder_ticks;  // skumulowana liczba tickow z lewego enkodera
    int64_t acumulated_right_encoder_ticks; // skumulowana liczba tickow z prawego enkodera
    } motor_sample_t;

