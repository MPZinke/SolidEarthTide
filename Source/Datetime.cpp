
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on ..                                                                                                              *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "Datetime.hpp"

#include <iostream>


// ———————————————— CONSTRUCTORS & DESTRUCTOR —————————————————

// Constructor for Datetime object to default hour, minute, second to 0 by calling Datetime::Datetime(......).
Datetime::Datetime(int year, int month, int day)
: Datetime{year, month, day, 0, 0, 0}
{}


// Constructor for Datetime object to default hour, minute, second to 0 by calling Datetime::Datetime(......).
Datetime::Datetime(int year, int month, int day, int hour, int minute, double second)
: Date{year, month, day}, _hour{hour}, _minute{minute}, _second{second}
{
	// LN73-74
	//      call civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
	//      call mjdciv(mjd,fmjd,iyr,imo,idy,ihr,imn,sec)    !*** normalize civil time
	CivilTime_to_ModifiedJulianDate();
	ModifiedJulianDate_to_CivilTime();
}


Datetime::~Datetime()
{
	if(_time) delete _time;
}


// Converts Civil Time to Modified Julian Date.
// LN 1151-1158
//       subroutine civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
// 
// *** convert civil date to modified julian date
// 
// *** imo in range 1-12, idy in range 1-31
// *** only valid in range mar-1900 thru feb-2100     (leap year protocols)
// *** ref: hofmann-wellenhof, 2nd ed., pg 34-35
// *** operation confirmed against table 3.3 values on pg.34
void Datetime::CivilTime_to_ModifiedJulianDate()
{
	// LN1164:      if(iyr.lt.1900) stop 34588
	if(_year < 1900) throw std::runtime_error("Year is less than allowable year");  // this shouldn't ever happen
	// LN1166-1178
	//      if(imo.le.2) then
	//        y=iyr-1
	//        m=imo+12
	//      else
	//        y=iyr
	//        m=imo
	//      endif
	//
	//      it1=365.25d0*y
	//      it2=30.6001d0*(m+1)
	//      mjd=it1+it2+idy-679019
	//
	//      fmjd=(3600*ihr+60*imn+sec)/86400.d0
	int years_to_days = APPR_DAY_IN_YEAR * ((double)_year - (_month <= 2 ? 1.0 : 0.0));
	int months_to_days = JULIAN_DAYS_IN_MONTH * (1.0 + (double)_month + (_month <= 2 ? 12.0 : 0.0));
	_mod_julian = years_to_days + months_to_days + _day - ERA_DAYS_TO_1901;

	_fractional_mod_julian = ((double)(3600*_hour + 60*_minute + _second)) / 86400.0;
}


