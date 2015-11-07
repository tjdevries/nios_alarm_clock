#include "timers.h"

/*		Timer setups 		*/
void timer0_isr(void * context, alt_32 id) {
	volatile int* contextTimerPtr = (volatile int*) context;
	*contextTimerPtr = *contextTimerPtr + 1;
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x0);
}

void init_timer_0(volatile int *tenths_int_ptr) {
	// Timer base is TIMER_0_BASE
	// Timer clock is 50,000,000 so to increment a counter, move it every 5,000,000 ticks.
	// high 0x004C
	// low 0x4B40
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
	// Used to use ALTERA_AVALON_TIMER_CONTROL_START_MSK, not sure which I need	
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0x004C);
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, 0x4B40);
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0x7);
	
	void* tenths_ptr = (void *) tenths_int_ptr;
	// Register the ISR. 
	alt_irq_register(TIMER_0_IRQ, tenths_ptr, timer0_isr);
}

void stop_timer_0() {
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0x0);
}

void timer1_isr(void * context, alt_32 id) {
	volatile int* contextTimerPtr = (volatile int*) context;
	*contextTimerPtr = *contextTimerPtr + 1;
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0x0);
}

void init_timer_1(volatile int *half_second_int_ptr) {
	// Timer base is TIMER_1_BASE
	// Timer clock is 50,000,000 so to increment a counter every half second, move it every 50,000,000/2 ticks.
	// high 0x017D
	// low 0x7840
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0);
	// Used to use ALTERA_AVALON_TIMER_CONTROL_START_MSK, not sure which I need	
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_1_BASE, 0x017D);
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE, 0x7840);
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 0x7);
	
	void* half_second_ptr = (void *) half_second_int_ptr;
	// Register the ISR. 
	alt_irq_register(TIMER_1_IRQ, half_second_ptr, timer1_isr);
}

void stop_timer_1() {
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 0x0);
}

//set timer to quicker speed for testing
//only for timer 0 for now
void speed_up(){
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0x0001);
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, 0x4585);	
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0x7);
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0);
}

//slow down timer back to original speed
//only for timer 0 for now
void slow_down(){
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0x004C);
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, 0x4B40);
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0x7);
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0);
}
