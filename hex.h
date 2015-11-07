#include "altera_avalon_pio_regs.h"
#include "BSP/system.h"
#include <stdint.h>

void hex_on();
void hex_off();

uint8_t num_to_seven_seg(int num);
void hex_write_date(int month, int day, int year);