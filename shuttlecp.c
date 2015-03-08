
/*

 Jog Dial for ChiliPeppr

 This code is meant to allow button, jog, and shuttle events flow
 to the Serial Port JSON server typically used with ChiliPeppr 
 driven CNC machines.

 Interface to Shuttle Contour Xpress based on "Contour ShuttlePro
 v2 interface" by Eric Messick.

*/

#include "shuttle.h"
#include "led_control.h"
#include "websocket.h"

#define SPJS_HOST     "localhost"        // Hostname where SPJS is running
#define SPJS_PORT     "8989"             // Port for SPJS
#define CYCLE_TIME_MICROSECONDS 100000   // time of each main loop iteration
#define MAX_FEED_RATE 1500.0   // (unit per minute - initially tested with millimeters)
#define OVERSHOOT     1.06     // amount of overshoot for shuttle wheel

// Each press of the increment button will toggle through 4 speed / distance
// increments. Use the defines below to adjust the distance moved by 
// each increment of the jog dial.
// These line up with MOTION_SPEED_1, MOTION_SPEED_2, etc.
#define INCREMENT1 0.001
#define INCREMENT2 0.01
#define INCREMENT3 0.1
#define INCREMENT4 1.0

typedef struct input_event EV;
unsigned short jogvalue = 0xffff;
int            shuttlevalue = 0xffff;
struct timeval last_shuttle;
int            need_synthetic_shuttle;

LED_STATES   led_states;
short int    websocket_connected = 0;
ACTIVE_AXIS  active_axis         = X_AXIS_ACTIVE;
ACTIVE_SPEED active_speed        = MOTION_SPEED_4;
Queue        cmd_queue;
char         lastcmd[MAX_CMD_LENGTH];
short int    continuously_send_last_command;


// A helper procedure to return the character used for each axis
// and the current speed/increment level.
void get_axis_and_speed( char* axis, float* speed ) {
    switch (active_axis) {
        case X_AXIS_ACTIVE: *axis = 'X'; break;
        case Y_AXIS_ACTIVE: *axis = 'Y'; break;
        case Z_AXIS_ACTIVE: *axis = 'Z'; break;
        case A_AXIS_ACTIVE: *axis = 'A'; break;
        default:            *axis = 'X'; break;
    }
    switch (active_speed) {
        case MOTION_SPEED_1: *speed = INCREMENT1; break;
        case MOTION_SPEED_2: *speed = INCREMENT2; break;
        case MOTION_SPEED_3: *speed = INCREMENT3; break;
        case MOTION_SPEED_4: *speed = INCREMENT4; break;
        default:             *speed = INCREMENT2; break;
    }
}


// Main event procedure whenever a button is pressed.
void key(unsigned short code, unsigned int value)
{
    char axis;
    float speed;
    char cmd[MAX_CMD_LENGTH];

    // Only work on value == 1, which is the button down event
    if (value == 1) {
        switch (code) {
            case X_AXIS_BUTTON: active_axis = X_AXIS_ACTIVE; break;
            case Y_AXIS_BUTTON: active_axis = Y_AXIS_ACTIVE; break;
            case Z_AXIS_BUTTON: active_axis = Z_AXIS_ACTIVE; break;
            case A_AXIS_BUTTON: active_axis = A_AXIS_ACTIVE; break;
            case INCREMENT_BUTTON: {
                active_speed = (active_speed + 1) % NUM_MOTION_SPEEDS;
                break;
            }
            default:
                fprintf(stderr, "key(%d, %d) out of range\n", code, value);
                break;
        }
        get_axis_and_speed( &axis, &speed );
        snprintf( cmd, MAX_CMD_LENGTH, "(commanding the %c axis at %.3f speed)\n", axis, speed );
        cmd[MAX_CMD_LENGTH-1] = '\0';
        cmd_queue.push( &cmd_queue, cmd );
        fprintf(stderr, "%s", cmd);
    }
}


