This keypad library is a modified version of the https://www.exploreembedded.com/wiki/AVR_C_Library keypad library. 
It has been modified to suit the keypad found in the Elegoo the most complete starter kit. The PORT has been changed set to PORTK.
The wiring used is provided. 

To modify the KEYPAD_GetKey() to only read the current state of the keypad (i.e. the pressed button) to the following:
- go to keypad.c
- comment out the functions calls  "KEYPAD_WaitForKeyRelease();" and "KEYPAD_WaitForKeyPress();"
- now the function will NOT wait that the previous button is released and that the key is pressed again before reading the keypad.
- the function returns the button pressed or 'z' for any state that is not specified.