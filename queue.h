#ifndef BLINK_UART_H
#define BLINK_UART_H

#define SW_0 9
#define SW_2 7
#define PIEZO_SENSOR 27

typedef enum {
    CALIBRATE = 1,
    DISPENSE_PILL = 2,
    PILL_FALLING = 3
} event_t;

void init_event();
bool get_event(event_t *event);
void empty_gue();  //empty gue so no messages is left
void gpio_handler(uint gpio, uint32_t event_mask); //interrupt handler

#endif //BLINK_UART_H