// Converts Modified Julian Date to Civil Time.
// LN1182-1189
//       subroutine mjdciv(mjd,fmjd,iyr,imo,idy,ihr,imn,sec)
// 
// *** convert modified julian date to civil date
// 
// *** imo in range 1-12, idy in range 1-31
// *** only valid in range mar-1900 thru feb-2100
// *** ref: hofmann-wellenhof, 2nd ed., pg 34-35
// *** operation confirmed for leap years (incl. year 2000)
void Datetime::ModifiedJulianDate_to_CivilTime()
{
	// julian_date is mislabeled as rjd (reduced JD) in LN1193 
	// this is confirmed by: https://gssc.esa.int/navipedia/index.php/Julian_Date

	// LN1193-1198
	//       rjd=mjd+fmjd+2400000.5d0
	//       ia=(rjd+0.5d0)
	//       ib=ia+1537
	//       ic=(ib-122.1d0)/365.25d0
	//       id=365.25d0*ic
	//       ie=(ib-id)/30.6001d0
	double julian_date = _mod_julian + _fractional_mod_julian + MODIFIED_JULIAN_TO_JULIAN;
	int inv_transform_B = (int)(julian_date + 0.5) + 1537;
	int inv_transform_C = (inv_transform_B - 122.1) / APPR_DAY_IN_YEAR;
	int inv_transform_D = APPR_DAY_IN_YEAR * inv_transform_C;
	int inv_transform_E = (inv_transform_B - inv_transform_D) / JULIAN_DAYS_IN_MONTH;

	// LN1200-1208
	// *** the fractional part of a julian day is fractional mjd + 0.5
	// *** therefore, fractional part of julian day + 0.5 is fractional mjd
	// 
	//       it1=ie*30.6001d0
	//       idy=ib-id-it1+fmjd
	//       it2=ie/14.d0
	//       imo=ie-1-12*it2
	//       it3=(7+imo)/10.d0
	//       iyr=ic-4715-it3

	// EQUIVALENTLY
	//       it1=ie*30.6001d0	≈ int=(double)int*double
	//       idy=ib-id-it1+fmjd	≈ int=(double)(int-int-int)+double
	//       it2=ie/14.d0		≈ int=(double)int/double
	//       imo=ie-1-12*it2	≈ int=(double)(int-int-int)*double
	//       it3=(7+imo)/10.d0	≈ int=(double)(int+int)/double
	//       iyr=ic-4715-it3	≈ int=int-int-int
	_day = inv_transform_B - inv_transform_D - (int)(inv_transform_E*JULIAN_DAYS_IN_MONTH) + _fractional_mod_julian;
	_month = inv_transform_E  - 1 - 12 * (int)(inv_transform_E / 14.0);
	_year = inv_transform_C - 4715 - (int)((7 + _month) / 10.0);

	// LN1210-1214
	//       tmp=fmjd*24.d0
	//       ihr=tmp
	//       tmp=(tmp-ihr)*60.d0
	//       imn=tmp
	//       sec=(tmp-imn)*60.d0

	// EQUIVALENTLY
	//       tmp=fmjd*24.d0			≈ double=double*double
	//       ihr=tmp				≈ int=(int)double
	//       tmp=(tmp-ihr)*60.d0	≈ double=(double-(double)int)*double
	//       imn=tmp				≈ int=(int)double
	//       sec=(tmp-imn)*60.d0	≈ double=(double-(double)int)*double
	double converted_fractional_mod_julian = _fractional_mod_julian * 24.0;
	_hour = converted_fractional_mod_julian;  // casts to int
	converted_fractional_mod_julian = (converted_fractional_mod_julian - _hour) * 60.0;
	_minute = converted_fractional_mod_julian;
	_second = (converted_fractional_mod_julian - _minute) * 60.0;
}


// LN1044-1048
//       subroutine setjd0(iyr,imo,idy)
// 
// *** set the integer part of a modified julian date as epoch, mjd0
// *** the modified julian day is derived from civil time as in civmjd()
// *** allows single number expression of time in seconds w.r.t. mjd0
void Datetime::set_initial_JulianDate()
{
	// LN1053
	//      common/mjdoff/mjd0

	// LN1057–1071
	//       if(imo.le.2) then
	//         y=iyr-1
	//         m=imo+12
	//       else
	//         y=iyr
	//         m=imo
	//       endif
	// 
	//       it1=365.25d0*y
	//       it2=30.6001d0*(m+1)
	//       mjd=it1+it2+idy-679019
	// 
	// *** now set the epoch for future time computations
	// 
	//       mjd0=mjd
	int int_years = APPR_DAY_IN_YEAR * (_year - (_month <= 2 ? 1 : 0));
	int int_month = JULIAN_DAYS_IN_MONTH * (_month + (_month <= 2 ? 12 : 0) + 1);
	_initial_mod_julian = int_years + int_month + _day - ERA_DAYS_TO_1901;
}


// solid.f: LN68-75
// *** here comes the sun  (and the moon)  (go, tide!)
//
//       ihr=   0
//       imn=   0
//       sec=0.d0                                         !*** UTC time system
//       call civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
//       call mjdciv(mjd,fmjd,iyr,imo,idy,ihr,imn,sec)    !*** normalize civil time
//       call setjd0(iyr,imo,idy)
void Datetime::standardize_civil_time_and_set_initial_julian_date()
{
	modified_julian_date();
	fractional_modified_julian_date();
	modified_julian_date_to_civil_time();
	modified_julian_date();
	_initial_modified_julian = _modified_julian;  // LN1071: mjd0=mjd
}


// ————————————————————— TYPE CASTERS —————————————————————

Date Datetime::date()
{
	return Date{_year, _month, _day};
}


Datetime Date::datetime()
{
	return Datetime{_year, _month, _day};
}


Datetime Date::datetime(int hour, int minute, double second)
{
	return Datetime{_year, _month, _day, hour, minute, second};
}


// ———————————————————————————————————————————————————— GETTERS ————————————————————————————————————————————————————— //

int Datetime::hour()
{
	return _hour;
}


int Datetime::minute()
{
	return _minute;
}


double Datetime::second()
{
	return _second;
}


