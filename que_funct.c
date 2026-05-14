#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/util/queue.h"
#include "string.h"

#include "que.h"

static queue_t events;

//init gueue
void init_event() {
    queue_init(&events, sizeof(event_t), 10);
}

//try remove event form gueue
bool get_event(event_t *event) {
    return queue_try_remove(&events, event);
}

//interrupt
void gpio_handler(uint gpio, uint32_t event_mask) {
    int event = 0;
    if (gpio == SW_0) {
        event = CALIBRATE;
    }
    else if (gpio == SW_2) {
        event = DISPENSE_PILL;
    }
    else if (gpio == PIEZO_SENSOR) {
        event = PILL_FALLING;
    }
    queue_try_add(&events, &event);
}

void empty_gue() {
    event_t event;
    while (get_event(&event)) {tight_loop_contents();}
}