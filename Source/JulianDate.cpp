

#include "JulianDate.hpp"


#include <iostream>


#include "Datetime.hpp"


const unsigned int JulianDate::MJDUPPER = 58664;
const unsigned int JulianDate::MJDLOWER = 41317;


JulianDate::JulianDate(unsigned int modified_julian_date, double fractional_modified_julian_date)
: _modified_julian_date{modified_julian_date}, _fractional_modified_julian_date{fractional_modified_julian_date}
{}


unsigned int JulianDate::modified_julian_date()
{
	return _modified_julian_date;
}


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
{
	/*
	```
	|*** use TT for solar ephemerides
	|
	|      leapflag=lflag
	|      tsecutc=fmjd*86400.d0                       !*** UTC time (sec of day)
	|      tsectt =utc2ttt(tsecutc)                    !*** TT  time (sec of day)
	|      fmjdtt =tsectt/86400.d0                     !*** TT  time (fract. day)
	|      lflag   = leapflag
	```
	tsecutc — time_seconds_UTC
	tsectt — time_seconds_TerrestrialTime
	fmjdtt — fractional_modified_julian_date_TT
	*/
	double time_seconds_UTC = _fractional_modified_julian_date * 86400.0;
	double time_seconds_TerrestrialTime = UTC_to_TerrestrialTime(time_seconds_UTC);
	double fractional_modified_julian_date_TT = time_seconds_TerrestrialTime / 86400.0;

	/*
	```
	|*** julian centuries since 1.5 january 2000 (J2000)
	|***   (note: also low precision use of mjd --> tjd)
	|
	|      tjdtt = mjd+fmjdtt+2400000.5d0              !*** Julian Date, TT
	|      t     = (tjdtt - 2451545.d0)/36525.d0       !*** julian centuries, TT
	```
	t — TerrestrialTime_seconds
	*/
	double tjdtt = _modified_julian_date + fractional_modified_julian_date_TT + 2400000.5;
	double TerrestrialTime_seconds = (tjdtt - 2451545.0) / 36525.0;
	return TerrestrialTime_seconds;
}


double JulianDate::to_TerrestrialTime(double time_seconds_UTC)
/*
solid.f [LN 1221–1228]
|```
|      double precision function utc2ttt(tutc)
|
|*** convert utc (sec) to terrestrial time (sec)
|
|      implicit double precision(a-h,o-z)
|
|      ttai   = utc2tai(tutc)
|      utc2ttt= tai2tt(ttai)
```
tutc — time_seconds_UTC
ttai — time_seconds_TAI
utc2ttt — time_seconds_TerrestrialTime
*/
{
	double time_seconds_TAI = UTC_to_TAI(time_seconds_UTC);

	/*
	solid.f [LN 415–417...421–422]
	```
	|      double precision function tai2tt(ttai)
	|
	|*** convert tai (sec) to terrestrial time (sec)
	⋮
	|***** http://tycho.usno.navy.mil/systime.html
	|      tai2tt = ttai + 32.184d0
	```
	ttai — time_seconds_TAI
	tai2tt — time_seconds_TerrestrialTime
	*/
	double time_seconds_TerrestrialTime = time_seconds_TAI + 32.184;

	return time_seconds_TerrestrialTime;
}


