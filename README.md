# Arduino Chair

This project contains the Arduino controller code to modify a [Hot Wheels Wild Thing](https://fisher-price.mattel.com/shop/en-us/fp/power-wheels/power-wheels-wild-thing-orange-dfv03) ride-on vehicle for children with mobility needs.

**DISCLAIMER: The author of this code is not responsible for any injuries to any person or damages to any object including the car caused by the modifications. Any type of modification will also void the warranty provided by the manufacturer of the car.**

Original credits for this project go to [FRC Team 1939](https://www.frcteam1939.com) who outlined [how to do the modification](https://www.instructables.com/id/Wild-Thing-Modification/). This project code follows this approach. However for this project, the Arduino controller code was modified to suit our needs.

## Modifications from FRC Team 1939 Code

The original codebase can be found here: [FIRST1939/GoBabyGo](https://github.com/FIRST1939/GoBabyGo). This codebase includes the following modifications.

* Inclusion of the Cytron_PS2Shield C++ code and header files for convenience.
* Introduction of diagonal joystick orientiation support.
* Removal of servo steering option and rewrite of motor control for better ramping support
* Introduction to disable backward motion for child
* Turn speed factor and minimum
* Reverse speed minimum
* Parent override implemented with wireless PS2 controller and Cytron PS2_Shield.

## Parts List

TBD

## Circuit Layout

TBD

## Customization Options

TBD
