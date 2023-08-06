

#include "Datetime.hpp"


#include "JulianDate.hpp"


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


Datetime::operator JulianDate()
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
	unsigned int modified_julian_date = year_days + month_days + _day - 679019;
	double fractional_modified_julian_date = (3600 * _hour + 60 * _minute + _second) / 86400;

	return JulianDate(modified_julian_date, fractional_modified_julian_date);
}