int* Datetime::time_array()
{
	if(!_time) _time = new int[3];

	_time[HOUR] = _hour;
	_time[MINUTE] = _minute;
	_time[SECOND] = _second;

	return _time;
}


void Datetime::time_array(int copy_array[])
{
	copy_array[HOUR] = _hour;
	copy_array[MINUTE] = _minute;
	copy_array[SECOND] = _second;
}


// ———————————————————————————————————————————————————— SETTERS ————————————————————————————————————————————————————— //

void Datetime::hour(int hour)
{
	_hour = hour;
}


void Datetime::minute(int minute)
{
	_minute = minute;
}


void Datetime::second(int second)
{
	_second = second;
}


void Datetime::time(int hour, int minute, double second)
{
	_hour = hour;
	_minute = minute;
	_second = second;
}


void Datetime::add_time(double seconds)
{
	_second += seconds - (int)(seconds / 60);
	if(60.0 <= _second)
	{
		_minute++;
		_second -= 60.0;
	}
	if(seconds < 60.0) return;

	_minute += seconds / 60;
	if(59 < _minute)
	{
		_hour++;
		_minute -= 60;
	}
	if(seconds < 3600) return;

	_hour += seconds / 3600;
	if(23 < _hour)
	{
		add_days(1);
	}
}


// ————————————————————————————————————————————————— TIME CONVERSION ———————————————————————————————————————————————— //

// solid.f: LN1152: subroutine civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
// ARGS: 	iyr = _year, imo = _month, idy = _day, ihr = _hour, imn = _minute, sec = _second, 
//			mjd = modified_julian, fmjd = fractional_modified_julian
// *** convert civil date to modified julian date
// *** imo in range 1-12, idy in range 1-31
// *** only valid in range mar-1900 thru feb-2100     (leap year protocols)
// *** ref: hofmann-wellenhof, 2nd ed., pg 34-35
// *** operation confirmed against table 3.3 values on pg.34
double Datetime::fractional_modified_julian_date()
{
	int seconds_of_day = SECONDS_IN_HOUR * _hour + SECONDS_IN_MINUTE * _minute + _second;
	double fractional_modified_julian = (double)seconds_of_day / SECONDS_IN_DAY;

	_fractional_mod_julian = fractional_modified_julian;
	return fractional_modified_julian;
}


// solid.f: LN1182: subroutine mjdciv(mjd,fmjd,iyr,imo,idy,ihr,imn,sec)
// ARGS:	mjd = _modified_julian, fmjd = _fractional_mod_julian, iyr = _year, imo = _month, idy = _day,
//			ihr = _hour, imn = _minute, sec = _second 
// *** convert modified julian date to civil date
// *** imo in range 1-12, idy in range 1-31
// *** only valid in range mar-1900 thru feb-2100
// *** ref: hofmann-wellenhof, 2nd ed., pg 34-35
// *** operation confirmed for leap years (incl. year 2000)
void Datetime::modified_julian_date_to_civil_time()
{
	// julian_date is mislabeled as rjd (reduced JD) in LN1193 
	// this is confirmed by: https://gssc.esa.int/navipedia/index.php/Julian_Date
	double julian_date = _modified_julian + _fractional_mod_julian + MODIFIED_JULIAN_TO_JULIAN;
	int inverse_transform_part_B = (int)(julian_date + 0.5) + 1537;
	int inverse_transform_part_C = (inverse_transform_part_B - 122.1) / DAYS_IN_YEAR;
	int inverse_transform_part_D = inverse_transform_part_C * DAYS_IN_YEAR;
	int inverse_transform_part_E = (inverse_transform_part_B - inverse_transform_part_D) / JULIAN_DAYS_IN_MONTH;

	// *** the fractional part of a julian day is fractional mjd + 0.5
	// *** therefore, fractional part of julian day + 0.5 is fractional mjd
	_day =	inverse_transform_part_B - inverse_transform_part_D
				- (int)(inverse_transform_part_E * JULIAN_DAYS_IN_MONTH)
				+ _fractional_mod_julian;
	_month = inverse_transform_part_E - 1 - 12 * (int)(inverse_transform_part_E / 14);  // prop already int div.
	_year = inverse_transform_part_C - 4715 - (int)((7 + _month) / 10);

	// set time members
	double fractional_modified_julian_hours = _fractional_mod_julian * 24.0;
	_hour = (int)fractional_modified_julian_hours;
	fractional_modified_julian_hours = (fractional_modified_julian_hours - _hour) * 60.0;
	_minute = fractional_modified_julian_hours;
	_second = (fractional_modified_julian_hours - _minute) * 60;
}


