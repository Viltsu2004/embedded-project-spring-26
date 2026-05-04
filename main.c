#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "h_file.h"


int main() {
    stdio_init_all();

    gpio_set_mode(SW_0, PULL_UP);
    init_event();
    gpio_set_irq_enabled_with_callback(SW_0, GPIO_IRQ_EDGE_FALL, true, &gpio_handler);
    gpio_set_mode(20, OUTPUT_PIN);
    event_t event;

    while (1) {
        while (get_event(&event)) {
            gpio_put(20, 1);
        }
    }
}
