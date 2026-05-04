#ifndef BLINK_H_FILE_H
#define BLINK_H_FILE_H

#define SW_0 9

#define UART_ID uart1
#define BAUD_RATE 9600

#define UART_TX_PIN 5
#define UART_RX_PIN 4
#define UART_READ_TIMEOUT 500000

#define ATTEMPTS 5
#define MAX_COMMAND_LENGTH 50
#define AT_COMMAND "AT\r\n"
#define AT_FIRMWARE_COMMAND "AT+VER\r\n"
#define AT_DEV_EUI "AT+ID=DevEui\r\n"

typedef enum {
    PULL_UP,
    PULL_DOWN,
    OUTPUT_PIN,
    INPUT_PIN
} pin_mode_t;

typedef enum {
    EVENT_BUTTON,
    EVENT_PLUS,
    EVENT_MINUS,
} event_t;

void init_event(void);
bool get_event(event_t *event);
void gpio_set_mode(uint pin, pin_mode_t mode);
void gpio_handler(uint gpio, uint32_t event_mask);

#endif //BLINK_H_FILE_H