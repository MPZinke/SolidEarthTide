

#include "JulianDate.hpp"


#include "Datetime.hpp"


JulianDate::JulianDate(unsigned int modified_julian_date, double fractional_modified_julian_date)
: _modified_julian_date{modified_julian_date}, _fractional_modified_julian_date{fractional_modified_julian_date}
{}


JulianDate::operator Datetime()
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
	double julian_date = _modified_julian_date + _fractional_modified_julian_date + 2400000.5;
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
	int day = ib - id - (ie * 30.6001) + _fractional_modified_julian_date;
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
	double temp_hour = _fractional_modified_julian_date * 24.0;
	int hour = temp_hour;
	double temp_minute = (temp_hour - hour) * 60.0;
	int minute = temp_minute;
	int second = (temp_minute - minute) * 60.0;

	return Datetime(year, month, day, hour, minute, second);
}


double JulianDate::to_TerrestrialTime()
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
	// double time_seconds_UTC = 
}
