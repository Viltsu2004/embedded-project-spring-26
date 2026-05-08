#ifndef BLINK_H_FILE_H
#define BLINK_H_FILE_H

#define SW_0 9
#define SW_2 7
#define PIEZO_SENSOR 27

#define LED1 21

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

#define IN1 2
#define IN2 3
#define IN3 6
#define IN4 13

#define CALIBRATION_ROTATIONS 1
#define NUMBER_OF_OUTPUT_PINS 4
#define FULL_REVOLUTION 8

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

typedef enum {
    CALIBRATE = 1,
    DISPENSE_PILL = 2,
    PILL_FALLING = 3
} event_t;

void empty_gue();  //empty gue so no messages is left
void calib(stepper_status *status); //calibrate device
int run_stepper_helper(stepper_status *status, int prev_value);  //run one step
void init_event(); //queue init
bool get_event(event_t *event); //remove event
void gpio_set_mode(uint pin, pin_mode_t mode);  //outputs and inputs and pulls and downs
void gpio_handler(uint gpio, uint32_t event_mask); //interrupt handler
bool run_motor_n_revolution(int number, stepper_status *status); //run motor n places
bool check_response(const char *buffer, const char *expected);
bool send_uart_command(char *buffer, const char *cmd, const char *expected);
void change_dev_eui(char *buffer);
bool pressed(uint pin);

#endif //BLINK_H_FILE_H