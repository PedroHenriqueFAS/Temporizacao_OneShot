#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#define LED_PIN_B 11
#define LED_PIN_R 12
#define LED_PIN_G 13
#define BUTTON_PIN 5

bool leds_active = false; // Indica se a sequência está em execução

// Callback para apagar o LED verde
int64_t turn_off_green_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_G, 0);
    leds_active = false; // Permite nova ativação pelo botão
    return 0;
}

// Callback para apagar o LED vermelho
int64_t turn_off_red_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_R, 0);
    add_alarm_in_ms(3000, turn_off_green_callback, NULL, false);
    return 0;
}

// Callback para apagar o LED azul
int64_t turn_off_blue_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_B, 0);
    add_alarm_in_ms(3000, turn_off_red_callback, NULL, false);
    return 0;
}

int main() {
    stdio_init_all();

    // Inicializa os pinos dos LEDs
    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    // Inicializa o botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true) {
        // Verifica se o botão foi pressionado (nível baixo no pino) e se o LED não está ativo.
        if (gpio_get(BUTTON_PIN) == 0 && !leds_active) {
            sleep_ms(50); // Debounce

            // Verifica novamente o estado do botão após o debounce.
            if (gpio_get(BUTTON_PIN) == 0) {
                leds_active = true;
                gpio_put(LED_PIN_B, 1);
                gpio_put(LED_PIN_R, 1);
                gpio_put(LED_PIN_G, 1);
                
                // Agenda um alarme para desligar o LED após 3 segundos (3000 ms).
                // A função 'turn_off_callback' será chamada após esse tempo.
                add_alarm_in_ms(3000, turn_off_blue_callback, NULL, false);
            }
        }
        // Introduz uma pequena pausa de 10 ms para reduzir o uso da CPU.
        // Isso evita que o loop seja executado muito rapidamente e consuma recursos desnecessários.
        sleep_ms(10); // Evita uso excessivo da CPU
    }
    return 0;
}
