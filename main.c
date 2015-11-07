#include "altera_up_avalon_character_lcd.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <stdio.h>
#include "BSP/system.h"

// Personal includes
#include "hex.h"
#include "timers.h"
#include "util.h"

// Initialize the LCD
alt_up_character_lcd_dev * char_lcd_dev;

// Initialize the buttons
/* Declare a global variable to hold the edge capture value. */
volatile int edge_capture;

// Initialize our context variable
volatile int minutes = 0;
volatile int seconds = 0;
volatile int tenths = 0;

// Our half second counter
volatile int half_second = 0;

// The tops and bottom rows of the display.
//					hh:mm:ss
char top_row[17] = "    12:00:00    ";
char bot_row[17];

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
		
		// Update the clock
		if (tenths != old_tenths) {
			// Increment our seconds
			if (tenths >= 10) {
				seconds++;
				if (seconds == 60) {
					minutes = (minutes + 1) % 60;
					seconds = 0;
					top_row[0] = '0' + (minutes - (minutes % 10)) / 10;
					top_row[1] = '0' + minutes % 10;
				}
				top_row[3] = '0' + (seconds - (seconds % 10)) / 10;
				top_row[4] = '0' + seconds % 10;
				tenths = 0;
			}
			old_tenths = tenths;
			top_row[6] = '0' + tenths;
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
			alt_up_character_lcd_string(char_lcd_dev, top_row);
		}
		
	}
	
	return 0;
}