// Main event procedure whenever shuttle wheel is turned.
void shuttle(int value)
{
    char cmd[MAX_CMD_LENGTH];
    char axis;
    float speed;
    float distance;
    int direction;

    if (value < -7 || value > 7) {
        fprintf(stderr, "shuttle(%d) out of range\n", value);
    } else {
        fprintf(stderr, "Received shuttle command for value %d ???\n", value);
        direction = (value >= 0) ? 1 : -1;
        gettimeofday(&last_shuttle, 0);
        need_synthetic_shuttle = value != 0;
        if( value != shuttlevalue ) {
            shuttlevalue = value;
        }

        // if we got a shuttle of zero (0), this is our indication to 
        // stop streaming commands. Since there is a bug and sometimes
        // the shuttle doesn't send the event for zero, we actually 
        // stop on 0 or 1.
        cmd_queue.clear(&cmd_queue);  // when we are shuttling, never queue commands
        if ((value == 0) || (value == 1) || (value == -1)) {
            continuously_send_last_command = 0;

            // Sending the wipe (%) command to GRBL doesn't really work, but this
            // should help for TinyG.  In reality, for TinyG we should really send
            // a feed hold, then a wipe, then a resume.  Hopefully someone can 
            // implement and test this on a TinyG.  TODO
            snprintf( cmd, MAX_CMD_LENGTH, "send /dev/ttyACM0 %%\n" );
            cmd_queue.push( &cmd_queue, cmd );
        } else {
            continuously_send_last_command = 1;
            get_axis_and_speed( &axis, &speed );

            // We calculate how far we want to move based on our current increment
            // setting and our current cycle time.  That way, we know roughly how
            // far the machine will move and can have the next command queued up
            // just before it starts to decelerate, which is why we have the
            // overshoot factor.
            speed = speed * direction * value * (MAX_FEED_RATE / (7.0*INCREMENT4));
            distance = (speed/60.0) * (CYCLE_TIME_MICROSECONDS * OVERSHOOT / 1000000.0) * direction;
            snprintf( cmd, MAX_CMD_LENGTH, "send /dev/ttyACM0 G91 G1 F%.3f %c%.3f\nG90\n", speed, axis, distance );
            strncpy( lastcmd, cmd, MAX_CMD_LENGTH ); 
            cmd[MAX_CMD_LENGTH-1] = lastcmd[MAX_CMD_LENGTH-1] = '\0';
            cmd_queue.push( &cmd_queue, cmd );
        }
    }
}

// Due to a bug (?) in the way Linux HID handles the ShuttlePro, the
// center position is not reported for the shuttle wheel.  Instead,
// a jog event is generated immediately when it returns.  We check to
// see if the time since the last shuttle was more than a few ms ago
// and generate a shuttle of 0 if so.
//
// Note, this fails if jogvalue happens to be 0, as we don't see that
// event either!
void jog(unsigned int value)
{
    int direction;
    struct timeval now;
    struct timeval delta;
    char axis;
    float distance;
    char cmd[MAX_CMD_LENGTH];

    // I think the reason we want to skip the very first jog is
    // because we can't calculate direction until we get 2 jog
    // events. --FG
    if ((jogvalue != 0xffff) && (jogvalue != value)) {
        direction = ((value - jogvalue) & 0x80) ? -1 : 1;
        get_axis_and_speed( &axis, &distance );
        distance *= direction;
        snprintf( cmd, MAX_CMD_LENGTH, "send /dev/ttyACM0 G91 G0 %c%.3f\nG90\n", axis, distance );
        cmd[MAX_CMD_LENGTH-1] = '\0';
        cmd_queue.push( &cmd_queue, cmd );
        continuously_send_last_command = 0;
    }
    jogvalue = value;

    // We should generate a synthetic event for the shuttle going
    // to the home position if we have not seen one recently
    if (need_synthetic_shuttle) {
        gettimeofday( &now, 0 );
        timersub( &now, &last_shuttle, &delta );

        if (delta.tv_sec >= 1 || delta.tv_usec >= 5000) {
            shuttle(0);
            need_synthetic_shuttle = 0;
        }
    }

    if (jogvalue != 0xffff) {
        value = value & 0xff;
        direction = ((value - jogvalue) & 0x80) ? -1 : 1;
        while (jogvalue != value) {
            // driver fails to send an event when jogvalue == 0
            jogvalue = (jogvalue + direction) & 0xff;
        }
    }
    jogvalue = value;
}

// Handler for jog and shuttle events, which calls the appropriate
// function for each.
void jogshuttle(unsigned short code, unsigned int value)
{
    switch (code) {
        case EVENT_CODE_JOG:
            jog(value);
            break;
        case EVENT_CODE_SHUTTLE:
            shuttle(value);
            break;
        default:
            fprintf(stderr, "jogshuttle(%d, %d) invalid code\n", code, value);
            break;
    }
}


