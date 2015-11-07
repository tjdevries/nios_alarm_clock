

void update_time(char *top_row_ptr, int *old_tenths_ptr, int *tenths_ptr, int *seconds_ptr, int *minutes_ptr, int *hours_ptr, int tenths_displayed) {
	// Increment our seconds
	if (*tenths_ptr >= 10) {
		(*seconds_ptr)++;
		if (*seconds_ptr == 60) {
			*minutes_ptr = (*minutes_ptr + 1) % 60;
			*seconds_ptr = 0;
			*top_row_ptr[0] = '0' + (*minutes_ptr - (*minutes_ptr % 10)) / 10;
			*top_row_ptr[1] = '0' + *minutes_ptr % 10;
		}
		*top_row_ptr[3] = '0' + (*seconds_ptr - (*seconds_ptr % 10)) / 10;
		*top_row_ptr[4] = '0' + *seconds_ptr % 10;
		*tenths_ptr = 0;
	}
	*old_tenths_ptr = *tenths_ptr;
	*top_row_ptr[6] = '0' + *tenths_ptr;
}

void update_hour() {

}

void update_min() {

}

void update_sec() {

}