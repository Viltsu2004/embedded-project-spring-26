#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/util/queue.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"


#include "lora_communication.h"

bool check_join(const char *cmd, const char *expected, const int timeout) {
    char buffer[MAX_COMMAND_LENGTH];
    int position = 0;
    buffer[0] = '\0';

    uart_puts(UART_ID, cmd);

    absolute_time_t check_time = make_timeout_time_ms(timeout);

    while (get_absolute_time() < check_time){//uart_is_readable_within_us (UART_ID,UART_READ_TIMEOUT)) {
        while (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n' || c == '\r') {  // if character is escape or newline
                if (position > 0) {
                    buffer[position] = '\0';
                    if (strstr(buffer, "+JOIN: Join failed")) {
                        return false;
                    }
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

bool send_lora_command(const char *cmd, const char *expected, const int timeout) {
    char buffer[MAX_COMMAND_LENGTH];
    int position = 0;
    buffer[0] = '\0';

    uart_puts(UART_ID, cmd);

    absolute_time_t check_time = make_timeout_time_ms(timeout);

    while (get_absolute_time() < check_time){//uart_is_readable_within_us (UART_ID,UART_READ_TIMEOUT)) {
        while (uart_is_readable(UART_ID)) {
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


bool check_response(const char *buffer, const char *expected) {
    printf("buffer: %s\n", buffer);
    if (strstr(buffer, expected) != NULL) {return true;}
    return false;
}


int connect_lora() {
    if (!send_lora_command(AT_COMMAND, AT_CMD_ANSWER, CMD_TIMEOUTS1)) {
        return false;
    }
    if (!send_lora_command(AT_MODE, MODE_CMD_ANSWER, CMD_TIMEOUTS1)) {
        return false;
    }
    if (!send_lora_command(AT_KEY, KEY_CMD_ANSWER, CMD_TIMEOUTS1)) {
        return false;
    }
    if (!send_lora_command(AT_CLASS, CLASS_CMD_ANSWER, CMD_TIMEOUTS1)) {
        return false;
    }
    if (!send_lora_command(AT_PORT, PORT_CMD_ANSWER, CMD_TIMEOUTS1)) {
        return false;
    }
    if (!send_lora_command(AT_DR, DR_CMD_ANSWER, CMD_TIMEOUTS1)) {
        return false;
    }
    if (!check_join(AT_JOIN, JOIN_CMD_ANSWER, CMD_TIMEOUTS2)) {
        return false;
    }
    return true;
}

bool send_message(bool *connection, const char *message) {
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "AT+MSG=\"%s\"\r\n", message);
    if (!send_lora_command(command, AT_CMD_ANSWER, CMD_TIMEOUTS1)) {
        *connection = true;
        return false;
    }
    *connection = false;
    return true;
}