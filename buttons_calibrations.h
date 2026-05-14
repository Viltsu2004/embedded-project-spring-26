#ifndef BLINK_H_FILE_H
#define BLINK_H_FILE_H

#define LED1 21
#define BAUD_RATE 9600

#define UART_TX_PIN 5
#define UART_RX_PIN 4
#define UART_READ_TIMEOUT 500000

#define ATTEMPTS 5

#define IN1 2
#define IN2 3
#define IN3 6
#define IN4 13

#define CALIBRATION_ROTATIONS 1
#define NUMBER_OF_OUTPUT_PINS 4
#define FULL_REVOLUTION 8
#define RUN_PILLS 7
#define BLINKER_TIMEOUT 1000
#define DAY_WAIT 3000
#define SLEEP 100

#define HALF_OPTO_FORK_LENGTH 2

#define OPTO_FORK 28

typedef struct {
    const int ins_pins[4];
    int count_taken_steps;
    bool calibrated;
    int step;
    const int steps[8][4];
    int opto_fork_length;
} stepper_status;

typedef enum {
    PULL_UP,
    PULL_DOWN,
    OUTPUT_PIN,
    INPUT_PIN
} pin_mode_t;


void calib(stepper_status *status); //calibrate device
int run_stepper_helper(stepper_status *status, int prev_value);  //run one step
void gpio_set_mode(uint pin, pin_mode_t mode);  //outputs and inputs and pulls and downs
bool run_motor_n_revolution(int number, stepper_status *status); //run motor n places

#endif //BLINK_H_FILE_H