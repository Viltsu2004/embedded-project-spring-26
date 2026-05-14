#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/util/queue.h"
#include "string.h"

#include "buttons_calibrations.h"
#include "queue.h"

static event_t event;

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


//calibrate device
void calib(stepper_status *status) {
    status->count_taken_steps = 0; //clear earlier steps
    status->opto_fork_length = 0;
    status->calibrated = false;   //set calibration to false

    int prev_status = gpio_get(OPTO_FORK);  //get status of opto fork, 0 or 1?

    //run motor to 0 point
    bool flag_zero_point = false;
    while (!flag_zero_point) {
        if (run_stepper_helper(status, prev_status)) { //run steps as long as stepper motor returns 1 == opto fork 0
            flag_zero_point = true; //set flag to end loop
        }
        prev_status = gpio_get(OPTO_FORK); //get previous status
    }


    for (int i = 0; i < CALIBRATION_ROTATIONS; i++) {  //make 3 full rotations

        bool calib_flag = false;  //rotations status

        prev_status = gpio_get(OPTO_FORK); //opto fork status

        while (!calib_flag) { //if rotation is not redy run loop
            if (run_stepper_helper(status, prev_status)) { //rotation ready -> out of loop
                calib_flag = true;
            }

            if (prev_status == false) { //in last rotation calculate opto_fork length
                status->opto_fork_length++;
            }

            status->count_taken_steps++;  //add 1 to taken steps
            prev_status = gpio_get(OPTO_FORK); //status of opto fork
        }
    }

    status->opto_fork_length = status->opto_fork_length / CALIBRATION_ROTATIONS / HALF_OPTO_FORK_LENGTH;  //center of opto_fork false area

    for (int r = 0; r < status->opto_fork_length; r++) {
        run_stepper_helper(status, prev_status);  //rotation ready -> out of loo
    }

    status->count_taken_steps = status->count_taken_steps/CALIBRATION_ROTATIONS; //avarage steps of rotation = sum of 3 rotation steps divided by 3
    status->calibrated = true;  //calibration status true
}


//run x amount of steps
int run_stepper_helper(stepper_status *status, int prev_value) {
    status->step = (status->step + 1) % FULL_REVOLUTION;  //move motor 1 step
    for (int i = 0; i < NUMBER_OF_OUTPUT_PINS; i++) { //drive all three pins
        gpio_put(status->ins_pins[i], status->steps[status->step][i]);
    }
    sleep_ms(1);
    int current_value = gpio_get(OPTO_FORK);
    if (prev_value == 1 && current_value == 0) {
        return 1; // if opto fork status 0 but previous status 1 mean we have falling edge
    }
    return 0;
}

// run x many places = dispence x many pills
bool run_motor_n_revolution(int number, stepper_status *status) {
    int pill_fall = false;
    int moved_step = status->count_taken_steps / FULL_REVOLUTION * number;
    int prev = gpio_get(OPTO_FORK);
    for (int i = 0; i < moved_step; i++) { //run motor as long as it reach wanted steps
        if (get_event(&event)) {
            if (event == PILL_FALLING) {
                pill_fall = true;
            }
        }
        run_stepper_helper(status, prev);
        prev = gpio_get(OPTO_FORK);
    }
    return pill_fall;
}
