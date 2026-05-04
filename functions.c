#include <ctype.h>
#include <stdio.h>
#include <string.h>
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

//Debounce off, button pressed
bool pressed(const uint pin)
{
    int press = 0;
    int release = 0;
    while(press < 3 && release < 3) {
        if(!gpio_get(pin)) {
            press++;
            release = 0;
        }
        else {
            release++;
            press = 0;
        }
        sleep_ms(10); // wait 10 ms
    }
    if(press > release) return true;
    return false;
}

//Send uart command that is wanted and read response
bool send_uart_command(char *buffer, const char *cmd, const char *expected) {
    int position = 0;
    buffer[0] = '\0';

    uart_puts(UART_ID, cmd);

    absolute_time_t timeout = make_timeout_time_ms(500);

    while (get_absolute_time() < timeout){//uart_is_readable_within_us (UART_ID,UART_READ_TIMEOUT)) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n' || c == '\r') {  // if character is escape or newline
                if (position > 0) {
                    buffer[position] = '\0';
                    if (check_response(buffer, expected)) {
                        return true;
                    }
                    position = 0;
                }
            }
            else {
                if (position < MAX_COMMAND_LENGTH - 1) {
                    buffer[position++] = c;  //if no newline or escape or command over max length - 1 = next char
                }
            }
       }
    }
    return false;
}

bool check_response(char *buffer, const char *expected) {
    if (strstr(buffer, expected) != NULL) {return true;}
    return false;
}

void change_dev_eui(char *buffer) {
    int i = 0;
    int j = 0;
    bool stat = false;

    while (buffer[i] != '\0') {  //if buffer[i] is \0 stop loop
        if (buffer[i] == ',') {
            stat = true;
            i++;
        }
        if (stat) {
            if (isxdigit((unsigned char)buffer[i])) {
                buffer[j] = tolower((unsigned char)buffer[i]);
                j++;
            }
        }
        i++;
    }
    buffer[j] = '\0';
}

static void gpio_handler(uint gpio, uint32_t event_mask) {
    event_t event;
    event = EVENT_BUTTON;
    queue_try_add(&events, &event);
    }