# HD44780
C-library to control max. 3 HD44780 display controllers in 4-bit mode (parallel communications) using one 74HC595 shift register. Developed on a Raspberry Pi Pico, but should be usable on other microcontrollers. This library uses the ShiftRegister library (https://github.com/mjklaren/ShiftRegister).
   
Up to 3 display controllers can be addressed using only one shiftregister via the Q5 (Enable Display #0), Q6 (Enable Display #1) and Q7 (Enable Display #2) outputs of the shiftregister. The 4 data lines (D4-D7) of each display controller are to be connected in parallel to the 4 data output lines (Q0-Q3) of the shiftregister. Check the wiring diagram below.
   
<img width="940" alt="Wiring diagram" src="https://github.com/mjklaren/HD44780/assets/127024801/01db77ee-a291-44d5-832e-2ff3b92a1333">
   
This library can control each display individually, in any combination or all at once. Displays are selected by using the 'HD44780SetActiveDisplays' command. When multiple displays are controlled simultanously the data that is sent is identical for these selected displays. If displays need to receive different data then they need to be addressed separately. The ShiftRegister.C library is used.

See the file "HD44780_demo.c" for some example code.

![HD44780_demo](https://github.com/user-attachments/assets/468772e8-1e26-4249-899e-d1ef2dbcdb24)

(two displays connected; the demo supports three)

See https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller and https://www.sparkfun.com/datasheets/LCD/HD44780.pdf for more technical details.

The wiring diagram is also provided in the sourcecode.

This sourcecode is distributed under the GPLv3 license.

   
