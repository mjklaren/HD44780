/*
   Library to control max. 3x HD44780 display controller in 4-bit mode (parallel communications) using one 74HC595 shift register.
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

   Copyright (c) 2024 Maarten Klarenbeek (https://github.com/mjklaren)
   Distributed under the GPLv3 license                                                   
   
*/


#ifndef MyHardwareHD44780
#define MyHardwareHD44780


#include "ShiftRegister.c"

#define HD44780_DEFAULT                 0
#define HD44780_DELAY                   50   // Delay for shift registers, as the HD44780 is a bit slower (in usec).
#define HD44780_RSCOMMAND               0
#define HD44780_RSDATA                  16
#define HD44780_PROCESSING_DELAY        2    // Processing delay for certain commands (in msec).

// Select which displays are communicated with; values for all combinations for display #0 to #2. For use in ´HD44780SetActiveDisplays´.
#define HD44780_ENABLEHIGH0             32   // Only display #0
#define HD44780_ENABLEHIGH01            96   // Display #0 and display #1
#define HD44780_ENABLEHIGH02            160  // Display #0 and display #2
#define HD44780_ENABLEHIGH1             64   // Only display #1
#define HD44780_ENABLEHIGH12            192  // Display #1 and display #2
#define HD44780_ENABLEHIGH2             128  // Only display 2
#define HD44780_ENABLEHIGHALL           224  // All displays
#define HD44780_ENABLELOW               0     

// Available instructions - see also https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
#define HD44780_CLEARDISPLAY            1
#define HD44780_CURSORHOME              2
#define HD44780_ENTRYMODESET            4
#define HD44780_DISPLAYONOFFCONTROL     8
#define HD44780_CURSORDISPLAYSHIFT      16   // Includes commands for the cursor
#define HD44780_FUNCTIONSET             32
#define HD44780_SETCGRAMADDRESS         64
#define HD44780_SETDDRAMADDRESS         128

// Parameters for the instructions above
// Entry mode set instruction
#define HD44780_DECREMENTCURSORPOSITION 0
#define HD44780_INCREMENTCURSORPOSITION 2
#define HD44780_NODISPLAYSHIFT          0
#define HD44780_DISPLAYSHIFT            1

// Display on/off instruction
#define HD44780_OFF                     0    // Display, cursor and cursor blink off.
#define HD44780_DISPLAYON               4
#define HD44780_CURSORON                2
#define HD44780_CURSORBLINKON           1

// Cursor/display shift instruction
#define HD44780_MOVECURSOR              0
#define HD44780_MOVEDISPLAY             8
#define HD44780_SHIFTLEFT               0
#define HD44780_SHIFTRIGHT              4

// Function set instruction
#define HD44780_4BITINTERFACE           0
#define HD44780_8BITINTERFACE           16   // Only to be used to initialize the controller
#define HD44780_DUTY1LINE               0    // 1/8 or 1/11 duty (1 line)
#define HD44780_DUTY2LINES              8    // 1/16 duty (2 lines); requires 5x8 dots font
#define HD44780_5X8DOTS                 0
#define HD44780_5X10DOTS                4    // Can only be used in combination with HD44780_DUTY1LINE



// Struct to manage the communications with the attached displays.
typedef struct 
{
  // Pointer to the shift register struct
  ShiftRegister *Register;

  // Initial configuration of the HD44780
  uint8_t FunctionSet, DisplayOnOffControl, EntryModeSet;

  // Active displays
  uint8_t ActiveDisplays;
} HD44780Display;


/*
  Write a half-byte ('nibble´) to the active displays. As the HD44780 is connected using 4 datalines we need to send 
  each byte in 2 parts.
*/
void HD44780WriteNibble(HD44780Display *Display, bool IsCommand, uint8_t Nibble)
{
  Nibble&=15;   // Only lowest 4 bits are to be used.

  // Write the nibble and set E-bit high for all active displays.
  Display->Register->OutputBuffer=(IsCommand?HD44780_RSCOMMAND:HD44780_RSDATA)|Display->ActiveDisplays|Nibble;
  ShiftRegisterUpdate(Display->Register);

  // Set E-bit to low. The HD44780 is triggered by falling edge of the E-line.
  Display->Register->OutputBuffer=(IsCommand?HD44780_RSCOMMAND:HD44780_RSDATA)|HD44780_ENABLELOW|Nibble;
  ShiftRegisterUpdate(Display->Register);
}


