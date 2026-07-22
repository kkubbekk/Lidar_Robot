#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// Pobranie konfiguracji pinów z pliku .overlay za pomocą aliasów
static const struct gpio_dt_spec ain1 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_ain1), gpios);
static const struct gpio_dt_spec ain2 = GPIO_DT_SPEC_GET(DT_ALIAS(motor_ain2), gpios);
static const struct gpio_dt_spec pwma = GPIO_DT_SPEC_GET(DT_ALIAS(motor_pwma), gpios);

int main(void)
{
    // Sprawdzenie, czy sterowniki GPIO są gotowe
    if (!gpio_is_ready_dt(&ain1) || !gpio_is_ready_dt(&ain2) || !gpio_is_ready_dt(&pwma)) {
        return 0;
    }

    // Konfiguracja wszystkich trzech pinów jako wyjścia cyfrowe
    gpio_pin_configure_dt(&ain1, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&ain2, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&pwma, GPIO_OUTPUT_ACTIVE);

    // USTAWIANIE STANÓW PINÓW:
    // Kierunek: AIN1 = 1, AIN2 = 0 (silnik kręci w jedną stronę)
    gpio_pin_set_dt(&ain1, 1);
    gpio_pin_set_dt(&ain2, 0);
    
    // Prędkość: PWMA = 1 (zastępuje nam PWM na poziomie 100% mocy)
    gpio_pin_set_dt(&pwma, 1);

    // Główna pętla programu – nic nie robi, utrzymuje stany pinów
    while (1) {
        k_msleep(1000);
    }

    return 0;
}