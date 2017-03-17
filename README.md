# Atmega16M1_CANDrv

A CAN Driver for the Atmega 16M1 this Driver supports standard 11Bit Identifier and Interrupt driven receiving.<br>
The driver Parts are:

* CANDrv.cpp
* CANDrv.h

The other files are a reference Project for the Driver and the provided Board,<br>
which reads out the Engine Temperature on my BMW E90.<br>
The Can Message 0x130 is sent by the car on the OBD CAN.<br>
This Message encodes the T15 Signal, which is not present on the OBD port of the E90.<br>
Therefore this Message is used to sent the Board to sleep.

# ATMEGA16M1Board

A small PCB with an Atmega 16M1 to test and use the Driver.<br>
The assembled Board looks like this:<br>
![picture of the Board in action](Schematics/KiCad/ATMEGA16M1Board/pictures/Board_V1%20(4).jpg)
![picture of the Board in action](Schematics/KiCad/ATMEGA16M1Board/pictures/Board_V1%20(5).jpg)
![picture of the Board in action](Schematics/KiCad/ATMEGA16M1Board/pictures/Board_V1%20(6).jpg)
The board in action can be seen here:<br>
![picture of the Board in action](Schematics/KiCad/ATMEGA16M1Board/pictures/Board_V1%20(1).jpg)
![picture of the Board in action](Schematics/KiCad/ATMEGA16M1Board/pictures/Board_V1%20(2).jpg)
![picture of the Board in action](Schematics/KiCad/ATMEGA16M1Board/pictures/Board_V1%20(3).jpg)

