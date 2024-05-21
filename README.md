# HD44780
   Library to control max. 3x HD44780 display controller in 4-bit mode (parallel communications) using one 74HC595 shift register. This library uses the ShiftRegister library (https://github.com/mjklaren/ShiftRegister).
   
   Up to 3 display controllers can be addressed via the Q5 (Enable Display #0), Q6 (Enable Display #1) and Q7 (Enable Display #2)
   outputs of the shift register. The 4 data lines (D4-D7) of the display controllers are connected in parallel to the 
   4 data output lines (Q0-Q3) of the shift register.
   This library can control each display individually, in any combination or all at once. Displays are selected by using the
   'HD44780SetActiveDisplays' command. When multiple displays are controlled simultanously the data that is sent is identical for 
   these selected displays. If displays need to receive different data then they need to be addressed individually.
   The ShiftRegister.C library is used.

   See https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller and https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
   

   Wiring diagram:

   Rasperry            Shift register            Display controllers
   Pi Pico                (74HC595)               (max. 3x HD44780)            Comments
   ========            ==============            ===================           ========
                          VCC   (16) ---- +5V
   DATA_PIN  ------------ DS    (14)                                           Data
                          ~OE   (13) ---- Ground                               Enable (active low)
   CLOCK_PIN ------------ ST_CP (12)                                           Clock
   LATCH_PIN ------------ SH_CP (11)                                           Latch
                          ~MR   (10) ---- +5V
                          ~Q7   (9)                                            Not used 
                          GND   (8)  ---- Ground
                                                      VSS  (1) ---- Ground
                                                      VCC  (2) ---- +5V
                                                    VEE/V0 (3) ---- Pot.       Contrast control; 10K potentiometer to GND/+5V
                            Q4 (4)  ----------------- RS   (4)                 Register Select (0=command, 1=data)
                                                      RW   (5) ---- Ground     Read/write (0=write, 1=read); ground to fix on Write
                            Q5 (5)  ----------------- E #0 (6)                 Display #0 Clock enable (falling edge triggered)
                            Q6 (6)  ----------------- E #1 (6)                 Display #1 Clock enable (falling edge triggered)
                            Q7 (7)  ----------------- E #2 (6)                 Display #2 Clock enable (falling edge triggered)
                                                      D0   (7)                 Not used
                                                      D1   (8)                 Not used
                                                      D2   (9)                 Not used
                                                      D3   (10)                Not used
                            Q0 (15) ----------------- D4   (11)                Data (LSB), combined for all displays
                            Q1 (1)  ----------------- D5   (12)                Data, combined for all displays
                            Q2 (2)  ----------------- D6   (13)                Data, combined for all displays
                            Q3 (3)  ----------------- D7   (14)                Data (MSB), combined for all displays
                                                      LED+ (15) ---- +5V
                                                      LED- (16) ---- Ground

