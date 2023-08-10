

#include "Geolocation.hpp"


#include "Coordinate.hpp"


Coordinate<double> Geolocation::tidal_displacement(unsigned int initial_modified_julian_date, JulianDate& julian_date)
/*
solid.f [LN 110–150]
```
|      subroutine detide(xsta,mjd,fmjd,xsun,xmon,dxtide,lflag)
|
|*** computation of tidal corrections of station displacements caused
|***    by lunar and solar gravitational attraction
|*** UTC version
|
|*** step 1 (here general degree 2 and 3 corrections +
|***         call st1idiu + call st1isem + call st1l1)
|***   + step 2 (call step2diu + call step2lon + call step2idiu)
|*** it has been decided that the step 3 un-correction for permanent tide
|*** would *not* be applied in order to avoid jump in the reference frame
|*** (this step 3 must added in order to get the mean tide station position
|*** and to be conformed with the iag resolution.)
|
|*** inputs
|***   xsta(i),i=1,2,3   -- geocentric position of the station (ITRF/ECEF)
|***   xsun(i),i=1,2,3   -- geoc. position of the sun (ECEF)
|***   xmon(i),i=1,2,3   -- geoc. position of the moon (ECEF)
|***   mjd,fmjd          -- modified julian day (and fraction) (in UTC time)
|
|****old calling sequence*****************************************************
|***   dmjd               -- time in mean julian date (including day fraction)
|***   fhr=hr+zmin/60.+sec/3600.   -- hr in the day
|
|*** outputs
|***   dxtide(i),i=1,2,3  -- displacement vector (ITRF)
|***   lflag              -- leap second table limit flag, false:flag not raised
|
|*** author iers 1996 :  v. dehant, s. mathews and j. gipson
|***    (test between two subroutines)
|*** author iers 2000 :  v. dehant, c. bruyninx and s. mathews
|***    (test in the bernese program by c. bruyninx)
|
|*** created:  96/03/23 (see above)
|*** modified from dehanttideinelMJD.f by Dennis Milbert 2006sep10
|*** bug fix regarding fhr (changed calling sequence, too)
|*** modified to reflect table 7.5a and b IERS Conventions 2003
|*** modified to use TT time system to call step 2 functions
|*** sign correction by V.Dehant to match eq.16b, p.81, Conventions
|*** applied by Dennis Milbert 2007may05
|*** UTC version by Dennis Milbert 2018june01
```
*/
{
	Coordinate<double> solar_coordinates = sun_coordinates(initial_modified_julian_date, julian_date);
	Coordinate<double> lunar_coordainate = moon_coordinates(initial_modified_julian_date, julian_date);

	/*
	```
	|*** nominal second degree and third degree love numbers and shida numbers
	|
	|      data h20/0.6078d0/,l20/0.0847d0/,h3/0.292d0/,l3/0.015d0/
	|
	|*** internal support for new calling sequence
	|*** first, convert UTC time into TT time (and, bring leapflag into variable)
	|
	|      leapflag=lflag
	|      tsecutc =fmjd*86400.d0                       !*** UTC time (sec of day)
	|      tsectt  =utc2ttt(tsecutc)                    !*** TT  time (sec of day)
	|      fmjdtt  =tsectt/86400.d0                     !*** TT  time (fract. day)
	|      lflag   = leapflag
	|
	|      dmjdtt=mjd+fmjdtt                           !*** float MJD in TT
	|*** commented line was live code in dehanttideinelMJD.f
	|*** changed on the suggestion of Dr. Don Kim, UNB -- 09mar21
	|*** Julian date for 2000 January 1 00:00:00.0 UT is  JD 2451544.5
	|*** MJD         for 2000 January 1 00:00:00.0 UT is MJD   51544.0
	|***** t=(dmjdtt-51545.d0)/36525.d0                !*** days to centuries, TT
	|      t=(dmjdtt-51544.d0)/36525.d0                !*** days to centuries, TT
	|      fhr=(dmjdtt-int(dmjdtt))*24.d0              !*** hours in the day, TT
	```
	dmjdtt — terrestrial_time_days
	t — terrestrial_time_years
	fhr — terrestrial_time_hours
	*/
	double terrestrial_time_days = julian_date.TerrestrialTime(initial_modified_julian_date);
	double terrestrial_time_years = (terrestrial_time - 51544.0) / 36525.0;
	double terrestrial_time_hours = (terrestrial_time - (int)terrestrial_time) * 24.0;

	/*
	```
	|*** scalar product of station vector with sun/moon vector
	|
	|      call sprod(xsta,xsun,scs,rsta,rsun)
	|      call sprod(xsta,xmon,scm,rsta,rmon)
	|      scsun=scs/rsta/rsun
	|      scmon=scm/rsta/rmon
	```
	*/
	Coordinate<double> geo_coordinate = (Coordinate<double>)*this;
	double geo_coordinates_sqrt  = sqrt(geo_coordinate * geo_coordinate);
	double solar_coordinates_sqrt = sqrt(solar_coordinates * solar_coordinates);
	double solar_scalar = geo_coordinate * solar_coordinates;

	/*
	```
	|*** computation of new h2 and l2
	|
	|      cosphi=dsqrt(xsta(1)*xsta(1) + xsta(2)*xsta(2))/rsta
	|      h2=h20-0.0006d0*(1.d0-3.d0/2.d0*cosphi*cosphi)
	|      l2=l20+0.0002d0*(1.d0-3.d0/2.d0*cosphi*cosphi)
	|
	|*** p2-term
	|
	|      p2sun=3.d0*(h2/2.d0-l2)*scsun*scsun-h2/2.d0
	|      p2mon=3.d0*(h2/2.d0-l2)*scmon*scmon-h2/2.d0
	|
	|*** p3-term
	|
	|      p3sun=5.d0/2.d0*(h3-3.d0*l3)*scsun**3+3.d0/2.d0*(l3-h3)*scsun
	|      p3mon=5.d0/2.d0*(h3-3.d0*l3)*scmon**3+3.d0/2.d0*(l3-h3)*scmon
	|
	|*** term in direction of sun/moon vector
	|
	|      x2sun=3.d0*l2*scsun
	|      x2mon=3.d0*l2*scmon
	|      x3sun=3.d0*l3/2.d0*(5.d0*scsun*scsun-1.d0)
	|      x3mon=3.d0*l3/2.d0*(5.d0*scmon*scmon-1.d0)
	|
	|*** factors for sun/moon
	|
	|      mass_ratio_sun=332945.943062d0
	|      mass_ratio_moon=0.012300034d0
	|      re =6378136.55d0
	|      fac2sun=mass_ratio_sun*re*(re/rsun)**3
	|      fac2mon=mass_ratio_moon*re*(re/rmon)**3
	|      fac3sun=fac2sun*(re/rsun)
	|      fac3mon=fac2mon*(re/rmon)
	|
	|*** total displacement
	|
	|      do i=1,3
	|        dxtide(i)=fac2sun*( x2sun*xsun(i)/rsun + p2sun*xsta(i)/rsta ) +
	|     *            fac2mon*( x2mon*xmon(i)/rmon + p2mon*xsta(i)/rsta ) +
	|     *            fac3sun*( x3sun*xsun(i)/rsun + p3sun*xsta(i)/rsta ) +
	|     *            fac3mon*( x3mon*xmon(i)/rmon + p3mon*xsta(i)/rsta )
	|      enddo
	|      call zero_vec8(xcorsta)
	|
	|*** corrections for the out-of-phase part of love numbers
	|***     (part h_2^(0)i and l_2^(0)i )
	|
	|*** first, for the diurnal band
	|
	|      call st1idiu(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	|
	|*** second, for the semi-diurnal band
	|
	|      call st1isem(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	|
	|*** corrections for the latitude dependence of love numbers (part l^(1) )
	|
	|      call st1l1(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	|
	|*** consider corrections for step 2
	|*** corrections for the diurnal band:
	|
	|***  first, we need to know the date converted in julian centuries
	|
	|***  this is now handled at top of code   (also convert to TT time system)
	|***** t=(dmjd-51545.)/36525.
	|***** fhr=dmjd-int(dmjd)             !*** this is/was a buggy line (day vs. hr)
	|
	|***  second, the diurnal band corrections,
	|***   (in-phase and out-of-phase frequency dependence):
	|
	|      call step2diu(xsta,fhr,t,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	|
	|***  corrections for the long-period band,
	|***   (in-phase and out-of-phase frequency dependence):
	|
	|      call step2lon(xsta,fhr,t,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	|
	|*** consider corrections for step 3
	|*-----------------------------------------------------------------------
	|* The code below is commented to prevent restoring deformation
	|* due to permanent tide.  All the code above removes
	|* total tidal deformation with conventional Love numbers.
	|* The code above realizes a conventional tide free crust (i.e. ITRF). 
	|* This does NOT conform to Resolution 16 of the 18th General Assembly
	|* of the IAG (1983).  This resolution has not been implemented by 
	|* the space geodesy community in general (c.f. IERS Conventions 2003).
	|*-----------------------------------------------------------------------
	|
	|*** uncorrect for the permanent tide  (only if you want mean tide system)
	|
	|***   pi=3.141592654
	|***   sinphi=xsta(3)/rsta
	|***   cosphi=dsqrt(xsta(1)**2+xsta(2)**2)/rsta
	|***   cosla=xsta(1)/cosphi/rsta
	|***   sinla=xsta(2)/cosphi/rsta
	|***   dr=-dsqrt(5./4./pi)*h2*0.31460*(3./2.*sinphi**2-0.5)
	|***   dn=-dsqrt(5./4./pi)*l2*0.31460*3.*cosphi*sinphi
	|***   dxtide(1)=dxtide(1)-dr*cosla*cosphi+dn*cosla*sinphi
	|***   dxtide(2)=dxtide(2)-dr*sinla*cosphi+dn*sinla*sinphi
	|***   dxtide(3)=dxtide(3)-dr*sinphi      -dn*cosphi
	|
	|      return
	|      end
	```
	*/
}