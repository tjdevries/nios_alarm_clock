#include "altera_up_avalon_character_lcd.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <stdio.h>
#include "BSP/system.h"

// Personal includes
#include "hex.h"
#include "timers.h"
#include "util.h"


/*
 * Switches:
 *	SW17: Enables "test mode" - i.e. 1 minutes = 1 second
 *  SW16: Enables the 24 or AM/PM mode. 0 for AM/PM, 1 for 24 Hour.
 *	SW2 : Enables the alarm. If 0, alarm off. If 1, alarm on
 *	SW1 : Enables modification of the alarm time. If 0, alarm not modified (i.e. is set). If 1, alarm may be modified
 *	SW0 : Enables modification of the clock time. If 0, clock not modified (i.e. is set). If 0, clock may be modified
 *		Note: If both switches are on, neither will be modified.
 *
 * Keys:
 *	Key3: 
 *		- When SW1 xor SW0 are 1, then it will add 1 to the current hour of the (alarm / clock) time (respectively).
 *		- If an alarm goes off, then Key 3 snooze.
 *	Key2:
 * 		- When SW1 xor SW0 are 1, then it will add 1 to the current minute of the (alarm / clock) time (respectively). 
 *	Key1:
 *		- If alarm goes off, Key1 will turn the alarm off
 *
 * Hexes:
 *	If ! alarm:
 *		Displays the date: mm dd yyyy
 *	If alarm:
 *		Flash and do fun things
 */
 
// Initialize the LCD
alt_up_character_lcd_dev * char_lcd_dev;

// Initialize the buttons
/* Declare a global variable to hold the edge capture value. */
volatile int edge_capture;

// Initialize our context variable
volatile int hours = 0;
volatile int minutes = 0;
volatile int seconds = 0;
volatile int tenths = 0;

// Our half second counter
volatile int half_second = 0;

// The indexes used in the top_row to write the correct placement for the hours, mins and secs
int hours_1 = 4;
int hours_2 = 5;
int min_1 = 7;
int min_2 = 8;
int sec_1 = 10;
int sec_2 = 11;
// The tops and bottom rows of the display.
//					hh:mm:ss
char top_row[17] = "    12:00:00    ";
char bot_row[17];

// Variables for the date
int month = 12;
int day = 25;
int year = 2015;

/* Functions used for updating displays */

// For the Key 1 press
void reset_display() {
	// Clear the top row
	strcpy(top_row, "    12:00:00    ");
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
	alt_up_character_lcd_string(char_lcd_dev, top_row);
	
	// Clear the bottom row
	strcpy(bot_row, "                ");
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
	alt_up_character_lcd_string(char_lcd_dev, bot_row);
	
	// Reset our counters
	minutes = 0;
	seconds = 0;
	tenths = 0;
}

// For the Key 2 press
void write_current_time_to_bot_row() {
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
	alt_up_character_lcd_string(char_lcd_dev, top_row);
}


/*		Key Setups 		*/
static void handle_button_interrupts(void* context, alt_u32 id)
{
	/* cast the context pointer to an integer pointer. */
	volatile int* edge_capture_ptr = (volatile int*) context;
	/*
	* Read the edge capture register on the button PIO.
	* Store value.
	*/
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
	/* Write to the edge capture register to reset it. */
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0);
	/* reset interrupt capability for the Button PIO. */
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0xf);
}

/* Initialize the button_pio. */
static void init_button_pio()
{
	/* Recast the edge_capture pointer to match the
	alt_irq_register() function prototype. */
	void* edge_capture_ptr = (void*) &edge_capture;
	/* Enable all 4 button interrupts. */
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0xf);
	/* Reset the edge capture register. */
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0x0);
	/* Register the ISR. */
	alt_irq_register( 	KEY_IRQ,
						edge_capture_ptr,
						handle_button_interrupts );
}

/* Our function that handles the key presses */
void handle_key_press() {
	// Key 1
	if (edge_capture == 2) {
		reset_display();
	}
	// Key 2
	else if (edge_capture == 4) {
		write_current_time_to_bot_row();
	}
	// Key 3
	else if (edge_capture == 8) {
		half_second = 1;
		init_timer_1(&half_second);
	}
	
	// Reset our edge capture back to 0
	edge_capture = 0;
}

int main(void)
{
	// open the Character LCD port
	char_lcd_dev = alt_up_character_lcd_open_dev ("/dev/LCD");
	/* Initialize the character display */
	alt_up_character_lcd_init(char_lcd_dev);
	
	// Initialize the switches
	int * sw_ptr = (int *) SW_BASE;
	int sw_values;
	int oldvalue = 0x00000000;
	int MASK = 0x00020000;
	
	// Initialize the Timers
	init_timer_0(&tenths);
	
	// Tracker to see when the time changes
	int old_tenths = 0;
	
	// Initialize the KEY port
	init_button_pio();
	
	// continually 
	while(1)  {
		// check the state of the context integer updated by various ISR functions	
		// Act accordingly, which means
		
		// Handle if a key was pressed
		if (edge_capture) {
			handle_key_press();
		}
		
		// Flash on and off our displays
		if (half_second) {
			// Odd numbers
			if (half_second % 2) {
				// Turn hex on
				hex_on();
			}
			else {
				// Turn hex off
				hex_off();
			}
			
			if (half_second == 7) {
				half_second = 0;
				// Turn hex off
				hex_off();
				// Turn off timer.
				stop_timer_1();
			}
		}
		
		// Check SW17 for "Test Mode" - speed up or slow down
		sw_values = *(sw_ptr);
		if((sw_values & MASK) == 0x00020000 && oldvalue == 0x00000000){
			speed_up();
			oldvalue = sw_values & MASK;
		} else if ((sw_values & MASK) == 0x00000000 && oldvalue == 0x00020000) { 
			slow_down(); 
			oldvalue = sw_values & MASK;
		}
		
		// Update the clock
		if (tenths != old_tenths) {
			// Increment our seconds
			if (tenths >= 10) {
				hex_write_date(month, day, year);
				increment_date(&month, &day, &year);
				seconds++;
				if (seconds == 60) {
					minutes = (minutes + 1) % 60;
					seconds = 0;
					
					// Write our minutes out to the top row
					top_row[min_1] = '0' + (minutes - (minutes % 10)) / 10;
					top_row[min_2] = '0' + minutes % 10;
				}
				top_row[sec_1] = '0' + (seconds - (seconds % 10)) / 10;
				top_row[sec_2] = '0' + seconds % 10;
				tenths = 0;
			}
			old_tenths = tenths;
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
			alt_up_character_lcd_string(char_lcd_dev, top_row);
		}
		
	}
	
	return 0;
}
