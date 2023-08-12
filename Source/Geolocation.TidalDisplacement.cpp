

#include "Geolocation.hpp"


#include "Coordinate.hpp"
#include "JulianDate.hpp"


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
xsta — geo_coordinate
mjd — julian_date
fmjd — julian_date
xsun — solar_coordinate
xmon — lunar_coordinate
dxtide — 
lflag — 
*/
{
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
	double terrestrial_time_years = (terrestrial_time_days - 51544.0) / 36525.0;
	double terrestrial_time_hours = (terrestrial_time_days - (int)terrestrial_time_days) * 24.0;

	/*
	```
	|*** scalar product of station vector with sun/moon vector
	|
	|      call sprod(xsta,xsun,scs,rsta,rsun)
	|      call sprod(xsta,xmon,scm,rsta,rmon)
	|      scsun=scs/rsta/rsun
	|      scmon=scm/rsta/rmon
	```
	xsta — geo_coordinate
	xsun — solar_coordinate
	scs — solar_scalar
	rsta — geo_distance
	rsun — solar_distance
	rmon — lunar_distance
	scsun — solar_sc
	scmon — lunar_sc
	*/
	Coordinate<double> geo_coordinate = (Coordinate<double>)*this;
	Coordinate<double> solar_coordinate = sun_coordinates(initial_modified_julian_date, julian_date);
	Coordinate<double> lunar_coordinate = moon_coordinates(initial_modified_julian_date, julian_date);

	double geo_distance  = sqrt(geo_coordinate * geo_coordinate);
	double solar_distance = sqrt(solar_coordinate * solar_coordinate);
	double lunar_distance = sqrt(lunar_coordinate * lunar_coordinate);

	double solar_scalar = geo_coordinate * solar_coordinate;
	double lunar_scalar = geo_coordinate * lunar_coordinate;

	double solar_sc = solar_scalar / geo_distance / solar_distance;
	double lunar_sc = lunar_scalar / geo_distance / lunar_distance;

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
	```
	cosphi — cos_ϕ
	h2 — second_degree_love
	l2 — second_degree_shida
	p2sun — solar_p2
	p2mon — lunar_p2
	p3sun — solar_p3
	p3mon — lunar_p3
	*/
	double cos_ϕ = sqrt(geo_coordinate[Y] * geo_coordinate[Y] + geo_coordinate[Z] * geo_coordinate[Z]) / geo_distance;
	double second_degree_love = SECOND_DEGREE_LOVE - 0.0006 * (1.0 - 3.0 / 2.0 * cos_ϕ * cos_ϕ);
	double second_degree_shida = SECOND_DEGREE_SHIDA + 0.0002 * (1.0 - 3.0 / 2.0 * cos_ϕ * cos_ϕ);

	double p2_pre_op = 3.0 * (second_degree_love / 2.0 - second_degree_shida);
	double solar_p2 = p2_pre_op * pow(solar_sc, 2) - second_degree_love / 2.0;
	double lunar_p2 = p2_pre_op * pow(lunar_sc, 2) - second_degree_love / 2.0;

	double p3_pre_op = 2.5 * (THIRD_DEGREE_LOVE - 3.0 * THIRD_DEGREE_SHIDA);
	double solar_p3 = p3_pre_op * pow(solar_sc, 3) + 1.5 * THIRD_DEGREE_SHIDA - THIRD_DEGREE_LOVE * solar_sc;
	double lunar_p3 = p3_pre_op * pow(lunar_sc, 3) + 1.5 * THIRD_DEGREE_SHIDA - THIRD_DEGREE_LOVE * lunar_sc;

	/*
	```
	|*** term in direction of sun/moon vector
	|
	|      x2sun=3.d0*l2*scsun
	|      x2mon=3.d0*l2*scmon
	|      x3sun=3.d0*l3/2.d0*(5.d0*scsun*scsun-1.d0)
	|      x3mon=3.d0*l3/2.d0*(5.d0*scmon*scmon-1.d0)
	```
	x2sun — solar_direction2
	x2mon — lunar_direction2
	x3sun — solar_direction3
	x3mon — lunar_direction3
	*/
	double solar_direction2 = 3.0 * second_degree_shida * solar_sc;
	double lunar_direction2 = 3.0 * second_degree_shida * lunar_sc;
	double solar_direction3 = 3.0 * THIRD_DEGREE_SHIDA / 2.0 * (5.0 * pow(solar_sc, 2) - 1.0);
	double lunar_direction3 = 3.0 * THIRD_DEGREE_SHIDA / 2.0 * (5.0 * pow(lunar_sc, 2) - 1.0);

	/*
	```
	|*** factors for sun/moon
	|
	|      mass_ratio_sun=332945.943062d0
	|      mass_ratio_moon=0.012300034d0
	|      re =6378136.55d0
	|      fac2sun=mass_ratio_sun*re*(re/rsun)**3
	|      fac2mon=mass_ratio_moon*re*(re/rmon)**3
	|      fac3sun=fac2sun*(re/rsun)
	|      fac3mon=fac2mon*(re/rmon)
	```
	fac2sun — solar_factor2
	fac2mon — lunar_factor2
	fac3sun — solar_factor3
	fac3mon — lunar_factor3
	*/
	double solar_factor2 = SOLAR_MASS_RATIO * RE * pow(RE / solar_distance, 3);
	double lunar_factor2 = LUNAR_MASS_RATIO * RE * pow(RE / lunar_distance, 3);
	double solar_factor3 = solar_factor2 * RE / solar_distance;
	double lunar_factor3 = lunar_factor2 * RE / lunar_distance;

	/*
	```
	|*** total displacement
	|
	|      do i=1,3
	|        dxtide(i)=fac2sun*( x2sun*xsun(i)/rsun + p2sun*xsta(i)/rsta ) +
	|     *            fac2mon*( x2mon*xmon(i)/rmon + p2mon*xsta(i)/rsta ) +
	|     *            fac3sun*( x3sun*xsun(i)/rsun + p3sun*xsta(i)/rsta ) +
	|     *            fac3mon*( x3mon*xmon(i)/rmon + p3mon*xsta(i)/rsta )
	|      enddo
	|      call zero_vec8(xcorsta)
	```
	*/
	Coordinate<double> detide;
	for(unsigned int index = 0; index < 3; index++)
	{
		detide[index] =
			solar_factor2 * (solar_direction2 * solar_coordinate[index] / solar_distance
				+ solar_p2 * geo_coordinate[index] / geo_distance)
			+ lunar_factor2 * (lunar_direction2 * lunar_coordinate[index] / lunar_distance
				+ lunar_p2 * geo_coordinate[index] / geo_distance)
			+ solar_factor3 * (solar_direction3 * solar_coordinate[index] / solar_distance
				+ solar_p3 * geo_coordinate[index] / geo_distance)
			+ lunar_factor3 * (lunar_direction3 * lunar_coordinate[index] / lunar_distance
				+ lunar_p3 * geo_coordinate[index] / geo_distance);
	}

	/*
	```
	|*** corrections for the out-of-phase part of love numbers
	|***     (part h_2^(0)i and l_2^(0)i )
	|
	|*** first, for the diurnal band
	|
	|      call st1idiu(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	```
	*/
	Coordinate<double> corrected_geo_coordinate = mantle_inelasticity_diurnal_band_correction(geo_coordinate,
		solar_coordinate, lunar_coordinate, solar_factor2, lunar_factor2);
	detide += corrected_geo_coordinate;

	/*
	```
	|*** second, for the semi-diurnal band
	|
	|      call st1isem(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	```
	*/

	/*
	```
	|*** corrections for the latitude dependence of love numbers (part l^(1) )
	|
	|      call st1l1(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	```
	*/

	/*
	```
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


Coordinate<double> mantle_inelasticity_diurnal_band_correction(Coordinate<double> geo_coordinate, 
	Coordinate<double> solar_coordinate, Coordinate<double> lunar_coordinate, double solar_factor2, double lunar_factor2 
)
/*
```
|      subroutine st1idiu(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
|
|*** this subroutine gives the out-of-phase corrections induced by
|*** mantle inelasticity in the diurnal band
|
|***  input: xsta,xsun,xmon,fac2sun,fac2mon
|*** output: xcorsta
```
xsta — geo_coordinate
xsun — solar_coordinate
xmon — lunar_coordinate
fac2sun — solar_factor2
fac2mon — lunar_factor2
xcorsta — [returned]
*/
{
	/*
	solid.f [LN ]
	```
	|      rsta=enorm8(xsta)
	|      sinphi=xsta(3)/rsta
	|      cosphi=dsqrt(xsta(1)**2+xsta(2)**2)/rsta
	|      cos2phi=cosphi**2-sinphi**2
	|      sinla=xsta(2)/cosphi/rsta
	|      cosla=xsta(1)/cosphi/rsta
	|      rmon=enorm8(xmon)
	|      rsun=enorm8(xsun)
	```
	rsta — geo_distance
	sinphi — sin_ϕ
	cosphi — cos_ϕ
	cos2phi — cos_squared_ϕ
	sinla — sin_latitude
	cosla — cos_latitude
	rmon — lunar_distance
	rsun — solar_distance
	*/
	double geo_distance = geo_coordinate.distance();
	double sin_ϕ = geo_coordinate[Z] / geo_distance;
	double cos_ϕ = sqrt(pow(geo_coordinate[X], 2) + pow(geo_coordinate[Y], 2)) / geo_distance;
	double cos_squared_ϕ = pow(cos_ϕ, 2) - pow(sin_ϕ, 2);
	double sin_latitude = geo_coordinate[Y] / cos_ϕ / geo_distance;
	double cos_latitude = geo_coordinate[X] / cos_ϕ / geo_distance;
	double lunar_distance = lunar_coordinate.distance();
	double solar_distance = solar_coordinate.distance();

	/*
	```
	|      drsun=-3.d0*dhi*sinphi*cosphi*fac2sun*xsun(3)*(xsun(1)*
	|     *            sinla-xsun(2)*cosla)/rsun**2
	|      drmon=-3.d0*dhi*sinphi*cosphi*fac2mon*xmon(3)*(xmon(1)*
	|     *            sinla-xmon(2)*cosla)/rmon**2
	|      dnsun=-3.d0*dli*cos2phi*fac2sun*xsun(3)*(xsun(1)*sinla-
	|     *            xsun(2)*cosla)/rsun**2
	|      dnmon=-3.d0*dli*cos2phi*fac2mon*xmon(3)*(xmon(1)*sinla-
	|     *            xmon(2)*cosla)/rmon**2
	|      desun=-3.d0*dli*sinphi*fac2sun*xsun(3)*
	|     * (xsun(1)*cosla+xsun(2)*sinla)/rsun**2
	|      demon=-3.d0*dli*sinphi*fac2mon*xmon(3)*
	|     * (xmon(1)*cosla+xmon(2)*sinla)/rmon**2
	|      dr=drsun+drmon
	|      dn=dnsun+dnmon
	|      de=desun+demon
	|      xcorsta(1)=dr*cosla*cosphi-de*sinla-dn*sinphi*cosla
	|      xcorsta(2)=dr*sinla*cosphi+de*cosla-dn*sinphi*sinla
	|      xcorsta(3)=dr*sinphi               +dn*cosphi
	```
	*/
	double solar_dr = -3.0 * -0.0022 * sin_ϕ * cos_ϕ * solar_factor2 * solar_coordinate[Z]
		* (solar_coordinate[X] * sin_latitude - solar_coordinate[Y] * cos_latitude) / pow(solar_distance, 2);
	double lunar_dr = -3.0 * -0.0022 * sin_ϕ * cos_ϕ * lunar_factor2 * lunar_coordinate[Z]
		* (lunar_coordinate[X] * sin_latitude - lunar_coordinate[Y] * cos_latitude) / pow(lunar_distance, 2);
	double solar_dn = -3.0 * -0.0007 * cos_squared_ϕ * solar_factor2 * solar_coordinate[Z]
		* (solar_coordinate[X] * sin_latitude - solar_coordinate[Y] * cos_latitude) / pow(solar_distance, 2);
	double lunar_dn = -3.0 * -0.0007 * cos_squared_ϕ * lunar_factor2 * lunar_coordinate[Z]
		* (lunar_coordinate[X] * sin_latitude - lunar_coordinate[Y] * cos_latitude) / pow(lunar_distance, 2);
	double solar_de = -3.0 * -0.0007 * sin_ϕ * solar_factor2 * solar_coordinate[Z]
		* (solar_coordinate[X] * cos_latitude * solar_coordinate[Y] * sin_latitude) / pow(solar_distance, 2);
	double lunar_de = -3.0 * -0.0007 * sin_ϕ * lunar_factor2 * lunar_coordinate[Z]
		* (lunar_coordinate[X] * cos_latitude * lunar_coordinate[Y] * sin_latitude) / pow(lunar_distance, 2);

	double dr = solar_dr + lunar_dr;
	double dn = solar_dn + lunar_dn;
	double de = solar_de + lunar_de;
	return Coordinate<double>(
		dr * cos_latitude * cos_ϕ - de * sin_latitude - dn * sin_ϕ * cos_latitude,
		dr * sin_latitude * cos_ϕ + de * cos_latitude - dn * sin_ϕ * sin_latitude,
		dr * sin_ϕ + dn * cos_ϕ
	);
}


Coordinate<double> 
/*
```
|      subroutine st1isem(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
|
|*** this subroutine gives the out-of-phase corrections induced by
|*** mantle inelasticity in the diurnal band
|
|***  input: xsta,xsun,xmon,fac2sun,fac2mon
|*** output: xcorsta
```
*/
{
	/*
	```
	|      implicit double precision (a-h,o-z)
	|      dimension xsta(3),xsun(3),xmon(3),xcorsta(3)
	|      data dhi/-0.0022d0/,dli/-0.0007d0/
	|
	|      rsta=enorm8(xsta)
	|      sinphi=xsta(3)/rsta
	|      cosphi=dsqrt(xsta(1)**2+xsta(2)**2)/rsta
	|      sinla=xsta(2)/cosphi/rsta
	|      cosla=xsta(1)/cosphi/rsta
	|      costwola=cosla**2-sinla**2
	|      sintwola=2.d0*cosla*sinla
	|      rmon=enorm8(xmon)
	|      rsun=enorm8(xsun)
	|      drsun=-3.d0/4.d0*dhi*cosphi**2*fac2sun*((xsun(1)**2-xsun(2)**2)*
	|     * sintwola-2.*xsun(1)*xsun(2)*costwola)/rsun**2
	|      drmon=-3.d0/4.d0*dhi*cosphi**2*fac2mon*((xmon(1)**2-xmon(2)**2)*
	|     * sintwola-2.*xmon(1)*xmon(2)*costwola)/rmon**2
	|      dnsun=1.5d0*dli*sinphi*cosphi*fac2sun*((xsun(1)**2-xsun(2)**2)*
	|     * sintwola-2.d0*xsun(1)*xsun(2)*costwola)/rsun**2
	|      dnmon=1.5d0*dli*sinphi*cosphi*fac2mon*((xmon(1)**2-xmon(2)**2)*
	|     * sintwola-2.d0*xmon(1)*xmon(2)*costwola)/rmon**2
	|      desun=-3.d0/2.d0*dli*cosphi*fac2sun*((xsun(1)**2-xsun(2)**2)*
	|     * costwola+2.*xsun(1)*xsun(2)*sintwola)/rsun**2
	|      demon=-3.d0/2.d0*dli*cosphi*fac2mon*((xmon(1)**2-xmon(2)**2)*
	|     * costwola+2.d0*xmon(1)*xmon(2)*sintwola)/rmon**2
	|      dr=drsun+drmon
	|      dn=dnsun+dnmon
	|      de=desun+demon
	|      xcorsta(1)=dr*cosla*cosphi-de*sinla-dn*sinphi*cosla
	|      xcorsta(2)=dr*sinla*cosphi+de*cosla-dn*sinphi*sinla
	|      xcorsta(3)=dr*sinphi+dn*cosphi
	|
	|      return
	|      end
	```
	*/
}