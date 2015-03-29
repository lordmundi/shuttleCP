     +-----+-----+---------+------+--B Plus--+------+---------+-----+-----+
     | BCM | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | BCM |
     +-----+-----+---------+------+----++----+------+---------+-----+-----+
     |     |     |    3.3v |      |  1 || 2  |      | 5v      |     |     |
 L1  |   2 |   8 |   SDA.1 |  OUT |  3 || 4  |      | 5V      |     |     |
 L2  |   3 |   9 |   SCL.1 |  OUT |  5 || 6  |      | 0v      |     |     |
 L3  |   4 |   7 | GPIO. 7 |  OUT |  7 || 8  | OUT  | TxD     | 15  | 14  | L7
     |     |     |      0v |      |  9 || 10 | OUT  | RxD     | 16  | 15  | L8
 L4  |  17 |   0 | GPIO. 0 |  OUT | 11 || 12 | OUT  | GPIO. 1 | 1   | 18  | L9
 L5  |  27 |   2 | GPIO. 2 |  OUT | 13 || 14 |      | 0v      |     |     |
 L6  |  22 |   3 | GPIO. 3 |  OUT | 15 || 16 | OUT  | GPIO. 4 | 4   | 23  | L10
     |     |     |    3.3v |      | 17 || 18 | IN   | GPIO. 5 | 5   | 24  | S4
 S1  |  10 |  12 |    MOSI |   IN | 19 || 20 |      | 0v      |     |     |
 S2  |   9 |  13 |    MISO |   IN | 21 || 22 | OUT  | GPIO. 6 | 6   | 25  |
 S3  |  11 |  14 |    SCLK |   IN | 23 || 24 | OUT  | CE0     | 10  | 8   |
     |     |     |      0v |      | 25 || 26 | OUT  | CE1     | 11  | 7   |
     |   0 |  30 |   SDA.0 |  OUT | 27 || 28 | OUT  | SCL.0   | 31  | 1   |
     |   5 |  21 | GPIO.21 |  OUT | 29 || 30 |      | 0v      |     |     |
     |   6 |  22 | GPIO.22 |  OUT | 31 || 32 | OUT  | GPIO.26 | 26  | 12  |
     |  13 |  23 | GPIO.23 |  OUT | 33 || 34 |      | 0v      |     |     |
     |  19 |  24 | GPIO.24 |  OUT | 35 || 36 | OUT  | GPIO.27 | 27  | 16  |
     |  26 |  25 | GPIO.25 |  OUT | 37 || 38 | OUT  | GPIO.28 | 28  | 20  |
     |     |     |      0v |      | 39 || 40 | OUT  | GPIO.29 | 29  | 21  |
     +-----+-----+---------+------+----++----+------+---------+-----+-----+
     | BCM | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | BCM |
     +-----+-----+---------+------+--B Plus--+------+---------+-----+-----+


     The code uses the wiringPi numbers (wPi columns). The LEDs and switches
     are assigned as follows:

        L1  = X axis indicator LED
        L2  = Y axis indicator LED
        L3  = Z axis indicator LED
        L4  = A axis indicator LED
        L5  = Connected indicator LED
        L6  = Online indicator LED
        L7  = 1.0 rate indicator LED
        L8  = 0.1 rate indicator LED
        L9  = 0.01 rate indicator LED
        L10 = 0.001 rate indicator LED
        S1  = Feed Hold button
        S2  = Resume button
        S3  = Reset button
        S4  = Reconnect button
