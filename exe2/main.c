#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_callback_red(repeating_timer_t *rt) {
    if (gpio_get(LED_PIN_R)) {
        gpio_put(LED_PIN_R, 0);
    } else {
        gpio_put(LED_PIN_R, 1);
    }
    return true;  
}
bool timer_callback_green(repeating_timer_t *rt) {
    if (gpio_get(LED_PIN_G)) {
        gpio_put(LED_PIN_G, 0);
    } else {
        gpio_put(LED_PIN_G, 1);
    }
    return true;  
}

int main() {

    int timer_red_active = 0;
    repeating_timer_t timer_red;
    int timer_green_active = 0;
    repeating_timer_t timer_green;

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,&btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    while (true) {

        if (flag_r) {
            flag_r = 0;
            if (!timer_red_active) {
                if (add_repeating_timer_ms(500, timer_callback_red, NULL, &timer_red)) {
                    timer_red_active = true;  
                }
            } else {
                gpio_put(LED_PIN_R, 0);
                cancel_repeating_timer(&timer_red);
                timer_red_active = false;
            }
        }
        if (flag_g) {
            flag_g = 0;
            if (!timer_green_active) {
                if (add_repeating_timer_ms(250, timer_callback_green, NULL, &timer_green)) {
                    timer_green_active = true;  
                }
            } else {
                gpio_put(LED_PIN_G, 0);
                cancel_repeating_timer(&timer_green);
                timer_green_active = false;
            }
        }
    }
}
