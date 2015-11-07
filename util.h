#include "altera_up_avalon_character_lcd.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <stdio.h>
#include "BSP/system.h"

// Personal includes
#include "hex.h"
#include "timers.h"

void update_time(char *top_row_ptr, int *old_tenths_ptr, int *tenths_ptr, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int tenths_displayed);
void update_hour();
void update_min();
void update_sec();