#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/util/queue.h"
#include "h_file.h"

static queue_t events;
    //Input and outputs
void gpio_set_mode(const uint pin, const pin_mode_t mode) {
    gpio_init(pin);
    switch (mode) {
        case PULL_UP:
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_up(pin);
            break;
        case PULL_DOWN:
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_down(pin);
            break;
        case OUTPUT_PIN:
            gpio_set_dir(pin, GPIO_OUT);
            break;
        case INPUT_PIN:
            gpio_set_dir(pin, GPIO_IN);
            break;
        default:
            break;
    }
}

void init_event(void) {
    queue_init(&events, sizeof(event_t), 10);
}

bool get_event(event_t *event) {
    return queue_try_remove(&events, event);
}

void gpio_handler(uint gpio, uint32_t event_mask) {
    event_t event = EVENT_BUTTON;
    queue_try_add(&events, &event);
}