double JulianDate::UTC_to_TAI(double time_seconds_UTC)
/*
solid.f [LN 1245–1254]
```
|      double precision function utc2tai(tutc)
|
|*** convert utc (sec) to tai (sec)
|
|      implicit double precision(a-h,o-z)
|
|      utc2tai = tutc - getutcmtai(tutc)
|
|      return
|      end
```
tutc — time_seconds_UTC
mjd0 — initial_modified_julian_date
*/
{
	/*
	solid.f [LN 1256–1266]
	```
	|      double precision function getutcmtai(tsec)
	|
	|*** get utc - tai (s)
	|
	|***** "Julian Date Converter"
	|***** http://aa.usno.navy.mil/data/docs/JulianDate.php
	⋮
	|      parameter(MJDUPPER=58664)    !*** upper limit, leap second table, 2019jun30
	|***** parameter(MJDUPPER=58299)    !*** upper limit, leap second table, 2018jun30
	|      parameter(MJDLOWER=41317)    !*** lower limit, leap second table, 1972jan01
	```
	*/

	/*
	solid.f [LN 1277–1290]
	```
	|      ttsec=tsec
	|      mjd0t=mjd0
	|
	|    1 if(ttsec.ge.86400.d0) then
	|        ttsec=ttsec-86400.d0
	|        mjd0t=mjd0t+1
	|        go to 1
	|      endif
	|
	|    2 if(ttsec.lt.0.d0) then
	|        ttsec=ttsec+86400.d0
	|        mjd0t=mjd0t-1
	|        go to 2
	|      endif
	```
	*/
	unsigned int initial_modified_julian_date;
	for(int limit = 365*2100; time_seconds_UTC >= 86400.0 && limit > 0; limit--)
	{
		time_seconds_UTC -= 86400.0;
		initial_modified_julian_date++;
	}

	for(int limit = 365*2100; time_seconds_UTC < 0.0 && limit > 0; limit--)
	{
		time_seconds_UTC += 86400.0;
		initial_modified_julian_date--;
	}

	/*
	```
	|*** test upper table limit         (upper limit set by bulletin C memos)
	|
	|      if(mjd0t.gt.MJDUPPER) then
	|        leapflag  =.true.               !*** true means flag *IS* raised
	|        getutcmtai= -37.d0              !*** return the upper table value
	|        return
	|      endif
	|
	|*** test lower table limit
	|
	|      if(mjd0t.lt.MJDLOWER) then
	|        leapflag=.true.                 !*** true means flag *IS* raised
	|        getutcmtai= -10.d0              !*** return the lower table value
	|        return
	|      endif

	```
	*/
	if(initial_modified_julian_date > JulianDate::MJDUPPER)
	{
		//TODO: LEAPFLAG
		return -37.0;
	}

	if(initial_modified_julian_date < MJDLOWER)
	{
		//TODO: LEAPFLAG
		return -10.0;
	}

	/*
	solid.f [LN 1308–1410]
	```
	|***** http://maia.usno.navy.mil/ser7/tai-utc.dat
	|*** 1972 JAN  1 =JD 2441317.5  TAI-UTC=  10.0s
	|*** 1972 JUL  1 =JD 2441499.5  TAI-UTC=  11.0s
	|*** 1973 JAN  1 =JD 2441683.5  TAI-UTC=  12.0s
	|*** 1974 JAN  1 =JD 2442048.5  TAI-UTC=  13.0s
	|*** 1975 JAN  1 =JD 2442413.5  TAI-UTC=  14.0s
	|*** 1976 JAN  1 =JD 2442778.5  TAI-UTC=  15.0s
	|*** 1977 JAN  1 =JD 2443144.5  TAI-UTC=  16.0s
	|*** 1978 JAN  1 =JD 2443509.5  TAI-UTC=  17.0s
	|*** 1979 JAN  1 =JD 2443874.5  TAI-UTC=  18.0s
	|*** 1980 JAN  1 =JD 2444239.5  TAI-UTC=  19.0s
	|*** 1981 JUL  1 =JD 2444786.5  TAI-UTC=  20.0s
	|*** 1982 JUL  1 =JD 2445151.5  TAI-UTC=  21.0s
	|*** 1983 JUL  1 =JD 2445516.5  TAI-UTC=  22.0s
	|*** 1985 JUL  1 =JD 2446247.5  TAI-UTC=  23.0s
	|*** 1988 JAN  1 =JD 2447161.5  TAI-UTC=  24.0s
	|*** 1990 JAN  1 =JD 2447892.5  TAI-UTC=  25.0s
	|*** 1991 JAN  1 =JD 2448257.5  TAI-UTC=  26.0s
	|*** 1992 JUL  1 =JD 2448804.5  TAI-UTC=  27.0s
	|*** 1993 JUL  1 =JD 2449169.5  TAI-UTC=  28.0s
	|*** 1994 JUL  1 =JD 2449534.5  TAI-UTC=  29.0s
	|*** 1996 JAN  1 =JD 2450083.5  TAI-UTC=  30.0s
	|*** 1997 JUL  1 =JD 2450630.5  TAI-UTC=  31.0s
	|*** 1999 JAN  1 =JD 2451179.5  TAI-UTC=  32.0s
	|*** 2006 JAN  1 =JD 2453736.5  TAI-UTC=  33.0s
	|*** 2009 JAN  1 =JD 2454832.5  TAI-UTC=  34.0s
	|*** 2012 JUL  1 =JD 2456109.5  TAI-UTC=  35.0s
	|*** 2015 JUL  1 =JD 2457204.5  TAI-UTC=  36.0s
	|*** 2017 JAN  1 =JD 2457754.5  TAI-UTC=  37.0s
	|***** other leap second references at:
	|***** http://hpiers.obspm.fr/eoppc/bul/bulc/Leap_Second_History.dat
	|***** http://hpiers.obspm.fr/eoppc/bul/bulc/bulletinc.dat
	|
	|*** test against newest leaps first
	|
	|      if    (mjd0t.ge.57754) then       !*** 2017 JAN 1 = 57754
	|        tai_utc = 37.d0
	|      elseif(mjd0t.ge.57204) then       !*** 2015 JUL 1 = 57204
	|        tai_utc = 36.d0
	|      elseif(mjd0t.ge.56109) then       !*** 2012 JUL 1 = 56109
	|        tai_utc = 35.d0
	|      elseif(mjd0t.ge.54832) then       !*** 2009 JAN 1 = 54832
	|        tai_utc = 34.d0
	|      elseif(mjd0t.ge.53736) then       !*** 2006 JAN 1 = 53736
	|        tai_utc = 33.d0
	|      elseif(mjd0t.ge.51179) then       !*** 1999 JAN 1 = 51179
	|        tai_utc = 32.d0
	|      elseif(mjd0t.ge.50630) then       !*** 1997 JUL 1 = 50630
	|        tai_utc = 31.d0
	|      elseif(mjd0t.ge.50083) then       !*** 1996 JAN 1 = 50083
	|        tai_utc = 30.d0
	|      elseif(mjd0t.ge.49534) then       !*** 1994 JUL 1 = 49534
	|        tai_utc = 29.d0
	|      elseif(mjd0t.ge.49169) then       !*** 1993 JUL 1 = 49169
	|        tai_utc = 28.d0
	|      elseif(mjd0t.ge.48804) then       !*** 1992 JUL 1 = 48804
	|        tai_utc = 27.d0
	|      elseif(mjd0t.ge.48257) then       !*** 1991 JAN 1 = 48257
	|        tai_utc = 26.d0
	|      elseif(mjd0t.ge.47892) then       !*** 1990 JAN 1 = 47892
	|        tai_utc = 25.d0
	|      elseif(mjd0t.ge.47161) then       !*** 1988 JAN 1 = 47161
	|        tai_utc = 24.d0
	|      elseif(mjd0t.ge.46247) then       !*** 1985 JUL 1 = 46247
	|        tai_utc = 23.d0
	|      elseif(mjd0t.ge.45516) then       !*** 1983 JUL 1 = 45516
	|        tai_utc = 22.d0
	|      elseif(mjd0t.ge.45151) then       !*** 1982 JUL 1 = 45151
	|        tai_utc = 21.d0
	|      elseif(mjd0t.ge.44786) then       !*** 1981 JUL 1 = 44786
	|        tai_utc = 20.d0
	|      elseif(mjd0t.ge.44239) then       !*** 1980 JAN 1 = 44239
	|        tai_utc = 19.d0
	|      elseif(mjd0t.ge.43874) then       !*** 1979 JAN 1 = 43874
	|        tai_utc = 18.d0
	|      elseif(mjd0t.ge.43509) then       !*** 1978 JAN 1 = 43509
	|        tai_utc = 17.d0
	|      elseif(mjd0t.ge.43144) then       !*** 1977 JAN 1 = 43144
	|        tai_utc = 16.d0
	|      elseif(mjd0t.ge.42778) then       !*** 1976 JAN 1 = 42778
	|        tai_utc = 15.d0
	|      elseif(mjd0t.ge.42413) then       !*** 1975 JAN 1 = 42413
	|        tai_utc = 14.d0
	|      elseif(mjd0t.ge.42048) then       !*** 1974 JAN 1 = 42048
	|        tai_utc = 13.d0
	|      elseif(mjd0t.ge.41683) then       !*** 1973 JAN 1 = 41683
	|        tai_utc = 12.d0
	|      elseif(mjd0t.ge.41499) then       !*** 1972 JUL 1 = 41499
	|        tai_utc = 11.d0
	|      elseif(mjd0t.ge.41317) then       !*** 1972 JAN 1 = 41317
	|        tai_utc = 10.d0
	|
	|*** should never, ever get here
	|
	|      else
	|        write(*,*) 'FATAL ERROR --'
	|        write(*,*) 'fell thru tests in gpsleap()'
	|        stop 66768
	|      endif
	|
	|*** return utc - tai (in seconds)
	|
	|      getutcmtai = -tai_utc
	```
	*/
	int seconds_for_years[] = {
		57754, 57204, 56109, 54832, 53736, 51179, 50630, 50083, 49534, 49169, 48804, 48257, 47892, 47161, 46247, 45516,
		45151, 44786, 44239, 43874, 43509, 43144, 42778, 42413, 42048, 41683, 41499, 41317
	};
	for(int x = 0; x < sizeof(seconds_for_years) / sizeof(int); x++)
	{
		if(initial_modified_julian_date >= seconds_for_years[x])
		{
			/*
			solid.f [LN 1410]
			```
			|      getutcmtai = -tai_utc
			```
			*/
			double tai_utc = (37.0 - x);
			double getutcmtai = -tai_utc;

			/*
			solid.f [LN 1251]
			```
			|      utc2tai = tutc - getutcmtai(tutc)
			```
			*/
			return time_seconds_UTC - getutcmtai;
		}
	}

	throw std::runtime_error("FATAL ERROR: Fell through tests in GPS leap");
}
