#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "h_file.h"

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

    while(1) {

        switch (state) {
            case 0:
                while (!pressed(SW_0)){ tight_loop_contents(); }  //if button is not pressed stay in loop
                while (pressed(SW_0)){ tight_loop_contents(); }   //stay loop as long as button is pressed
                state = 1;  //change state
                break;

            case 1:
                const char connect_lora[] = AT_COMMAND;
                if (send_uart_command(buffer, connect_lora, "OK")) {  //If return is true == lora is connected
                    printf("Connected to LoRa module\n");
                    count = 0;
                    state = 2;
                }
                else {
                    count++;
                    if (count == ATTEMPTS) {  //after at command is send 5 times and evry time false == no lora connection
                        printf("module not responding\n");
                        count = 0;
                        state = 0;
                    }
                }
                break;

            case 2:
                const char at_version[] = AT_FIRMWARE_COMMAND;
                if (send_uart_command(buffer, at_version, "+VER:")) {
                    printf("%s\n", buffer);
                    state = 3;
                }
                else {
                    printf("module stopped responding\n");
                    state = 0;
                }
                break;

            case 3:
                const char com_dev[] = AT_DEV_EUI;
                if (send_uart_command(buffer, com_dev, "+ID:")) {
                    change_dev_eui(buffer);  //if response is resieved change format to lower
                    printf("%s\n", buffer);
                    state = 0; //back to beginning
                }
                else {
                    printf("module stopped responding\n");
                    state = 0;
                }
                break;
        }
    }
}
