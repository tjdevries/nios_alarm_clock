#include "hex.h"

void hex_on() {
	IOWR_ALTERA_AVALON_PIO_DATA(SEVENSEG30_BASE, 0xFFFFFFFF);
	IOWR_ALTERA_AVALON_PIO_DATA(SEVENSEG74_BASE, 0xFFFFFFFF);
}

void hex_off() {
	IOWR_ALTERA_AVALON_PIO_DATA(SEVENSEG30_BASE, 0x00000000);
	IOWR_ALTERA_AVALON_PIO_DATA(SEVENSEG74_BASE, 0x00000000);
}

uint8_t num_to_seven_seg(int num) {
	unsigned int result = 0;
	
	if (num == 0) { result = 0x7F - 0x40; }
	else if (num == 1) { result = 0x7F - 0x79; } 
	else if (num == 2) { result = 0x7F - 0x24; }
	else if (num == 3) { result = 0x7F - 0x30; }
	else if (num == 4) { result = 0x7F - 0x19; }
	else if (num == 5) { result = 0x7F - 0x12; }
	else if (num == 6) { result = 0x7F - 0x02; }
	else if (num == 7) { result = 0x7F - 0x78; }
	else if (num == 8) { result = 0x7F - 0x00; }
	else if (num == 9) { result = 0x7F - 0x10; }
	
	return result;
}

void hex_write_date(int month, int day, int year) {
	// Get the digits for the month
	int m1 = month % 10;
	int m2 = (month - m1) % 100;;
	
	// They will go in the first and second slots of the display
	uint8_t b = num_to_seven_seg(m1);
	uint8_t a = num_to_seven_seg(m2 / 10);
	
	// Get the digits of the day
	int d1 = day % 10;
	int d2 = (day - d1) % 100;
	
	// They will go in the third and fourth slots of the display
	uint8_t d = num_to_seven_seg(d1);
	uint8_t c = num_to_seven_seg(d2 / 10);

	// Concatenate them into the whole 32 bit seven seg representation
	uint32_t seven_seg_num_md = (a << 24) | (b << 16) | (c << 8) | d;
	
	// Write to the display 
	IOWR_ALTERA_AVALON_PIO_DATA(SEVENSEG74_BASE, seven_seg_num_md);
	
	// Get the digits for the date
	int y1 = year % 10;
	int y2 = (year - y1) % 100;
	int y3 = (year - y2 - y1) % 1000;
	int y4 = (year - y3 - y2 - y1) % 10000;
	
	// Change them into seven seg numbers
	uint8_t h = num_to_seven_seg(y1);
	uint8_t g = num_to_seven_seg(y2 / 10);
	uint8_t f = num_to_seven_seg(y3 / 100);
	uint8_t e = num_to_seven_seg(y4 / 1000);

	// Concatenate them into the whole 32 bit seven seg representation
	uint32_t seven_seg_num_y = (e << 24) | (f << 16) | (g << 8) | h;
	
	// Write to the display
	IOWR_ALTERA_AVALON_PIO_DATA(SEVENSEG30_BASE, seven_seg_num_y);
}