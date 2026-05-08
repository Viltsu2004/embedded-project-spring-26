#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "h_file.h"
#include "stdio.h"


int main() {
    stdio_init_all();
    bool led_on = false;

    //pitää yllä dispenserin statusta ja pinnejä
    stepper_status status = {
        .ins_pins = {IN1, IN2, IN3, IN4},
        .count_taken_steps = 0,
        .calibrated = false,
        .step = 0,
        .opto_fork_length = 0,
        .steps = {
                        {1, 0, 0, 0},
                        {1, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 1},
                        {0, 0, 0, 1},
                        {1, 0, 0, 1},
                    }
    };

    gpio_set_mode(LED1, OUTPUT_PIN);

    gpio_set_mode(PIEZO_SENSOR, PULL_UP);
    gpio_set_mode(OPTO_FORK, PULL_UP);

    gpio_set_mode(SW_0, PULL_UP);
    gpio_set_mode(SW_2, PULL_UP);
    //Initialize outputs and inputs
    for (int i = 0; i < 4; i++) {
        gpio_set_mode(status.ins_pins[i], OUTPUT_PIN);
    }

    init_event();
    gpio_set_irq_enabled_with_callback(SW_0, GPIO_IRQ_EDGE_FALL, true, &gpio_handler);
    gpio_set_irq_enabled(SW_2, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(PIEZO_SENSOR, GPIO_IRQ_EDGE_FALL, true);

    event_t event;
    absolute_time_t toggle = make_timeout_time_ms(1000);

    uart_init(UART_ID, BAUD_RATE);
    //set uart to functionate
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    char buffer[MAX_COMMAND_LENGTH];
    int counter = 0;
    int switch_state = 0;
    //int state = 0;
    int count = 0;
    //bool flag = false;

    /*while(!flag) {

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
                        flag = true;
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
                    flag = true;
                }
                break;

            case 3:
                const char com_dev[] = AT_DEV_EUI;
                if (send_uart_command(buffer, com_dev, "+ID:")) {
                    change_dev_eui(buffer);  //if response is resieved change format to lower
                    printf("%s\n", buffer);
                }
                else {
                    printf("module stopped responding\n");
                    state = 0;
                }
                flag = true;
                break;
        }
    }*/




    while (1) {
        switch (switch_state) {
            case 0:
                while (!pressed(SW_0)){ tight_loop_contents(); }  //if button is not pressed stay in loop
                while (pressed(SW_0)){ tight_loop_contents(); }
                const char connect_lora[] = AT_COMMAND;
                if (send_uart_command(buffer, connect_lora, "OK")) {  //If return is true == lora is connected
                    printf("Connected to LoRa module\n");
                    count = 0;
                    switch_state = 1;
                }
                else {
                    count++;
                    if (count == ATTEMPTS) {  //after at command is send 5 times and evry time false == no lora connection
                        printf("module not responding\n");
                        count = 0;
                        switch_state = 0;
                        //flag = true;
                    }
                }
                break;
            case 1:
                const char com_dev[] = AT_DEV_EUI;
                if (send_uart_command(buffer, com_dev, "+ID:")) {
                    change_dev_eui(buffer);  //if response is resieved change format to lower
                    printf("%s\n", buffer);
                    switch_state = 2;
                }
                else {
                    printf("module stopped responding\n");
                    switch_state = 0;
                }
                break;
            case 2:
                const char lwoota[] = "AT+MODE=LWOTAA\r\n";
                if (send_uart_command(buffer, lwoota, "+MODE:")) {
                    printf("%s\n", buffer);
                    switch_state = 3;
                }
                else {
                    printf("module stopped responding\n");
                    switch_state = 0;
                }
                break;
            case 3:
                const char appkey[] = "AT+KEY=APPKEY,y33k55qj6klf9vc1f0wmm4t00000gn\r\n";
                if (send_uart_command(buffer, appkey, "+KEY:")) {
                    printf("%s\n", buffer);
                    switch_state = 4;
                }
                else {
                    printf("module stopped responding\n");
                    switch_state = 0;
                }
                break;
            case 4:
                const char class[] = "AT+CLASS=A\r\n";
                if (send_uart_command(buffer, class, "+CLASS:")) {
                    printf("%s\n", buffer);
                    switch_state = 5;
                }
                else {
                    printf("module stopped responding\n");
                    switch_state = 0;
                }
                break;
            case 5:
                const char port[] = "AT+PORT=8\r\n";
                if (send_uart_command(buffer, port, "+PORT:")) {
                    printf("%s\n", buffer);
                    switch_state = 6;
                }
                else {
                    printf("module stopped responding\n");
                    switch_state = 0;
                }
                break;
            case 6:
                const char dr[] = "AT+DR=5\r\n";
                if (send_uart_command(buffer, dr, "+DR:")) {
                    printf("%s\n", buffer);
                    switch_state = 7;
                }
                else {
                    printf("module stopped responding\n");
                    switch_state = 0;
                }
                break;
            case 7:
                const char join[] = "AT+JOIN\r\n";
                if (send_uart_command(buffer, join, "+JOIN:")) {
                    printf("%s\n", buffer);
                    switch_state = 8;
                }
                else {
                    printf("module stopped responding\n");
                    switch_state = 0;
                }
                break;
            case 8:
                break;


/*
            case 1:
                if (get_event(&event)) {    //interrupt
                    if (!status.calibrated && event == CALIBRATE) { //only sw_0 interrupt -> calibrate device
                        calib(&status);
                        gpio_put(LED1, 1);
                    }
                    else if (event == DISPENSE_PILL && status.calibrated) { //only sw_2 interrupt -> dispense pills
                        switch_state = 2;
                        gpio_put(LED1, 0);
                        run_motor_n_revolution(1, &status);
                        toggle = make_timeout_time_ms(5000);
                    }
                }

                //if not calibrated -> led blinks
                if (!status.calibrated && time_reached(toggle)) {
                    led_on = !led_on;
                    gpio_put(LED1, led_on);
                    toggle = make_timeout_time_ms(1000);
                }
                break;


            case 2:
                gpio_put(LED1, 0); //leds off
                if (time_reached(toggle)) { //if time reached -> dispense one pill
                    empty_gue();
                    if (!run_motor_n_revolution(1, &status)) { //if false, -> no pill dispensed -> led blinks
                        for (int i = 0; i < 6; i++) {
                            led_on = !led_on;
                            gpio_put(LED1, led_on);
                            sleep_ms(100);
                        }
                    }
                    counter++;  //count rotations
                    toggle = make_timeout_time_ms(5000);
                }
                if (counter == FULL_REVOLUTION) {
                    counter = 0;
                    status.calibrated = false;
                    switch_state = 1;
                }
                break;*/
        }
    }
}