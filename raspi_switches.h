#ifndef RASPI_SWITCHES_H
#define RASPI_SWITCHES_H


typedef struct {
    short int feed_hold;
    short int resume;
    short int reset;
    short int reconnect_requested;
    short int prev_feed_hold;
    short int prev_resume;
    short int prev_reset;
    short int prev_reconnect_requested;
} SWITCH_STATES;

#define SWITCH_FEED_HOLD            12
#define SWITCH_RESUME               13
#define SWITCH_RESET                14
#define SWITCH_RECONNECT            5


void initialize_raspi_switch_states( SWITCH_STATES *raspi_switches );
void read_raspi_switches( SWITCH_STATES *switch_states );

#endif   /* RASPI_SWITCHES_H - do not put anything below this line! */