// Toplevel event handler
void handle_event(EV ev)
{
    switch (ev.type) {
        case EVENT_TYPE_DONE:
        case EVENT_TYPE_ACTIVE_KEY:
            break;
        case EVENT_TYPE_KEY:
            key(ev.code, ev.value);
            break;
        case EVENT_TYPE_JOGSHUTTLE:
            jogshuttle(ev.code, ev.value);
            break;
        default:
            fprintf(stderr, "handle_event() invalid type code\n");
            break;
    }
}


int
main(int argc, char **argv)
{
    EV ev;
    int nread;
    char *dev_name;
    int fd, num_cmds_in_queue, num_cmds_sent;
    int first_time = 1;
    struct timeval time_start;
    struct timeval time_end;
    struct timeval time_taken;
    struct timeval time_to_sleep;
    struct timeval cycle_time_us;
    struct timeval select_timeout;
    unsigned int sleep_us;
    fd_set set;
    char host[256];
    char port[16];

    select_timeout.tv_sec = 0;
    select_timeout.tv_usec = 0;

    cycle_time_us.tv_sec = 0;
    cycle_time_us.tv_usec = CYCLE_TIME_MICROSECONDS;

    if (argc != 2) {
        fprintf(stderr, "usage: shuttlecp <device>\n" );
        exit(1);
    }

    dev_name = argv[1];

    // initialize LEDs
    initialize_led_states( &led_states );

    cmd_queue = createQueue();

    while (1) {

        // initialize - open websocket and open device
        snprintf(host, sizeof(host), SPJS_HOST);
        snprintf(port, sizeof(port), SPJS_PORT);
        while ( websocket_init( host, port ) ) {
            fprintf(stderr, "Attempting connection to %s:%s\n", host, port);
            usleep(1000000);
        }
        websocket_connected = 1;
        fprintf(stderr, "Connected.\n");

        // Open the connection to the device
        fd = open(dev_name, O_RDONLY);
        if (fd < 0) {
            perror(dev_name);
            if (first_time) {
                exit(1);
            }
        } else {
            // Flag it as exclusive access
            if(ioctl( fd, EVIOCGRAB, 1 ) < 0) {
                perror( "evgrab ioctl" );
            } else {

                // The main loop we operate in
                first_time = 0;
                while (1) {

                    // if we have lost connection to the websocket, then break
                    // the loop so we can reinitialize everything.
                    // TODO: need to also add in a kill flag to allow a button
                    // to force us to reinit and reconnect.
                    if (!websocket_connected) {
                        fprintf(stderr, "============ Reinitializing jog dial interface\n");
                        cmd_queue.clear( &cmd_queue );
                        continuously_send_last_command = 0;
                        break;
                    }

                    gettimeofday( &time_start, 0 );

                    // We are going to just select on the FD to see if a read
                    // would produce any data.  If not, we skip the read.
                    FD_ZERO(&set);
                    FD_SET(fd, &set);

                    while (select(fd+1, &set, NULL, NULL, &select_timeout)) {
                        // read jog controller events
                        nread = read(fd, &ev, sizeof(ev));
                        if (nread == sizeof(ev)) {
                            handle_event(ev);
                        } else {
                            if (nread < 0) {
                                perror("read event");
                                break;
                            } else {
                                fprintf(stderr, "short read: %d\n", nread);
                                break;
                            }
                        }
                    }

                    // TODO: read raspberry pi buttons

                    // send all queued commands over websocket
                    if (websocket_connected) {
                        num_cmds_in_queue = cmd_queue.size;
                        num_cmds_sent = websocket_send_cmds( &cmd_queue );
                        if (num_cmds_sent != num_cmds_in_queue) {
                            websocket_connected = 0;
                        }

                        // If we should be continuously sending a cmd, enqueue it here
                        if ( continuously_send_last_command && websocket_connected ) {
                            cmd_queue.push( &cmd_queue, lastcmd );
                        }
                    }

                    // update LEDs
                    update_led_states( &led_states, websocket_connected, active_axis, active_speed );
                    drive_leds( &led_states );

                    // sleep until next cycle
                    gettimeofday( &time_end, 0 );
                    timersub( &time_end, &time_start, &time_taken );
                    timersub( &cycle_time_us, &time_taken, &time_to_sleep );
                    if (time_to_sleep.tv_sec < 0) {
                        sleep_us = 0;
                    } else {
                        sleep_us = time_to_sleep.tv_usec;
                        usleep( sleep_us );
                    }
                }
            }
        }
        close(fd);
        sleep(1);
    }
}
