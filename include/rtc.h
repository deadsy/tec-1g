//-----------------------------------------------------------------------------
/*

RTC Driver (ds1302)

*/
//-----------------------------------------------------------------------------

#ifndef RTC_H
#define RTC_H

//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------

struct rtc_time {
	uint8_t second;		// 0..59
	uint8_t minute;		// 0..59
	uint8_t hour;		// 0..12 or 0..23
	uint8_t day_of_month;	// 1..31
	uint8_t month_of_year;	// 1..12
	uint8_t day_of_week;	// 1..7
	uint8_t year;		// 0..99
	bool mode12;		// 12 hour clock
	bool pm;		// PM for a 12 hour clock
};

#define RTC_BASE_YEAR 2000

//-----------------------------------------------------------------------------

bool rtc_init(void);
uint8_t rtc_get_secs(void);
void rtc_get_time(struct rtc_time *t);

const char *rtc_day_of_week(struct rtc_time *t);
char *rtc_hms(struct rtc_time *t, char *s);
char *rtc_date(struct rtc_time *t, char *s);

//-----------------------------------------------------------------------------

#endif				// RTC_H

//-----------------------------------------------------------------------------
