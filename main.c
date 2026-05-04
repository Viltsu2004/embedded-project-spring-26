#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "h_file.h"
#include "pico/util/queue.h"

static queue_t events;

int main() {
    stdio_init_all();
    //input pinnit funktiolla
    gpio_set_mode(SW_0, PULL_UP);

    uart_init(UART_ID, BAUD_RATE);
    //set uart to functionate
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    char buffer[MAX_COMMAND_LENGTH];
    int state = 0;
    int count = 0;

    queue_init(&events, sizeof(int), 10);
    gpio_set_irq_enabled_with_callback(SW_0, GPIO_IRQ_EDGE_FALL, true, &gpio_handler);
    event_t event;

    gpio_set_mode(20, OUTPUT_PIN);

    while (queue_try_remove(&events, &event))

        while(1) {
            while (queue_try_remove(&events, &event)) {
                switch (event) {
                    case EVENT_BUTTON:
                        gpio_put(20, true);
                }
            }
        }
}
