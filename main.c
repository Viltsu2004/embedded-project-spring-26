#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "stdio.h"

#include "interrupt_button.h"
#include "lora_eeprom.h"
#include "que.h"


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

    int counter = 0;
    int state = 1;
    bool lora_connection = false;


    if (connect_lora()) {
        send_message(&lora_connection, "boot");
    }


    while(1) {
        switch (state) {
            case 1:
                if (get_event(&event)) {    //interrupt
                    if (!status.calibrated && event == CALIBRATE) { //only sw_0 interrupt -> calibrate device
                        calib(&status);
                        gpio_put(LED1, 1);
                        if (!lora_connection) {
                            send_message(&lora_connection, "calibrated");
                        }
                    }
                    else if (event == DISPENSE_PILL && status.calibrated) { //only sw_2 interrupt -> dispense pills
                        state = 2;
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
                /*if (counter == 0) {
                    toggle = make_timeout_time_ms(30000);
                    if (!lora_connection) {
                        send_message(&lora_connection, "calibrated");
                    }
                }*/
                gpio_put(LED1, 0); //leds off
                if (time_reached(toggle)) { //if time reached -> dispense one pill
                    empty_gue();
                    if (!run_motor_n_revolution(1, &status)) { //if false, -> no pill dispensed -> led blinks
                        for (int i = 0; i < 6; i++) {
                            led_on = !led_on;
                            gpio_put(LED1, led_on);
                            sleep_ms(100);
                        }
                        if (!lora_connection) {
                            send_message(&lora_connection, "not dispensed");
                        }
                    }
                    else {
                        if (!lora_connection) {
                            send_message(&lora_connection, "pill dispensed");
                        }
                    }
                    counter++;  //count rotations
                    toggle = make_timeout_time_ms(30000);
                }
                if (counter == FULL_REVOLUTION) {
                    counter = 0;
                    status.calibrated = false;
                    if (!lora_connection) {
                        send_message(&lora_connection, "Dispenser empty");
                    }
                    state = 1;
                }
                break;
        }
    }
}