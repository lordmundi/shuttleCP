
# Copyright 2013 Eric Messick (FixedImagePhoto.com/Contact)

# Change GPIO_SUPPORT to 0 to disable driving and reading of Raspi GPIO pins
#CFLAGS=-g -W -Wall -I /usr/local/include/nopoll -DGPIO_SUPPORT=1
CFLAGS=-O3 -W -Wall -I /usr/local/include/nopoll -DGPIO_SUPPORT=1

INSTALL_DIR=/usr/local/bin

OBJ=\
	shuttlecp.o\
	websocket.o\
	led_control.o\
	raspi_switches.o

all: shuttlecp

install: all
	install shuttle shuttlecp ${INSTALL_DIR}

shuttlecp: ${OBJ}
	gcc ${CFLAGS} ${OBJ} -o shuttlecp -lwiringPi -lnopoll -Wl,-rpath -Wl,/usr/local/lib

clean:
	rm -f shuttlecp keys.h $(OBJ)

shuttlecp.o: shuttle.h
led_control.o: led_control.h
raspi_switches.o: raspi_switches.h
websocket.o: websocket.h
