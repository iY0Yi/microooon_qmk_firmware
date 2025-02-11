#include "quantum.h"
#include "report.h"
#include "pointing_device.h"

#define ACTIVATE_TRACKBALL

#ifdef ACTIVATE_TRACKBALL

#define DEFAULT_BLACKBERRY_TRACKBALL_CPI 100
#define PIN_NAME_UP D1
#define PIN_NAME_DOWN D0
#define PIN_NAME_LEFT D4
#define PIN_NAME_RIGHT C6
#define PIN_NAME_BTN D7

#define DIRECTION_X 0
#define DIRECTION_Y 1

#define BTN_PIN_MOUSE_BTN_INDEX POINTING_DEVICE_BUTTON1

uint16_t blackberry_mouse_cpi;

int trackball_pins[2][2] = {
    {PIN_NAME_LEFT, PIN_NAME_RIGHT},
    {PIN_NAME_UP, PIN_NAME_DOWN}
};

int x_actions[2],y_actions[2];
int x_last_actions[2],y_last_actions[2];
int xmove,ymove;

int read_action(int direction) {
    int *action_array;
    int *last_action_array;

    if(direction == DIRECTION_X){
        action_array = x_actions;
        last_action_array = x_last_actions;
    }else if(direction == DIRECTION_Y){
        action_array = y_actions;
        last_action_array = y_last_actions;
    }else{
        return 0;
    }

    for(int i = 0; i < 2; ++i) {
        action_array[i] = readPin(trackball_pins[direction][i]);

        if(action_array[i] != last_action_array[i]) {
            last_action_array[i] = action_array[i];

            if(i == 0) {
                return (-1) * abs(blackberry_mouse_cpi/10);
            } else {
                return abs(blackberry_mouse_cpi/10);
            }
        }
    }
    return 0;
}

static inline void setPinInput_atomic(pin_t pin) {
    ATOMIC_BLOCK_FORCEON { setPinInput(pin); }
}

void pointing_device_driver_init(void) {
    //trackball pin setup
    for(int i=0;i < 2;i++){
        for(int j=0;j < 2;j++){
            setPinInput_atomic(trackball_pins[i][j]);
        }
    }
    //button pin setup
    setPinInput_atomic(PIN_NAME_BTN);

    //initialize
    for(int i=0;i<2;i++){
        x_actions[i] = 0;
        y_actions[i] = 0;
        x_last_actions[i] = 0;
        y_last_actions[i] = 0;
    }


    blackberry_mouse_cpi = DEFAULT_BLACKBERRY_TRACKBALL_CPI;
}

// report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {

//     xmove = read_action(DIRECTION_X);
//     ymove = read_action(DIRECTION_Y);

//     mouse_report.x = xmove;
//     mouse_report.y = ymove;

//     #ifdef CONSOLE_ENABLE
//     uprintf("Mouse pressed: %s\n", !((bool)(readPin(PIN_NAME_BTN))?"true":"false"));
//     uprintf("Raw  X: %d, Y: %d\n", xmove, ymove);
//     #endif

//     mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons,!((bool)(readPin(PIN_NAME_BTN))),POINTING_DEVICE_BUTTON1);

//     return mouse_report;
// }

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {

    xmove = read_action(DIRECTION_X);
    ymove = read_action(DIRECTION_Y);

    mouse_report.x = xmove;
    mouse_report.y = ymove;

    // トラックボールのボタンが押されたとき
    if (!readPin(PIN_NAME_BTN)) {
        tap_code16(LALT(KC_K));  // Alt + K を送信
    }

    return mouse_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return blackberry_mouse_cpi;
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    blackberry_mouse_cpi = cpi;
}

# endif