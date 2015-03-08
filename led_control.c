
#include "led_control.h"
#include <wiringPi.h>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
#define LED 0


// initialize the led states structure and the hardware
void initialize_led_states( LED_STATES *states ) {
    states->online               = 1;
    states->websocket_connected  = 0;
    states->x_axis_active  = 1;
    states->y_axis_active  = 0;
    states->z_axis_active  = 0;
    states->a_axis_active  = 0;
    states->motion_speed_1  = 1;
    states->motion_speed_2  = 0;
    states->motion_speed_3  = 0;
    states->motion_speed_4  = 0;

    // also initilize the LED pins to be output
    wiringPiSetup();
    pinMode( LED_ONLINE,              OUTPUT );
    pinMode( LED_WEBSOCKET_CONNECTED, OUTPUT );
    pinMode( LED_X_AXIS_ACTIVE,       OUTPUT );
    pinMode( LED_Y_AXIS_ACTIVE,       OUTPUT );
    pinMode( LED_Z_AXIS_ACTIVE,       OUTPUT );
    pinMode( LED_A_AXIS_ACTIVE,       OUTPUT );
    pinMode( LED_MOTION_SPEED_1,      OUTPUT );
    pinMode( LED_MOTION_SPEED_2,      OUTPUT );
    pinMode( LED_MOTION_SPEED_3,      OUTPUT );
    pinMode( LED_MOTION_SPEED_4,      OUTPUT );
    return;
}


// update the led states structure to match current program state
void update_led_states( LED_STATES *states, short connected, ACTIVE_AXIS axis, ACTIVE_SPEED speed ) {
    states->online               = 1;
    states->websocket_connected  = (connected != 0);
    states->x_axis_active        = (axis == X_AXIS_ACTIVE);
    states->y_axis_active        = (axis == Y_AXIS_ACTIVE);
    states->z_axis_active        = (axis == Z_AXIS_ACTIVE);
    states->a_axis_active        = (axis == A_AXIS_ACTIVE);
    states->motion_speed_1       = (speed == MOTION_SPEED_1);
    states->motion_speed_2       = (speed == MOTION_SPEED_2);
    states->motion_speed_3       = (speed == MOTION_SPEED_3);
    states->motion_speed_4       = (speed == MOTION_SPEED_4);
}


// drive out LED states
void drive_leds( LED_STATES *states ) {

    digitalWrite( LED_ONLINE,              states->online );
    digitalWrite( LED_WEBSOCKET_CONNECTED, states->websocket_connected );
    digitalWrite( LED_X_AXIS_ACTIVE,       states->x_axis_active );
    digitalWrite( LED_Y_AXIS_ACTIVE,       states->y_axis_active );
    digitalWrite( LED_Z_AXIS_ACTIVE,       states->z_axis_active );
    digitalWrite( LED_A_AXIS_ACTIVE,       states->a_axis_active );
    digitalWrite( LED_MOTION_SPEED_1,      states->motion_speed_1 );
    digitalWrite( LED_MOTION_SPEED_2,      states->motion_speed_2 );
    digitalWrite( LED_MOTION_SPEED_3,      states->motion_speed_3 );
    digitalWrite( LED_MOTION_SPEED_4,      states->motion_speed_4 );
    return;
}


