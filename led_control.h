#ifndef LED_CONTROL_H
#define LED_CONTROL_H


#include "shuttle.h"

typedef struct {
    short int online;
    short int websocket_connected;
    short int x_axis_active;
    short int y_axis_active;
    short int z_axis_active;
    short int a_axis_active;
    short int motion_speed_1;
    short int motion_speed_2;
    short int motion_speed_3;
    short int motion_speed_4;
} LED_STATES;


#define LED_ONLINE               3
#define LED_WEBSOCKET_CONNECTED  2
#define LED_X_AXIS_ACTIVE        8
#define LED_Y_AXIS_ACTIVE        9
#define LED_Z_AXIS_ACTIVE        7
#define LED_A_AXIS_ACTIVE        0
#define LED_MOTION_SPEED_1       4
#define LED_MOTION_SPEED_2       1
#define LED_MOTION_SPEED_3       16
#define LED_MOTION_SPEED_4       15



void initialize_led_states( LED_STATES *states );
void update_led_states( LED_STATES *states, short connected, ACTIVE_AXIS axis, ACTIVE_SPEED speed );
void drive_leds( LED_STATES *states );

#endif   /* LED_CONTROL_H - do not put anything below this line! */
