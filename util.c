

char * update_time(int *old_tenths_ptr, volatile int *tenths_ptr, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int tenths_displayed) {
	// Set the indeces of the times we are writing out to the screen.
	int hours_1 = 4;
	int hours_2 = 5;
	int min_1 = 7;
	int min_2 = 8;
	int sec_1 = 10;
	int sec_2 = 11;
	
	
	char top_row[17] = "    00:00:00    ";
	// Increment our seconds
	if ( (*tenths_ptr) >= 10) {
		(*seconds_ptr) = (*seconds_ptr) + 1;
		if ((*seconds_ptr) == 60) {
			(*minutes_ptr) = ((*minutes_ptr) + 1) % 60;
			(*seconds_ptr) = 0;
			// Write the new minutes out to the display 
			top_row[min_1] = '0' + ((*minutes_ptr) - ((*minutes_ptr) % 10)) / 10;
			top_row[min_2] = '0' + (*minutes_ptr) % 10;
		}
		// Write out our new seconds to the display.
		top_row[sec_1] = '0' + ((*seconds_ptr) - ((*seconds_ptr) % 10)) / 10;
		top_row[sec_2] = '0' + (*seconds_ptr) % 10;
		(*tenths_ptr) = 0;
	}
	(*old_tenths_ptr) = (*tenths_ptr);
	return top_row;
}

void update_hour() {

}

void update_min() {

}

void update_sec() {

}