// solid.f: LN909-910: convert FMJD to UTC to TT
double Datetime::modified_julian_date_to_Terrestrial_Time_julian_date_centuries()
{
	// LN909: !*** UTC time (sec of day)
	double seconds_UTC = _fractional_mod_julian * SECONDS_IN_DAY;
	// LN910: !*** TT  time (sec of day)
	double terrestrial_time_of_day = UTC_seconds_to_Terrestrial_Time(seconds_UTC); 
	double FMJD_TT = terrestrial_time_of_day / SECONDS_IN_DAY;  // LN911: !*** TT  time (fract. day)

	// LN914: *** julian centuries since 1.5 january 2000 (J2000)
	// ***   (note: also low precision use of mjd --> tjd)
	// LN917: !*** Julian Date, TT
	double julian_date_TT = _modified_julian + FMJD_TT + MODIFIED_JULIAN_TO_JULIAN;
	double julian_centuries = (julian_date_TT - 2451545.0) / 36525.0;  // LN918: !*** julian centuries, TT

	return julian_centuries;
}


// *** convert utc (sec) to terrestrial time (sec)
double Datetime::UTC_seconds_to_Terrestrial_Time(double UTC_seconds)
{
	double seconds_TAI = UTC_to_International_Atomic_Time(UTC_seconds);
	// LN1415: double precision function tai2tt(ttai)
	// ***** http://tycho.usno.navy.mil/systime.html
	double seconds_TT = seconds_TAI + INTERNATION_ATOMIC_TIME_TO_TERRESTRIAL_TIME;
	return seconds_TT;
}