// Write a byte to the active displays.
void HD44780WriteByte(HD44780Display *Display, bool IsCommand, uint8_t Byte)
{
  // We're in 4-bit mode; write the 4 MSB bits first, then the LSBs.
  HD44780WriteNibble(Display,IsCommand,(Byte>>4));     // Last 4 bits (MSB)
  HD44780WriteNibble(Display,IsCommand,(Byte&15));     // First 4 bits (LSB)
}


// Write a text to the active displays, one byte at a time.
void HD44780Write(HD44780Display *Display, char *Text)
{
  // Run through the provided string and write to the controller.
  while (*Text)
  {
    HD44780WriteByte(Display,false,*Text);
    Text++;
  }
}


/* 
  Send a command + parameter to the display controller. When no parameter is needed use the ´0' value.
  For a full list of commands and their use see https://www.sparkfun.com/datasheets/LCD/HD44780.pdf. Macro's for the 
  available commands and their parameters are listed above.
*/
void HD44780Command(HD44780Display *Display, uint8_t Command, uint8_t Parameters)
{
  HD44780WriteByte(Display,true,Command|Parameters);

  // Some commands require additional processing delay.
  if(Command==HD44780_CLEARDISPLAY || Command==HD44780_CURSORHOME)
    sleep_ms(HD44780_PROCESSING_DELAY);
}


// Reset the displays; set to 4 bit mode and configure them.
void HD44780Reset(HD44780Display *Display)
{
  // Reset sequence, set the controller to 4-bits mode. Shift the bits 4 positions to the right as we're writing the nibble directly.
  for(int counter=0;counter<3;counter++)
    HD44780WriteNibble(Display,true,(HD44780_FUNCTIONSET|HD44780_8BITINTERFACE)>>4);
  HD44780WriteNibble(Display,true,(HD44780_FUNCTIONSET|HD44780_4BITINTERFACE)>>4);  

  // Now configure the device.
  HD44780Command(Display,HD44780_FUNCTIONSET,Display->FunctionSet);
  HD44780Command(Display,HD44780_DISPLAYONOFFCONTROL,Display->DisplayOnOffControl); 
  HD44780Command(Display,HD44780_ENTRYMODESET,Display->EntryModeSet); 
  HD44780Command(Display,HD44780_CLEARDISPLAY,0); // Clear display
  HD44780Command(Display,HD44780_CURSORHOME,0); // Set cursor to home position
}


// Select which displays are 'active' (=which are being communicated with).
void HD44780SetActiveDisplays(HD44780Display *Display, uint8_t ActiveDisplays)
{
  Display->ActiveDisplays=ActiveDisplays;
}


/*
  Initialize all connected displays and return the pointer to the struct that is used to manage the communication. 
  During the initialization all displays are addressed (HD44780_ENABLEHIGHALL), after which display #0 is selected as 
  the active display. 
*/
HD44780Display *HD44780Init(uint8_t ClockGPIO, uint8_t DataOutGPIO, uint8_t LatchGPIO, uint8_t FunctionSet, uint8_t DisplayOnOffControl, uint8_t EntryModeSet)
{
  // Create the struct needed for managing the HD44780
  HD44780Display *Display=malloc(sizeof(HD44780Display));
  Display->Register=ShiftRegisterCreate(SHIFTREGISTER_OUTPUT,ClockGPIO,0,DataOutGPIO,LatchGPIO,0,1);

  // The shift register needs to switch slower for the HD44780 to keep up.
  Display->Register->ClockDelayUS=HD44780_DELAY;
  Display->Register->LatchDelayUS=HD44780_DELAY;

  // Copy the initial settings of the HD4780 in the struct and initialize the display.
  Display->FunctionSet=FunctionSet|HD44780_4BITINTERFACE;
  Display->DisplayOnOffControl=DisplayOnOffControl;
  Display->EntryModeSet=EntryModeSet;
  Display->ActiveDisplays=HD44780_ENABLEHIGHALL;  // Initialize all displays
  HD44780Reset(Display);

  // Make display #0 the active display.
  Display->ActiveDisplays=HD44780_ENABLEHIGH0;

  // Return the pointer to the shift register that controls communication to this display.
  return(Display);
}

#endif
