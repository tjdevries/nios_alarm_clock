

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

void increment_date(int *month, int *day, int *year) {
	// Used to take the date and add 1 to it.
	//	Will handle changing the month and year if necessary.
	/*
	1  January	31
	2  February	28*, 29
	3  March	31
	4  April	30
	5  May	31
	6  June	30
	7  July	31
	8  August	31
	9  September	30
	10 October	31
	11 November	30
	12 December	31
	*/
	// int month_31[7] = {1, 3, 5, 7, 8, 10, 12};
	// int month_30[4] = {4, 6, 9, 11};
	
	// All the months will let us go up to 28
	if (*day < 28 ) { (*day)++; }
	// Handle February... what a month
	else if ( *month == 2 ) {
		// If it is the 28th and a leap year, then we make it bigger
		if (*day == 28 && is_leap_year(*year)) {
			(*day)++;
		}
		// Otherwise, we always increment the month and start over.
		else {
			(*month)++;
			*day = 1;
		}
	}
	// Handle the 30 day months
	else if( *month == 4 || *month == 6 || *month == 9 || *month == 11 ) {
		if ( *day < 30 ) {
			(*day)++;
		}
		else {
			(*month)++;
			*day = 1;
		}
	}
	// Must be a 31 month
	else {
		if ( *day < 31 ) {
			(*day)++;
		}
		else {
			// handle December
			if (*month == 12) {
				(*year)++;
				*month = 1;
			}
			else {
				(*month)++;
			}
			*day = 1;
		}
	}
}

void is_leap_year(int year) {
	// Returns a 1 for is a leap year, returns 0 otherwise
	if (year % 400 == 0) {
       return 1;
	}
	else if (year % 100 == 0) {
		return 0;
	}
	else if (year % 4 == 0) {
		return 1;
	}
	return 0;
}