// solid.f: LN1245: double precision function utc2tai(tutc)
// ARGS: tutc = seconds_UTC
// *** convert utc (sec) to tai (sec)
// solid.f: LN1256: double precision function getutcmtai(tsec)
// ARGS: tsec = seconds_UTC
// *** get utc - tai (s)
// ***** "Julian Date Converter"
// ***** http://aa.usno.navy.mil/data/docs/JulianDate.php
double Datetime::UTC_to_International_Atomic_Time(double seconds_UTC)
{
	double normalized_seconds_UTC = seconds_UTC;  // copy of UTC to normalize
	int normalized_initial_modified_julian = _initial_modified_julian;
	while(SECONDS_IN_DAY <= normalized_seconds_UTC)
	{
		normalized_seconds_UTC -= SECONDS_IN_DAY;
		normalized_initial_modified_julian++;
	}

	while(normalized_seconds_UTC < 0)
	{
		normalized_seconds_UTC += SECONDS_IN_DAY;
		normalized_initial_modified_julian--;
	}

	// LN1292-1298 *** test upper table limit         (upper limit set by bulletin C memos)
	if(UPPER_MODIFIED_JULIAN < normalized_initial_modified_julian)
	{
		_leap_second_flag = true;  // LN1295: !*** true means flag *IS* raised
		return normalized_seconds_UTC - 37.0;  // LN1296: !*** return the upper table value
	}

	if(normalized_initial_modified_julian < LOWER_MODIFIED_JULIAN)
	{
		_leap_second_flag = true;  // LN1303: !*** true means flag *IS* raised
		return normalized_seconds_UTC - 10.0;  // LN1304: !*** return the upper table value
	}

	// LN1308
	// ***** http://maia.usno.navy.mil/ser7/tai-utc.dat
	// *** 1972 JAN  1 =JD 2441317.5  TAI-UTC=  10.0s		*** 1972 JUL  1 =JD 2441499.5  TAI-UTC=  11.0s
	// *** 1973 JAN  1 =JD 2441683.5  TAI-UTC=  12.0s		*** 1974 JAN  1 =JD 2442048.5  TAI-UTC=  13.0s
	// *** 1975 JAN  1 =JD 2442413.5  TAI-UTC=  14.0s		*** 1976 JAN  1 =JD 2442778.5  TAI-UTC=  15.0s
	// *** 1977 JAN  1 =JD 2443144.5  TAI-UTC=  16.0s		*** 1978 JAN  1 =JD 2443509.5  TAI-UTC=  17.0s
	// *** 1979 JAN  1 =JD 2443874.5  TAI-UTC=  18.0s		*** 1980 JAN  1 =JD 2444239.5  TAI-UTC=  19.0s
	// *** 1981 JUL  1 =JD 2444786.5  TAI-UTC=  20.0s		*** 1982 JUL  1 =JD 2445151.5  TAI-UTC=  21.0s
	// *** 1983 JUL  1 =JD 2445516.5  TAI-UTC=  22.0s		*** 1985 JUL  1 =JD 2446247.5  TAI-UTC=  23.0s
	// *** 1988 JAN  1 =JD 2447161.5  TAI-UTC=  24.0s		*** 1990 JAN  1 =JD 2447892.5  TAI-UTC=  25.0s
	// *** 1991 JAN  1 =JD 2448257.5  TAI-UTC=  26.0s		*** 1992 JUL  1 =JD 2448804.5  TAI-UTC=  27.0s
	// *** 1993 JUL  1 =JD 2449169.5  TAI-UTC=  28.0s		*** 1994 JUL  1 =JD 2449534.5  TAI-UTC=  29.0s
	// *** 1996 JAN  1 =JD 2450083.5  TAI-UTC=  30.0s		*** 1997 JUL  1 =JD 2450630.5  TAI-UTC=  31.0s
	// *** 1999 JAN  1 =JD 2451179.5  TAI-UTC=  32.0s		*** 2006 JAN  1 =JD 2453736.5  TAI-UTC=  33.0s
	// *** 2009 JAN  1 =JD 2454832.5  TAI-UTC=  34.0s		*** 2012 JUL  1 =JD 2456109.5  TAI-UTC=  35.0s
	// *** 2015 JUL  1 =JD 2457204.5  TAI-UTC=  36.0s		*** 2017 JAN  1 =JD 2457754.5  TAI-UTC=  37.0s
	// ***** other leap second references at:
	// ***** http://hpiers.obspm.fr/eoppc/bul/bulc/Leap_Second_History.dat
	// ***** http://hpiers.obspm.fr/eoppc/bul/bulc/bulletinc.dat
	// *** test against newest leaps first	
	int seconds_for_years[] =	{
									57754, 57204, 56109, 54832, 53736, 51179, 50630, 50083, 49534, 49169, 
									48804, 48257, 47892, 47161, 46247, 45516, 45151, 44786, 44239, 43874,
									43509, 43144, 42778, 42413, 42048, 41683, 41499, 41317
								};
	for(int x = 0; x < 27; x++)
	{
		if(normalized_initial_modified_julian >= seconds_for_years[x])
			return normalized_seconds_UTC - (x - 37);
	}
	
	throw std::runtime_error("FATAL ERROR --\nFell through tests in Datetime::UTC_to_International_Atomic_Time");
}


// solid.f: LN840: subroutine getghar(mjd,fmjd,ghar)
// ARGS: mjd = modified_julian, fmjd = fractional_modified_julian, ghar = greenwhich_hour_angle
// *** convert mjd/fmjd in UTC time to Greenwich hour angle (in radians)
// *** "satellite orbits: models, methods, applications" montenbruck & gill(2000)
// *** section 2.3.1, pg. 33
double Datetime::greenwhich_hour_angle_radians()
{
	// LN853: !*** UTC time (sec of day)
	double seconds_UTC = _fractional_mod_julian * SECONDS_IN_DAY;
	// LN857: !*** days since J2000
	double days_since_start_2000 =	_modified_julian - MODIFIED_JULIAN_START_TO_J2000 
										+ _fractional_mod_julian + JULIAN_TO_REDUCED_JULIAN;

	// LN859: *** greenwich hour angle for J2000  (12:00:00 on 1 Jan 2000)
	// LN862: !*** corrn.   (+digits)
	double greenwhich_hour_angle = days_since_start_2000 * 360.9856473662862 + 280.46061837504;

	// LN864: **** normalize to 0-360 and convert to radians
	int int_greenwhich_hour_angle = greenwhich_hour_angle / 360.0;  // LN866
	// LN867
	greenwhich_hour_angle = (greenwhich_hour_angle - int_greenwhich_hour_angle * 360) / RADIAN;
	while(PI_TIMES_TWO <  greenwhich_hour_angle)
		greenwhich_hour_angle -= PI_TIMES_TWO;

	while(greenwhich_hour_angle < 0)
		greenwhich_hour_angle += PI_TIMES_TWO;

	return greenwhich_hour_angle;
}