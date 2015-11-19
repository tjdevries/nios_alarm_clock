	
// Set the places where we write the hours	
int hours_1 = 4;
int hours_2 = 5;
// Set the places where we write the seconds
int sec_1 = 10;
int sec_2 = 11;
// Set the places where we write the minutes
int min_1 = 7;
int min_2 = 8;

// Set where we write AM or PM
int am_1 = 13;
int am_2 = 14;
	
void write_time_to_buffer(char *time, int second, int minute, int hour, int am_pm_mode){
	// Write the initial items that we need to in to the display
	// Write the hours back to the top_row
	handle_am_pm(time, &hour, am_pm_mode);
	
	// Write the new minutes out to the display 
	time[min_1] = '0' + (minute - (minute % 10)) / 10;
	time[min_2] = '0' + minute % 10;
	// Write out our new seconds to the display.
	time[sec_1] = '0' + (second - (second % 10)) / 10;
	time[sec_2] = '0' + second % 10;
}
	
void update_time(char * top_row, int *old_tenths_ptr, volatile int *tenths_ptr, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year, int am_pm_mode, int tenths_displayed) {
	// Set the tenths max
	int tenths_max = 10;
	
	// Increment our seconds
	if ( (*tenths_ptr) >= tenths_max) {
		// Update the seconds
		update_sec(top_row, seconds_ptr, minutes_ptr, hours_ptr, day, month, year, am_pm_mode);
		
		// Reset our tenths variable
		(*tenths_ptr) = 0;
	}
	
	// Update the display based on am_pm
	handle_am_pm(top_row, hours_ptr, am_pm_mode);
	
	// Pass our pointer through to the old tenths for time keeping
	(*old_tenths_ptr) = (*tenths_ptr);
}

void increment_hours(char *time, int *hours_ptr, int am_pm_mode){
	(*hours_ptr) = ((*hours_ptr) % 24) + 1;
	handle_am_pm(time, hours_ptr, am_pm_mode);
}

void increment_minutes(char *time, int *minutes_ptr){
	(*minutes_ptr) = (*minutes_ptr + 1) % 60;
	time[min_1] = '0' + (*minutes_ptr - (*minutes_ptr % 10)) / 10;
	time[min_2] = '0' + *minutes_ptr % 10;
}

void increment_seconds(char *time, int *seconds_ptr){
	(*seconds_ptr) = (*seconds_ptr + 1) % 60;
	time[sec_1] = '0' + (*seconds_ptr - (*seconds_ptr % 10)) / 10;
	time[sec_2] = '0' + *seconds_ptr % 10;
}

void update_hour(char * top_row, int *hours_ptr, int *day, int *month, int *year, int am_pm_mode) {
	// Set the maximum hours
	// TODO: Insert AM/PM
	int hour_max = 24;

	// Increment our hours variable
	(*hours_ptr) = (*hours_ptr) + 1;
	
	// See if we have reached our max hours
	if ((*hours_ptr) == hour_max) {
		// Update the date
		increment_date(day, month, year);
		
		// Reset hours
		(*hours_ptr) = 0;
	}
}

void update_min(char * top_row, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year, int am_pm_mode) {
	// Set our maximum minutes
	int min_max = 60;
	
	// Increment our minutes variable
	(*minutes_ptr) = (*minutes_ptr) + 1;
	
	if ((*minutes_ptr) == min_max) {
		// Update the hour
		update_hour(top_row, hours_ptr, day, month, year, am_pm_mode);
		
		// Reset the minutes
		(*minutes_ptr) = 0;
	}
	
	// Write the new minutes out to the display 
	top_row[min_1] = '0' + ((*minutes_ptr) - ((*minutes_ptr) % 10)) / 10;
	top_row[min_2] = '0' + (*minutes_ptr) % 10;
}

void update_sec(char * top_row, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int *day, int *month, int *year, int am_pm_mode) {
	int sec_max = 60;
	
	// Increment our seconds variable
	(*seconds_ptr) = (*seconds_ptr) + 1;
	
	// Check if we have incremented the seconds max
	if ((*seconds_ptr) == sec_max) {
		// Update the minutes
		update_min(top_row, minutes_ptr, hours_ptr, day, month, year, am_pm_mode);
		
		// Reset the seconds
		(*seconds_ptr) = 0;
	}
	
	// Write out our new seconds to the display.
	top_row[sec_1] = '0' + ((*seconds_ptr) - ((*seconds_ptr) % 10)) / 10;
	top_row[sec_2] = '0' + (*seconds_ptr) % 10;
}

void handle_am_pm(char * time, int *hours_ptr, int am_pm_mode) {
	// Write the hours back to the top_row
	//	This has to be updated every tenth of a second because the switch can happen at any time
	//	If the hours are less than twelve or it isn't am_pm_mode, then we just write the time
	if ( (*hours_ptr) <= 12 || (am_pm_mode == 0) ) { 
		if ( (*hours_ptr) == 0 ) {
			time[hours_1] = '1';
			time[hours_2] = '2';
		}
		else {
			time[hours_1] = '0' + ((*hours_ptr) - ((*hours_ptr) % 10)) / 10;
			time[hours_2] = '0' + (*hours_ptr) % 10;
		}
	} 
	// Otherwise we write the time minus 12
	else {
		int temp = (*hours_ptr) - 12;
		time[hours_1] = '0' + (temp - (temp % 10)) / 10;
		time[hours_2] = '0' + (temp) % 10;
	}
	// Write the AM or PM to the screen
	if (am_pm_mode) {
		if ((*hours_ptr) <= 11) {
			time[am_1] = 'A';
		}
		else {
			time[am_1] = 'P';
		}
		time[am_2] = 'M';
	}
	else {
		time[am_1] = ' ';
		time[am_2] = ' ';
	}
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
	
	hex_write_date(*month, *day, *year);
}

int is_leap_year(int year) {
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
