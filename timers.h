#include "BSP/system.h"
#include "altera_avalon_timer_regs.h"

void timer0_isr(void * context, alt_32 id);
void init_timer_0(volatile int *tenths_int_ptr);

void timer1_isr(void * context, alt_32 id);
void init_timer_1(volatile int *half_second_int_ptr);

void stop_timer_0();
void stop_timer_1();

void speed_up();
void slow_down();