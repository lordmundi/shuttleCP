
# ShuttleCP: Jog Dial for ChiliPeppr

This code is meant to allow button, jog, and shuttle events from a Contour ShuttleXpress controller to flow to CNC machines
The original branch of this code was developed for communication with the Serial Port JSON server typically used with ChiliPeppr 
driven CNC machines.  The latest version adds communication to CNC machines driven with bCNC.

Interface to Shuttle Contour Xpress based on "Contour ShuttlePro v2 interface" by Eric Messick.

## Configuration, Building, and Running
See README file.  See README_pinout.txt for hardware hookup information.

## Warning:
This has only really been tested with GRBL and with a machine running in metric units.  If you set your machine to inches, my guess is that it will move much further than you intend it to.  If you are testing this utility out with your machine for the first time, make sure you are near an E-stop for your machine so you don't run into issues with it banging past its hard limits!

## Video
To get a rough idea of what this does, see the following video:  https://www.youtube.com/watch?v=7LP4he22kKk
