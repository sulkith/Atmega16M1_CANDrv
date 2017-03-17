# Errata to the Schematics

Schematics from commit adee088dd9ad20461ed7a09ecef526a60e1ee473
* Important fix:<br>
  connect +5VPin from Display IC3-PIN3 to +5VPin from FTDI IC2-PIN3.<br>
  no fixed board yet
* other fixes: <br>
  for Display Operation connect R4 to Display CS IC3-PIN7.<br>
  no fixed board yet
* for Wakeup: <br>
  remove Q4 and R10. Connect RXCAN of MCP2551 U2-PIN4 to PWM1A1.<br>
  no fixed schematics yet<br>
  no fixed board yet<br>
  
The fixes are marked on the Pictures below:
![Front](pictures/Board_V1_Errata_1.jpg)
![Back](pictures/Board_V1_Errata_2.jpg)
