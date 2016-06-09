
#if GPIO_SUPPORT
#include "raspi_switches.h"
#include <wiringPi.h>


// initialize the switch states structure and the hardware
void initialize_raspi_switch_states( SWITCH_STATES *raspi_switches ) {
    raspi_switches->feed_hold                 = 1;
    raspi_switches->resume                    = 1;
    raspi_switches->reset                     = 1;
    raspi_switches->reconnect_requested       = 1;
    raspi_switches->prev_feed_hold            = 1;
    raspi_switches->prev_resume               = 1;
    raspi_switches->prev_reset                = 1;
    raspi_switches->prev_reconnect_requested  = 1;

    // also initilize the switch pins to be input
    pinMode( SWITCH_FEED_HOLD,    INPUT );
    pinMode( SWITCH_RESUME,       INPUT );
    pinMode( SWITCH_RESET,        INPUT );
    pinMode( SWITCH_RECONNECT,    INPUT );

    // set internal pull-up resistors to pull up
    pullUpDnControl ( SWITCH_FEED_HOLD,  PUD_UP );
    pullUpDnControl ( SWITCH_RESUME,     PUD_UP );
    pullUpDnControl ( SWITCH_RESET,      PUD_UP );
    pullUpDnControl ( SWITCH_RECONNECT,  PUD_UP );
    return;
}

// read the switch states into the data structure
void read_raspi_switches( SWITCH_STATES *raspi_switches ) {
    raspi_switches->prev_feed_hold            = raspi_switches->feed_hold;
    raspi_switches->prev_resume               = raspi_switches->resume;
    raspi_switches->prev_reset                = raspi_switches->reset;
    raspi_switches->prev_reconnect_requested  = raspi_switches->reconnect_requested;

    raspi_switches->feed_hold            = digitalRead( SWITCH_FEED_HOLD );
    raspi_switches->resume               = digitalRead( SWITCH_RESUME    );
    raspi_switches->reset                = digitalRead( SWITCH_RESET     );
    raspi_switches->reconnect_requested  = digitalRead( SWITCH_RECONNECT );
    return;
}

#endif

