/*
  Demo code for the HD44780 library. This code assumes the HD44780 is used with
  one or more 16x2 display (max. 3). See HD44780.c for the wiring diagram.

  Copyright (c) 2024 Maarten Klarenbeek (https://github.com/mjklaren)
  Distributed under the GPLv3 license  
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "HD44780.c"

// GPIOs
#define CLOCKGPIO    8
#define DATAOUTGPIO  7
#define LATCHGPIO    9


void main()
{
  // Configure I/O on the Pico.
  stdio_init_all();
  sleep_ms(5000);

  // Configure the displays. See definitions in HD44780.c.
  printf("Configuring the displays.\n");
  uint8_t FunctionSet=HD44780_DUTY2LINES|HD44780_5X8DOTS;                               
  uint8_t DisplayOnOffControl=HD44780_DISPLAYON|HD44780_CURSORON|HD44780_CURSORBLINKON; 
  uint8_t EntryModeSet=HD44780_NODISPLAYSHIFT;
  HD44780Display *Display=HD44780Init(CLOCKGPIO, DATAOUTGPIO, LATCHGPIO, FunctionSet, DisplayOnOffControl, EntryModeSet);

 // Endless loop
 while(1)
 {
    // Send a message to all displays.
    printf("Sending text to all displays.\n");
    HD44780SetActiveDisplays(Display, HD44780_ENABLEHIGHALL);                  // All displays active
    HD44780Command(Display, HD44780_DISPLAYONOFFCONTROL, HD44780_DISPLAYON|HD44780_CURSORON|HD44780_CURSORBLINKON);    // Display, cursor and cursor-blink on
    HD44780Write(Display, "All displays");
    HD44780Command(Display, HD44780_SETDDRAMADDRESS, 0x40);                    // 2nd line, first position.
    HD44780Write(Display, "the same text.");
    sleep_ms(1500);
    HD44780Command(Display, HD44780_CLEARDISPLAY, 0);                          // Clear display, cursor to 1st position
    HD44780Command(Display, HD44780_DISPLAYONOFFCONTROL, HD44780_DISPLAYON);   // Only display on; cursor remains off.


    // Now write a new message to all individual displays. Max. 3 displays are supported.
    // Screen #0
    printf("Sending text to display #0.\n");
    HD44780SetActiveDisplays(Display, HD44780_ENABLEHIGH0);
    HD44780Write(Display, "This text is on");    
    HD44780Command(Display, HD44780_SETDDRAMADDRESS, 0x40);
    HD44780Write(Display, "Display #0");  
    sleep_ms(1500);
    HD44780Command(Display, HD44780_CLEARDISPLAY, 0);

    // Screen #1
    printf("Sending text to display #1.\n");
    HD44780SetActiveDisplays(Display, HD44780_ENABLEHIGH1);
    HD44780Write(Display, "This text is on");    
    HD44780Command(Display, HD44780_SETDDRAMADDRESS, 0x40);
    HD44780Write(Display, "Display #1");   
    sleep_ms(1500);
    HD44780Command(Display, HD44780_CLEARDISPLAY, 0);

    // Screen #2
    printf("Sending text to display #2.\n");
    HD44780SetActiveDisplays(Display, HD44780_ENABLEHIGH2);
    HD44780Write(Display, "This text is on");    
    HD44780Command(Display, HD44780_SETDDRAMADDRESS, 0x40);
    HD44780Write(Display, "Display #2"); 
    sleep_ms(1500);
    HD44780Command(Display, HD44780_CLEARDISPLAY, 0);

    // Screen #0 and screen #1
    printf("Sending text to display #0 and #1.\n");
    HD44780SetActiveDisplays(Display, HD44780_ENABLEHIGH01);  // Both displays #0 and #1
    HD44780Write(Display, "This text is on");    
    HD44780Command(Display, HD44780_SETDDRAMADDRESS, 0x40);
    HD44780Write(Display, "Display #0 + #1");
    sleep_ms(1500);
    HD44780Command(Display, HD44780_CLEARDISPLAY, 0);  

    // Blink all displays
    printf("Doing some blinking.\n");
    HD44780SetActiveDisplays(Display, HD44780_ENABLEHIGHALL);
    HD44780Write(Display, "Hi there!");    
    for(uint8_t counter=0; counter <3; counter++)
    {
       HD44780Command(Display, HD44780_DISPLAYONOFFCONTROL, HD44780_OFF);        // Display and cursor are off.
       sleep_ms(500);
       HD44780Command(Display, HD44780_DISPLAYONOFFCONTROL, HD44780_DISPLAYON);  // Only display on, cursor remains off.
       sleep_ms(500);
    }
    HD44780Command(Display, HD44780_CLEARDISPLAY, 0);
 }
}

