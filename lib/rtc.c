//-----------------------------------------------------------------------------
/*

RTC Driver (ds1302)

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>

#include "hw.h"
#include "rtc.h"

//-----------------------------------------------------------------------------
// ds1302 registers

// clock
#define	clockSecond        (0x80 | (0 << 1))
#define	clockMinute        (0x80 | (1 << 1))
#define	clockHour          (0x80 | (2 << 1))
#define	clockDayOfMonth    (0x80 | (3 << 1))
#define	clockMonthOfYear   (0x80 | (4 << 1))
#define	clockDayOfWeek     (0x80 | (5 << 1))
#define	clockYear          (0x80 | (6 << 1))
#define	clockWriteProtect  (0x80 | (7 << 1))
#define	clockTrickleCharge (0x80 | (8 << 1))
#define	clockBurst (0x80 | (0x1f << 1))

#define writeProtectEnabled (1 << 7)	// in clockWriteProtect
#define clockHalted (1 << 7)	// in clockSecond
#define mode12Hour (1 << 7)	// in clockHour

// ram
#define RAM_ADR(x) (0x80 | (1 << 6) | (((x) & 0x1f) << 1))
#define	ramBurst RAM_ADR(0x1f)

//-----------------------------------------------------------------------------

static uint8_t bcd_to_uint(uint8_t x) {
	return (10 * (x >> 4)) + (x & 15);
}

//-----------------------------------------------------------------------------
// time formatting routines

// Note: the ds1302 has a 1..7 counter that increments every day.
// The assignment of this counter to an actual day name is arbitrary.
// This is the convention used on this system. It's up to the user
// to configure the number so the day name matches the actual date.

static const char *const day_of_week[] = {
	"Monday",		// 1
	"Tuesday",		// 2
	"Wednesday",		// 3
	"Thursday",		// 4
	"Friday",		// 5
	"Saturday",		// 6
	"Sunday",		// 7
};

const char *rtc_day_of_week(struct rtc_time *t) {
	return day_of_week[(t->day_of_week - 1) % 7];
}

char *rtc_hms(struct rtc_time *t, char *s) {
	if (t->mode12) {
		sprintf(s, "%02d:%02d:%02d %s", t->hour, t->minute, t->second, (t->pm) ? "pm" : "am");
	} else {
		sprintf(s, "%02d:%02d:%02d", t->hour, t->minute, t->second);
	}
	return s;
}

char *rtc_date(struct rtc_time *t, char *s) {
	int year = t->year + RTC_BASE_YEAR;
	sprintf(s, "%04d/%02d/%02d", year, t->month_of_year, t->day_of_month);
	return s;
}

//-----------------------------------------------------------------------------

static uint8_t rtc_byte_rd(void) {
	// data is read on the falling edge of the clock
	// writeEnable is active low, so we must set it here
	uint8_t val = 0;
	for (uint8_t i = 0; i < 8; i++) {
		// clock hi
		rtcPort = rtcChipEnable | rtcWriteEnable | rtcClock;
		// clock lo
		rtcPort = rtcChipEnable | rtcWriteEnable;
		val >>= 1;
		if (rtcPort & rtcOut) {
			val |= 0x80;
		}
	}
	return val;
}

static void rtc_byte_wr(uint8_t val) {
	// data is written on the rising edge of the clock
	for (uint8_t i = 0; i < 8; i++) {
		uint8_t port = rtcChipEnable;
		// setup the input bit to the rtc
		if (val & 1) {
			port |= rtcIn;
		}
		val >>= 1;
		// clock lo
		rtcPort = port;
		// clock hi
		rtcPort = port | rtcClock;
	}
}

//-----------------------------------------------------------------------------

// read a register
static uint8_t rtc_rd(uint8_t adr) {
	adr |= 1;		// RW = 1
	// chip enable hi
	rtcPort = rtcChipEnable;
	rtc_byte_wr(adr);
	uint8_t val = rtc_byte_rd();
	// chip enable lo
	rtcPort = 0;
	return val;
}

// read n registers in burst mode
static void rtc_rd_burst(uint8_t adr, uint8_t *buf, uint8_t n) {
	adr |= 1;		// RW = 1
	// chip enable hi
	rtcPort = rtcChipEnable;
	rtc_byte_wr(adr);
	for (uint8_t i = 0; i < n; i++) {
		buf[i] = rtc_byte_rd();
	}
	// chip enable lo
	rtcPort = 0;
}

// write a register
static void rtc_wr(uint8_t adr, uint8_t val) {
	adr &= ~1;		// RW = 0
	// chip enable hi
	rtcPort = rtcChipEnable;
	rtc_byte_wr(adr);
	rtc_byte_wr(val);
	// chip enable lo
	rtcPort = 0;
}

//-----------------------------------------------------------------------------
// time

void rtc_get_time(struct rtc_time *t) {
	uint8_t buf[8];
	rtc_rd_burst(clockBurst, buf, sizeof(buf));
	t->second = bcd_to_uint(buf[0] & ~clockHalted);
	t->minute = bcd_to_uint(buf[1]);
	t->day_of_month = bcd_to_uint(buf[3]);
	t->month_of_year = bcd_to_uint(buf[4]);
	t->day_of_week = buf[5];
	t->year = bcd_to_uint(buf[6]);
	t->mode12 = (buf[2] & mode12Hour) != 0;
	if (t->mode12) {
		t->pm = (buf[2] & (1 << 5)) != 0;
		t->hour = bcd_to_uint(buf[2] & 0x1f);
	} else {
		t->pm = false;
		t->hour = bcd_to_uint(buf[2] & 0x3f);
	}
}

uint8_t rtc_get_secs(void) {
	return bcd_to_uint(rtc_rd(clockSecond) & ~clockHalted);
}

//-----------------------------------------------------------------------------
// ram

// read a value from ram
uint8_t rtc_rd_ram(uint8_t adr) {
	return rtc_rd(RAM_ADR(adr));
}

// write a value to ram
void rtc_wr_ram(uint8_t adr, uint8_t val) {
	rtc_wr(RAM_ADR(adr), val);
}

// get a buffer of ram values
void rtc_get_ram(uint8_t *buf, uint8_t n) {
	rtc_rd_burst(ramBurst, buf, n);
}

//-----------------------------------------------------------------------------

bool rtc_init(void) {
	// clear write protect
	rtc_wr(clockWriteProtect, 0);
	// clear trickle charge
	rtc_wr(clockTrickleCharge, 0);
	// see if we really have a ds1302...
	// read the seconds
	uint8_t s = rtc_rd(clockSecond);
	s = bcd_to_uint(s & ~clockHalted);
	if (s > 59) {
		return false;
	}
	// read the day of week
	uint8_t dow = rtc_rd(clockDayOfWeek);
	if ((dow < 1) || (dow > 7)) {
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
