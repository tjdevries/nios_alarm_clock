

void update_time(char * top_row, int *old_tenths_ptr, volatile int *tenths_ptr, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year, int tenths_displayed) {
	// Set the tenths max
	int tenths_max = 10;
	
	// Increment our seconds
	if ( (*tenths_ptr) >= tenths_max) {
		// Update the seconds
		update_sec(top_row, seconds_ptr, minutes_ptr, hours_ptr, day, month, year);
		
		// Reset our tenths variable
		(*tenths_ptr) = 0;
	}
	
	// Pass our pointer through to the old tenths for time keeping
	(*old_tenths_ptr) = (*tenths_ptr);
	
	// End
	return 0;
}

void update_hour(char * top_row, int *hours_ptr, int *day, int *month, int *year) {
	// Set the maximum hours
	// TODO: Insert AM/PM
	int hour_max = 13;

	// Increment our hours variable
	(*hours_ptr) = (*hours_ptr) + 1;
	
	// See if we have reached our max hours
	if ((*hours_ptr) == hour_max) {
		// Update the date
		increment_date(day, month, year);
		
		// Reset hours
		(*hours_ptr) = 1;
	}
	
	// Set the places where we write the hours	
	int hours_1 = 4;
	int hours_2 = 5;
	
	// Write the hours back to the top_row
	top_row[hours_1] = '0' + ((*hours_ptr) - ((*hours_ptr) % 10)) / 10;
	top_row[hours_2] = '0' + (*hours_ptr) % 10;
}

void update_min(char * top_row, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year) {
	// Set our maximum minutes
	int min_max = 60;
	
	// Increment our minutes variable
	(*minutes_ptr) = (*minutes_ptr) + 1;
	
	if ((*minutes_ptr) == min_max) {
		// Update the hour
		update_hour(top_row, hours_ptr, day, month, year);
		
		// Reset the minutes
		(*minutes_ptr) = 0;
	}
	
	// Set the places where we write the minutes
	int min_1 = 7;
	int min_2 = 8;
	
	// Write the new minutes out to the display 
	top_row[min_1] = '0' + ((*minutes_ptr) - ((*minutes_ptr) % 10)) / 10;
	top_row[min_2] = '0' + (*minutes_ptr) % 10;
}

void update_sec(char * top_row, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year) {
	int sec_max = 60;
	
	// Increment our seconds variable
	(*seconds_ptr) = (*seconds_ptr) + 1;
	
	// Check if we have incremented the seconds max
	if ((*seconds_ptr) == sec_max) {
		// Update the minutes
		update_min(top_row, minutes_ptr, hours_ptr, day, month, year);
		
		// Reset the seconds
		(*seconds_ptr) = 0;
	}
	
	// Set the places where we write the seconds
	int sec_1 = 10;
	int sec_2 = 11;
	
	// Write out our new seconds to the display.
	top_row[sec_1] = '0' + ((*seconds_ptr) - ((*seconds_ptr) % 10)) / 10;
	top_row[sec_2] = '0' + (*seconds_ptr) % 10;
}

void increment_date(int *day, int *month, int *year) {
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