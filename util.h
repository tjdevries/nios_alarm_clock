#include "altera_up_avalon_character_lcd.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <stdio.h>
#include "BSP/system.h"

// Personal includes
#include "hex.h"
#include "timers.h"

void write_time_to_buffer(char *time, int second, int minute, int hour);
void update_time(char * top_row, int *old_tenths_ptr, volatile int *tenths_ptr, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year, int tenths_displayed);
void update_hour(char * top_row, int *hours_ptr, int *day, int *month, int *year);
void update_min(char * top_row, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year);
void update_sec(char * top_row, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year);

void increment_hours(char *time, int *hours);
void increment_minutes(char *time, int *minutes);
void increment_seconds(char *time, int *seconds);

void increment_date(int *day, int *month, int *year);
void is_leap_year(int year);

void is_in_array(int val, int *arr, int size);
