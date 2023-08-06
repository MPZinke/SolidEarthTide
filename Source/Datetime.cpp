

#include "Datetime.hpp"


#include <assert.h>


Datetime::Datetime(unsigned int year, unsigned int month/*=1*/, unsigned int day/*=1*/, unsigned int hour/*=0*/,
  unsigned int minute/*=0*/, unsigned int second/*=0*/)
: _year{year}, _month{month}, _day{day}, _hour{hour}, _minute{minute}, _second{second}
{
	assert(0 < month && month <= 12);
	assert(0 < day);
	if(month == JANUARY || month == MARCH || month == MAY || month == JULY || month == AUGUST || month == OCTOBER
	  || month == DECEMBER
	)
	{
		assert(day <= 31);
	}
	else if(month != FEBRUARY)
	{
		assert(day <= 30);
	}
	// Leap year: https://en.wikipedia.org/wiki/Leap_year
	else if(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
	{
		assert(day <= 29);
	}
	else
	{
		assert(day <= 28);
	}

	assert(hour < 24);
	assert(minute < 60);
	assert(second < 60);
}


Datetime Datetime::normalize()
/*
solid.f [LN 73–75]
```
|      call civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
|      call mjdciv(mjd,fmjd,iyr,imo,idy,ihr,imn,sec)    !*** normalize civil time
|      call setjd0(iyr,imo,idy)
```
*/
{
	unsigned int modified_julian_date;
	double fractional_modified_julian_date;

	civil_datetime_to_modified_julian_datetime(modified_julian_date, fractional_modified_julian_date);
	return modified_julian_datetime_to_civil_datetime(modified_julian_date, fractional_modified_julian_date);
}


void Datetime::civil_datetime_to_modified_julian_datetime(unsigned int& modified_julian_date, double& fractional_modified_julian_date)
/*
solid.f [LN 1152–1159]
```
|      subroutine civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
|
|*** convert civil date to modified julian date
|
|*** imo in range 1-12, idy in range 1-31
|*** only valid in range mar-1900 thru feb-2100     (leap year protocols)
|*** ref: hofmann-wellenhof, 2nd ed., pg 34-35
|*** operation confirmed against table 3.3 values on pg.34
```
iyr — _year
imo — _month
idy — _day
ihr — _hour
imn — _minute
sec — _second
*mjd — modified_julian_date
*fmjd — fractional_modified_julian_date
*/
{
	/*
	solid.f [LN 1162–1172]
	```
	|      integer y
	|
	|      if(iyr.lt.1900) stop 34588
	|
	|      if(imo.le.2) then
	|        y=iyr-1
	|        m=imo+12
	|      else
	|        y=iyr
	|        m=imo
	|      endif
	```
	*/
	assert(_year >= 1900);

	unsigned int year = _year;
	unsigned int month = _month;
	if(_month <= 2)
	{
		year--;
		month += 12;
	}

	/*
	solid.f [LN 1174–1175]
	```
	|      it1=365.25d0*y
	|      it2=30.6001d0*(m+1)
	```
	*/
	unsigned int year_days = (double)365.25 * year;
	unsigned int month_days = (double)30.6001 * (month + 1);

	/*
	solid.f [LN 1176, 1178]
	```
	|      mjd=it1+it2+idy-679019
	|      fmjd=(3600*ihr+60*imn+sec)/86400.d0
	```
	mjd — modified_julian_date
	fmjd — fractional_modified_julian_date
	*/
	modified_julian_date = year_days + month_days + _day - 679019;
	fractional_modified_julian_date = (3600 * _hour + 60 * _minute + _second) / 86400;
}


Datetime Datetime::modified_julian_datetime_to_civil_datetime(unsigned int modified_julian_date, double fractional_modified_julian_date)
/*
solid.f [LN 1182–1189]
```
|      subroutine mjdciv(mjd,fmjd,iyr,imo,idy,ihr,imn,sec)
|
|*** convert modified julian date to civil date
|
|*** imo in range 1-12, idy in range 1-31
|*** only valid in range mar-1900 thru feb-2100
|*** ref: hofmann-wellenhof, 2nd ed., pg 34-35
|*** operation confirmed for leap years (incl. year 2000)
```
mjd — 
fmjd — 
*iyr — _year
*imo — _month
*idy — _day
*ihr — _hour
*imn — _minute
*sec — _second
*/
{
	/*
	solid.f [LN 1193—1198]
	```
	|      rjd=mjd+fmjd+2400000.5d0
	|      ia=(rjd+0.5d0)
	|      ib=ia+1537
	|      ic=(ib-122.1d0)/365.25d0
	|      id=365.25d0*ic
	|      ie=(ib-id)/30.6001d0
	```
	rjd — julian_date
	*/
	double julian_date = modified_julian_date + fractional_modified_julian_date + 2400000.5;
	int ia = julian_date + 0.5;
	int ib = ia + 1537;
	int ic = (ib - 122.1) / 365.25;
	int id = 365.25 * ic;
	int ie = (ib - id) / 30.6001;

	/*
	solid.f [LN 1200—1208]
	```
	|*** the fractional part of a julian day is fractional mjd + 0.5
	|*** therefore, fractional part of julian day + 0.5 is fractional mjd
	|
	|      it1=ie*30.6001d0
	|      idy=ib-id-it1+fmjd
	|      it2=ie/14.d0
	|      imo=ie-1-12*it2
	|      it3=(7+imo)/10.d0
	|      iyr=ic-4715-it3
	```
	*/
	int day = ib - id - (ie * 30.6001) + fractional_modified_julian_date;
	int month = ie - 1 - 12 * (ie / 14.0);
	int year = ic - 4715 - ((7 + month) / 10.0);

	/*
	solid.f [LN 12—12]
	```
	|      tmp=fmjd*24.d0
	|      ihr=tmp
	|      tmp=(tmp-ihr)*60.d0
	|      imn=tmp
	|      sec=(tmp-imn)*60.d0
	```
	*/
	double temp_hour = fractional_modified_julian_date * 24.0;
	int hour = temp_hour;
	double temp_minute = (temp_hour - hour) * 60.0;
	int minute = temp_minute;
	int second = (temp_minute - minute) * 60.0;

	return Datetime(year, month, day, hour, minute, second);
}


unsigned int Datetime::initial_modified_julian_date()
/*
solid.f [LN 1044–1048...1052–1053]
```
|      subroutine setjd0(iyr,imo,idy)
|
|*** set the integer part of a modified julian date as epoch, mjd0
|*** the modified julian day is derived from civil time as in civmjd()
|*** allows single number expression of time in seconds w.r.t. mjd0
⋮
|      save /mjdoff/
|      common/mjdoff/mjd0
```
iyr — _year
imo — _month
idy — _day
*/
{
	/*
	solid.f [LN 1055–1063]
	```
	|      if(iyr.lt.1900) stop 34587
	|
	|      if(imo.le.2) then
	|        y=iyr-1
	|        m=imo+12
	|      else
	|        y=iyr
	|        m=imo
	|      endif
	```
	*/
	assert(_year >= 1900);

	unsigned int year = _year;
	unsigned int month = _month;
	if(_month <= 2)
	{
		year--;
		month += 12;
	}

	/*
	solid.f [LN 1065–1067...1071]
	```
	|      it1=365.25d0*y
	|      it2=30.6001d0*(m+1)
	|      mjd=it1+it2+idy-679019
	⋮
	|      mjd0=mjd
	```
	*/
	unsigned int it1 = 365.25 * year;
	unsigned int it2 = 30.6001 * (month + 1);
	unsigned int modified_julian_date = it1 + it2 + _day - 679019;
	return modified_julian_date;
}


double Datetime::to_TerrestrialTime()
/*
```
|*** use TT for solar ephemerides
|
|      leapflag=lflag
|      tsecutc=fmjd*86400.d0                       !*** UTC time (sec of day)
|      tsectt =utc2ttt(tsecutc)                    !*** TT  time (sec of day)
|      fmjdtt =tsectt/86400.d0                     !*** TT  time (fract. day)
|      lflag   = leapflag
|
|*** julian centuries since 1.5 january 2000 (J2000)
|***   (note: also low precision use of mjd --> tjd)
|
|      tjdtt = mjd+fmjdtt+2400000.5d0              !*** Julian Date, TT
|      t     = (tjdtt - 2451545.d0)/36525.d0       !*** julian centuries, TT
```
*/
{

}
