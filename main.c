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
 *	SW0 : Enables modification of the clock time. If 0, clock not modified (i.e. is set). If 1, clock may be modified
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
volatile int hours = 11;
volatile int minutes = 40;
volatile int seconds = 0;
volatile int tenths = 0;

// Initialize the alarm variables
volatile int alarm_hours = 12;
volatile int alarm_minutes = 00;

// Our half second counter
volatile int half_second = 0;

// The tops and bottom rows of the display.
//					hh:mm:ss
char top_row[17] = "    12:00:00    ";
char bot_row[17] = "ALR 12:00:00    ";

// Variables for the date
int month = 12;
int day = 25;
int year = 2015;

// Variables for the AM_PM switch
int am_pm_mode = 1;		// If this is 1, that means that AM_PM mode is enabled.
						// If this is 0, then means that 24 hour mode is enabled.

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
void handle_key_press_time() {
	// Key 1
	if (edge_capture == 2) {
		increment_seconds(&top_row, &seconds);
		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
		alt_up_character_lcd_string(char_lcd_dev, top_row);
	}
	// Key 2
	else if (edge_capture == 4) {
		increment_minutes(&top_row, &minutes);
		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
		alt_up_character_lcd_string(char_lcd_dev, top_row);
	}
	// Key 3
	else if (edge_capture == 8) {
		increment_hours(&top_row, &hours);
		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
		alt_up_character_lcd_string(char_lcd_dev, top_row);
	}
	
	// Reset our edge capture back to 0
	edge_capture = 0;
}

void handle_key_press_alarm() {
	// Key 2
	if (edge_capture == 4) {
		increment_minutes(&bot_row, &alarm_minutes);
		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
		alt_up_character_lcd_string(char_lcd_dev, bot_row);
	}
	// Key 3
	else if (edge_capture == 8) {
		increment_hours(&bot_row, &alarm_hours);
		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
		alt_up_character_lcd_string(char_lcd_dev, bot_row);
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

	start(&top_row, seconds, minutes, hours);
	
	// Initialize the switches
	int * sw_ptr = (int *) SW_BASE;
	int sw_values;
	int oldvalue = 0x00000000;
	 // Masks for individual switches
	int MASK_17 = 0x00020000;
	int MASK_16 = 0x00010000;
	int MASK_1 = 0x00000002;
	int MASK_0 = 0x00000001;
	
	int is_fast = 0; //use to tell other function if sped up, 0 = slow, 1 = fast
	int clk_modify = 0; //if 0, clock isn't being changed, if 1 clock is being changed
	int alarm_modify = 0; //if 0 alarm isn't being changed, if 1, alarm is being changed
	
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
		
		// Update the switch_values
		sw_values = *(sw_ptr);
		if((sw_values & MASK_17) == 0x00020000 && oldvalue == 0x00000000){
			speed_up();
			oldvalue = sw_values & MASK_17;
			is_fast = 1;
		} else if ((sw_values & MASK_17) == 0x00000000 && oldvalue == 0x00020000) { 
			slow_down(); 
			oldvalue = sw_values & MASK_17;
			is_fast = 0;
		}
		
		//Allow user to change the time if SW0 is up
		if((sw_values & MASK_0) == 0x00000001){ clk_modify = 1;}
		else{ clk_modify = 0;}
		
		//buttons increment the hours, minutes, and seconds, respectively to Key3, Key2, and Key1
		if(clk_modify == 1 && alarm_modify == 0){
			// Handle if a key was pressed
			if (edge_capture) {
				handle_key_press_time();
			}
		}
		
		//Allow user to change the alarm if SW1 is up
		if((sw_values & MASK_1) == 0x00000002){
			alarm_modify = 1;
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
			alt_up_character_lcd_string(char_lcd_dev, bot_row);
		}
		else{ 
			alarm_modify = 0;
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
			alt_up_character_lcd_string(char_lcd_dev, "                ");
		}
		
		//buttons increment the hours, minutes, and seconds, respectively to Key3, Key2, and Key1
 		if(alarm_modify == 1 && clk_modify == 0){
			// Handle if a key was pressed
			if (edge_capture) {
				handle_key_press_alarm();
			}
		}

		// Check SW16 for "AM_PM" enable or "24" mode enable
		//		If the switch is enabled, then we turn on 24 hour mode
		//		Else we turn on AM / PM Mode
		// TODO: Optimize so that it doesn't assign something every loop cycle. Maybe we could slim it down
		if((sw_values & MASK_16) == MASK_16 ) {
			am_pm_mode = 0;
		}
		else {
			am_pm_mode = 1;
		}
		
		// Update the clock
		if (tenths != old_tenths) {
			// Call the util.h function to update the time
			update_time(top_row, &old_tenths, &tenths, &seconds, &minutes, &hours, &day, &month, &year, 0);

			// Write the updated time to the display
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
			alt_up_character_lcd_string(char_lcd_dev, top_row);
		}
		
	}
	
	return 0;
}
