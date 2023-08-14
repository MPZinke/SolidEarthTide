

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
	solid.f [LN –]
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
	solid.f [LN –]
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
	solid.f [LN –]
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
	solid.f [LN –]
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
	solid.f [LN –]
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
	solid.f [LN –]
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
	solid.f [LN –]
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
	Coordinate<double> corrected_geo_coordinate_1st = mantle_inelasticity_1st_diurnal_band_correction(geo_coordinate,
		solar_coordinate, lunar_coordinate, solar_factor2, lunar_factor2);
	detide += corrected_geo_coordinate_1st;

	/*
	solid.f [LN –]
	```
	|*** second, for the semi-diurnal band
	|
	|      call st1isem(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	```
	*/
	Coordinate<double> corrected_geo_coordinate_semi = mantle_inelasticity_semi_diurnal_band_correction(geo_coordinate,
		solar_coordinate, lunar_coordinate, solar_factor2, lunar_factor2);
	detide += corrected_geo_coordinate_semi;

	/*
	solid.f [LN –]
	```
	|*** corrections for the latitude dependence of love numbers (part l^(1) )
	|
	|      call st1l1(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
	|      dxtide(1)=dxtide(1)+xcorsta(1)
	|      dxtide(2)=dxtide(2)+xcorsta(2)
	|      dxtide(3)=dxtide(3)+xcorsta(3)
	```
	*/
	Coordinate<double> corrected_latitude_dependence = latitude_dependence_correction(geo_coordinate,
		solar_coordinate, lunar_coordinate, solar_factor2, lunar_factor2);
	detide += corrected_latitude_dependence;

	/*
	solid.f [LN –]
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


Coordinate<double> Geolocation::mantle_inelasticity_1st_diurnal_band_correction(Coordinate<double>& geo_coordinate, 
	Coordinate<double>& solar_coordinate, Coordinate<double>& lunar_coordinate, double solar_factor2,
	double lunar_factor2 
)
/*
solid.f [LN 589–595]
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
	solid.f [LN 601–608]
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
	solid.f [LN 609–626]
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


Coordinate<double> Geolocation::mantle_inelasticity_semi_diurnal_band_correction(Coordinate<double>& geo_coordinate, 
	Coordinate<double>& solar_coordinate, Coordinate<double>& lunar_coordinate, double solar_factor2,
	double lunar_factor2 
)
/*
solid.f [LN 631–637]
```
|      subroutine st1isem(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
|
|*** this subroutine gives the out-of-phase corrections induced by
|*** mantle inelasticity in the diurnal band
|
|***  input: xsta,xsun,xmon,fac2sun,fac2mon
|*** output: xcorsta
xsta — geo_coordinate
xsun — solar_coordinate
xmon — lunar_coordinate
fac2sun — solar_factor2
fac2mon — lunar_factor2
xcorsta — [returned]
```
*/
{
	/*
	solid.f [LN 643–651]
	```
	|      rsta=enorm8(xsta)
	|      sinphi=xsta(3)/rsta
	|      cosphi=dsqrt(xsta(1)**2+xsta(2)**2)/rsta
	|      sinla=xsta(2)/cosphi/rsta
	|      cosla=xsta(1)/cosphi/rsta
	|      costwola=cosla**2-sinla**2
	|      sintwola=2.d0*cosla*sinla
	|      rmon=enorm8(xmon)
	|      rsun=enorm8(xsun)
	```
	rsta — geo_distance
	sinphi — sin_ϕ
	cosphi — cos_ϕ
	sinla — sin_latitude
	cosla — cos_latitude
	costwola — cos_squared_latitude
	sintwola — sin_squared_latitude
	rmon — lunar_distance
	rsun — solar_distance
	*/
	double geo_distance = geo_coordinate.distance();
	double sin_ϕ = geo_coordinate[Z] / geo_distance;
	double cos_ϕ = sqrt(pow(geo_coordinate[X], 2) + pow(geo_coordinate[Y], 2)) / geo_distance;
	double sin_latitude = geo_coordinate[Y] / cos_ϕ / geo_distance;
	double cos_latitude = geo_coordinate[X] / cos_ϕ / geo_distance;
	double cos_squared_latitude = pow(cos_ϕ, 2) - pow(sin_ϕ, 2);
	double sin_squared_latitude = 2.0 * cos_latitude * sin_latitude;
	double lunar_distance = lunar_coordinate.distance();
	double solar_distance = solar_coordinate.distance();

	/*
	solid.f [LN 652–670]
	```
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
	```
	*/
	double solar_dr = -3.0 / 4.0 * -0.0022 * pow(cos_ϕ, 2) * solar_factor2
		* ((pow(solar_coordinate[X], 2) - pow(solar_coordinate[Y], 2))
			* sin_squared_latitude-2.0 * solar_coordinate[X] * solar_coordinate[Y] * cos_squared_latitude)
			/ pow(solar_distance, 2);
	double lunar_dr = -3.0 / 4.0 * -0.0022 * pow(cos_ϕ, 2) * lunar_factor2
		* ((pow(lunar_coordinate[X], 2) - pow(lunar_coordinate[Y], 2))
			* sin_squared_latitude-2.0 * lunar_coordinate[X] * lunar_coordinate[Y] * cos_squared_latitude)
			/ pow(lunar_distance, 2);
	double solar_dn = 1.5 * -0.0007 * sin_ϕ * cos_ϕ * solar_factor2
		* ((pow(solar_coordinate[X], 2) - pow(solar_coordinate[Y], 2))
			* sin_squared_latitude-2.0 * solar_coordinate[X] * solar_coordinate[Y] * cos_squared_latitude)
			/ pow(solar_distance, 2);
	double lunar_dn = 1.5 * -0.0007 * sin_ϕ * cos_ϕ * lunar_factor2
		* ((pow(lunar_coordinate[X], 2) - pow(lunar_coordinate[Y], 2))
			* sin_squared_latitude-2.0 * lunar_coordinate[X] * lunar_coordinate[Y] * cos_squared_latitude)
			/ pow(lunar_distance, 2);
	double solar_de = -3.0 / 2.0 * -0.0007 * cos_ϕ * solar_factor2
		* ((pow(solar_coordinate[X], 2) - pow(solar_coordinate[Y], 2))
			* cos_squared_latitude+2.0 * solar_coordinate[X] * solar_coordinate[Y] * sin_squared_latitude)
			/ pow(solar_distance, 2);
	double lunar_de = -3.0 / 2.0 * -0.0007 * cos_ϕ * lunar_factor2
		* ((pow(lunar_coordinate[X], 2) - pow(lunar_coordinate[Y], 2))
			* cos_squared_latitude+2.0 * lunar_coordinate[X] * lunar_coordinate[Y] * sin_squared_latitude)
			/ pow(lunar_distance, 2);

	double dr = solar_dr + lunar_dr;
	double dn = solar_dn + lunar_dn;
	double de = solar_de + lunar_de;
	return Coordinate<double>(
		dr * cos_latitude * cos_ϕ - de * sin_latitude - dn * sin_ϕ * cos_latitude,
		dr * sin_latitude * cos_ϕ + de * cos_latitude - dn * sin_ϕ * sin_latitude,
		dr * sin_ϕ + dn * cos_ϕ
	);
}


Coordinate<double> Geolocation::latitude_dependence_correction(Coordinate<double>& geo_coordinate, 
	Coordinate<double>& solar_coordinate, Coordinate<double>& lunar_coordinate, double solar_factor2,
	double lunar_factor2 
)
/*
solid.f [308–314]
```
|      subroutine st1l1(xsta,xsun,xmon,fac2sun,fac2mon,xcorsta)
|
|*** this subroutine gives the corrections induced by the latitude dependence
|*** given by l^(1) in mahtews et al (1991)
|
|***  input: xsta,xsun,xmon,fac3sun,fac3mon
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
	solid.f [LN 321–327]
	```
	|      rsta=enorm8(xsta)
	|      sinphi=xsta(3)/rsta
	|      cosphi=dsqrt(xsta(1)**2+xsta(2)**2)/rsta
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
	solid.f [LN 329–344]
	```
	|*** for the diurnal band
	|
	|      l1=l1d
	|      dnsun=-l1*sinphi**2*fac2sun*xsun(3)*(xsun(1)*cosla+xsun(2)*sinla)
	|     *            /rsun**2
	|      dnmon=-l1*sinphi**2*fac2mon*xmon(3)*(xmon(1)*cosla+xmon(2)*sinla)
	|     *            /rmon**2
	|      desun=l1*sinphi*(cosphi**2-sinphi**2)*fac2sun*xsun(3)*
	|     * (xsun(1)*sinla-xsun(2)*cosla)/rsun**2
	|      demon=l1*sinphi*(cosphi**2-sinphi**2)*fac2mon*xmon(3)*
	|     * (xmon(1)*sinla-xmon(2)*cosla)/rmon**2
	|      de=3.d0*(desun+demon)
	|      dn=3.d0*(dnsun+dnmon)
	|      xcorsta(1)=-de*sinla-dn*sinphi*cosla
	|      xcorsta(2)= de*cosla-dn*sinphi*sinla
	|      xcorsta(3)=          dn*cosphi
	```
	*/
	double solar_diurnal_dn = -0.0012 * pow(sin_ϕ, 2) * solar_factor2 * solar_coordinate[Z]
		* (solar_coordinate[X] * cos_latitude + solar_coordinate[Y] * sin_latitude) / pow(solar_distance, 2);
	double lunar_diurnal_dn = -0.0012 * pow(sin_ϕ, 2) * lunar_factor2 * lunar_coordinate[Z]
		* (lunar_coordinate[X] * cos_latitude + lunar_coordinate[Y] * sin_latitude) / pow(lunar_distance, 2);
	double solar_diurnal_de = 0.0012 * sin_ϕ * (pow(cos_ϕ, 2) - pow(sin_ϕ, 2)) * solar_factor2 * solar_coordinate[Z]
		* (solar_coordinate[X] * sin_latitude - solar_coordinate[Y] * cos_latitude) / pow(solar_distance, 2);
	double lunar_diurnal_de = 0.0012 * sin_ϕ * (pow(cos_ϕ, 2) - pow(sin_ϕ, 2)) * lunar_factor2 * lunar_coordinate[Z]
		* (lunar_coordinate[X] * sin_latitude - lunar_coordinate[Y] * cos_latitude) / pow(lunar_distance, 2);
	double diurnal_de = 3.0 * (solar_diurnal_de +  lunar_diurnal_de);
	double diurnal_dn = 3.0 * (solar_diurnal_dn +  lunar_diurnal_dn);

	Coordinate<double> diurnal_band_correction(-diurnal_de * sin_latitude - diurnal_dn * sin_ϕ * cos_latitude,
		diurnal_de * cos_latitude - diurnal_dn * sin_ϕ * sin_latitude,
		diurnal_dn * cos_ϕ
	);

	/*
	solid.f [LN 346–363]
	```
	|*** for the semi-diurnal band
	|
	|      l1=l1sd
	|      costwola=cosla**2-sinla**2
	|      sintwola=2.d0*cosla*sinla
	|      dnsun=-l1/2.d0*sinphi*cosphi*fac2sun*((xsun(1)**2-xsun(2)**2)*
	|     * costwola+2.d0*xsun(1)*xsun(2)*sintwola)/rsun**2
	|      dnmon=-l1/2.d0*sinphi*cosphi*fac2mon*((xmon(1)**2-xmon(2)**2)*
	|     * costwola+2.d0*xmon(1)*xmon(2)*sintwola)/rmon**2
	|      desun=-l1/2.d0*sinphi**2*cosphi*fac2sun*((xsun(1)**2-xsun(2)**2)*
	|     * sintwola-2.d0*xsun(1)*xsun(2)*costwola)/rsun**2
	|      demon=-l1/2.d0*sinphi**2*cosphi*fac2mon*((xmon(1)**2-xmon(2)**2)*
	|     * sintwola-2.d0*xmon(1)*xmon(2)*costwola)/rmon**2
	|      de=3.d0*(desun+demon)
	|      dn=3.d0*(dnsun+dnmon)
	|      xcorsta(1)=xcorsta(1)-de*sinla-dn*sinphi*cosla
	|      xcorsta(2)=xcorsta(2)+de*cosla-dn*sinphi*sinla
	|      xcorsta(3)=xcorsta(3)         +dn*cosphi
	```
	*/
	double cos_squared_latitude = pow(cos_latitude, 2) - pow(sin_latitude, 2);
	double sin_squared_latitude = 2.0 * cos_latitude * sin_latitude;
	double solar_semi_dn = -0.0024 / 2.0 * sin_ϕ * cos_ϕ * solar_factor2
		* ((pow(solar_coordinate[X], 2) - pow(solar_coordinate[Z], 2))
			* cos_squared_latitude + 2.0 * solar_coordinate[X] * solar_coordinate[Y] * sin_squared_latitude)
		/ pow(solar_distance, 2);
	double lunar_semi_dn = -0.0024 / 2.0 * sin_ϕ * cos_ϕ * lunar_factor2
		* ((pow(lunar_coordinate[X], 2) - pow(lunar_coordinate[Y], 2))
			* cos_squared_latitude + 2.0 * lunar_coordinate[X] * lunar_coordinate[Y] * sin_squared_latitude)
		/ pow(lunar_distance, 2);
	double solar_semi_de = -0.0024 / 2.0 * pow(sin_ϕ, 2) * cos_ϕ * solar_factor2
		* ((pow(solar_coordinate[X], 2) - pow(solar_coordinate[Y], 2))
			* sin_squared_latitude - 2.0 * solar_coordinate[X] * solar_coordinate[Y] * cos_squared_latitude)
		/ pow(solar_distance, 2);
	double lunar_semi_de = -0.0024 / 2.0 * pow(sin_ϕ, 2) * cos_ϕ * lunar_factor2
		* ((pow(lunar_coordinate[X], 2) - pow(lunar_coordinate[Y], 2))
			* sin_squared_latitude - 2.0 * lunar_coordinate[X] * lunar_coordinate[Y] * cos_squared_latitude)
		/ pow(lunar_distance, 2);
	double semi_de = 3.0 * (solar_semi_de + lunar_semi_de);
	double semi_dn = 3.0 * (solar_semi_dn + lunar_semi_dn);
	return Coordinate<double>(
		diurnal_band_correction[X] - semi_de * sin_latitude - semi_dn * sin_ϕ * cos_latitude,
		diurnal_band_correction[Y] + semi_de * cos_latitude - semi_dn * sin_ϕ * sin_latitude,
		diurnal_band_correction[Z] + semi_dn * cos_ϕ
	);
}


Coordinate<double> Geolocation::second_step_diurnal_band_correction(Coordinate<double>& geo_coordinate,
	double terrestrial_time_hours, double terrestrial_time_years
)
/*
solid.f [LN 368–373...380–386]
```
|    subroutine step2diu(xsta,fhr,t,xcorsta)
|
|*** last change:  vd   17 may 00   1:20 pm
|*** these are the subroutines for the step2 of the tidal corrections.
|*** they are called to account for the frequency dependence
|*** of the love numbers.
⋮
|*** note, following table is derived from dehanttideinelMJD.f (2000oct30 16:10)
|*** has minor differences from that of dehanttideinel.f (2000apr17 14:10)
|*** D.M. edited to strictly follow published table 7.5a (2006aug08 13:46)
|
|*** cf. table 7.5a of IERS conventions 2003 (TN.32, pg.82)
|*** columns are s,h,p,N',ps, dR(ip),dR(op),dT(ip),dT(op)
|*** units of mm
```
xsta — geo_coordinate
fhr — terrestrial_time_hours
t — terrestrial_time_years
xcorsta — [returned]
*/
{
	/*
	solid.f [LN 388–389...447]
	```
	|      data ((datdi(i,j),i=1,9),j=1,31)/
	|     * -3., 0., 2., 0., 0.,-0.01,-0.01, 0.0 , 0.0,
	⋮
	|     *  3., 0., 0., 1., 0., 0.0 , 0.01, 0.0 , 0.0/
	```
	datdi — IERS_conversion
	Notably, Fortran uses column-major order, which is the oppose of C++. Column-major order maintains continuity of the
	 left-most index.
	*/

	double IERS_conversion[31][9] = {
		{-3.0,  0.0,  2.0,  0.0,  0.0, -0.01, -0.01,   0.0,   0.0},
		{-3.0,  2.0,  0.0,  0.0,  0.0, -0.01, -0.01,   0.0,   0.0},
		{-2.0,  0.0,  1.0, -1.0,  0.0, -0.02, -0.01,   0.0,   0.0},
		{-2.0,  0.0,  1.0,  0.0,  0.0, -0.08,   0.0,  0.01,  0.01},
		{-2.0,  2.0, -1.0,  0.0,  0.0, -0.02, -0.01,   0.0,   0.0},
		{-1.0,  0.0,  0.0, -1.0,  0.0, -0.10,   0.0,   0.0,   0.0},
		{-1.0,  0.0,  0.0,  0.0,  0.0, -0.51,   0.0, -0.02,  0.03},
		{-1.0,  2.0,  0.0,  0.0,  0.0,  0.01,   0.0,   0.0,   0.0},
		{ 0.0, -2.0,  1.0,  0.0,  0.0,  0.01,   0.0,   0.0,   0.0},
		{ 0.0,  0.0, -1.0,  0.0,  0.0,  0.02,  0.01,   0.0,   0.0},
		{ 0.0,  0.0,  1.0,  0.0,  0.0,  0.06,   0.0,   0.0,   0.0},
		{ 0.0,  0.0,  1.0,  1.0,  0.0,  0.01,   0.0,   0.0,   0.0},
		{ 0.0,  2.0, -1.0,  0.0,  0.0,  0.01,   0.0,   0.0,   0.0},
		{ 1.0, -3.0,  0.0,  0.0,  1.0, -0.06,   0.0,   0.0,   0.0},
		{ 1.0, -2.0,  0.0,  1.0,  0.0,  0.01,   0.0,   0.0,   0.0},
		{ 1.0, -2.0,  0.0,  0.0,  0.0, -1.23, -0.07,  0.06,  0.01},
		{ 1.0, -1.0,  0.0,  0.0, -1.0,  0.02,   0.0,   0.0,   0.0},
		{ 1.0, -1.0,  0.0,  0.0,  1.0,  0.04,   0.0,   0.0,   0.0},
		{ 1.0,  0.0,  0.0, -1.0,  0.0, -0.22,  0.01,  0.01,   0.0},
		{ 1.0,  0.0,  0.0,  0.0,  0.0, 12.00, -0.78, -0.67, -0.03},
		{ 1.0,  0.0,  0.0,  1.0,  0.0,  1.73, -0.12, -0.10,   0.0},
		{ 1.0,  0.0,  0.0,  2.0,  0.0, -0.04,   0.0,   0.0,   0.0},
		{ 1.0,  1.0,  0.0,  0.0, -1.0, -0.50, -0.01,  0.03,   0.0},
		{ 1.0,  1.0,  0.0,  0.0,  1.0,  0.01,   0.0,   0.0,   0.0},
		{ 1.0,  1.0,  0.0,  1.0, -1.0, -0.01,   0.0,   0.0,   0.0},
		{ 1.0,  2.0, -2.0,  0.0,  0.0, -0.01,   0.0,   0.0,   0.0},
		{ 1.0,  2.0,  0.0,  0.0,  0.0, -0.11,  0.01,  0.01,   0.0},
		{ 2.0, -2.0,  1.0,  0.0,  0.0, -0.01,   0.0,   0.0,   0.0},
		{ 2.0,  0.0, -1.0,  0.0,  0.0, -0.02,  0.02,   0.0,  0.01},
		{ 3.0,  0.0,  0.0,  0.0,  0.0,   0.0,  0.01,   0.0,  0.01},
		{ 3.0,  0.0,  0.0,  1.0,  0.0,   0.0,  0.01,   0.0,   0.0}
	};

	/*
	solid.f [LN 449–463]
	```
	|      s=218.31664563d0+481267.88194d0*t-0.0014663889d0*t*t
	|     * +0.00000185139d0*t**3
	|      tau=fhr*15.d0+280.4606184d0+36000.7700536d0*t+0.00038793d0*t*t
	|     * -0.0000000258d0*t**3-s
	|      pr=1.396971278*t+0.000308889*t*t+0.000000021*t**3
	|     * +0.000000007*t**4
	|      s=s+pr
	|      h=280.46645d0+36000.7697489d0*t+0.00030322222d0*t*t
	|     * +0.000000020*t**3-0.00000000654*t**4
	|      p=83.35324312d0+4069.01363525d0*t-0.01032172222d0*t*t
	|     * -0.0000124991d0*t**3+0.00000005263d0*t**4
	|      zns=234.95544499d0 +1934.13626197d0*t-0.00207561111d0*t*t
	|     * -0.00000213944d0*t**3+0.00000001650d0*t**4
	|      ps=282.93734098d0+1.71945766667d0*t+0.00045688889d0*t*t
	|     * -0.00000001778d0*t**3-0.00000000334d0*t**4
	```
	*/
	double s_less_pr = 218.31664563 + 481267.88194 * terrestrial_time_years - 0.0014663889 * terrestrial_time_years
		* terrestrial_time_years + 0.00000185139 * pow(terrestrial_time_years, 3);
	double tau_part = terrestrial_time_hours * 15.0 + 280.4606184 + 36000.7700536 * terrestrial_time_years
		+ 0.00038793 * terrestrial_time_years * terrestrial_time_years - 0.0000000258
		* pow(terrestrial_time_years, 3) - s_less_pr;
	double pr = 1.396971278 * terrestrial_time_years + 0.000308889 * terrestrial_time_years
		* terrestrial_time_years + 0.000000021 * pow(terrestrial_time_years, 3) + 0.000000007
		* pow(terrestrial_time_years, 4);
	double h_part = 280.46645 + 36000.7697489 * terrestrial_time_years + 0.00030322222 * terrestrial_time_years
		* terrestrial_time_years + 0.000000020 * pow(terrestrial_time_years, 3) - 0.00000000654
		* pow(terrestrial_time_years, 4);
	double p_part = 83.35324312 + 4069.01363525 * terrestrial_time_years - 0.01032172222 * terrestrial_time_years
		* terrestrial_time_years - 0.0000124991 * pow(terrestrial_time_years, 3) + 0.00000005263
		* pow(terrestrial_time_years, 4);
	double zns_part = 234.95544499  + 1934.13626197 * terrestrial_time_years - 0.00207561111
		* terrestrial_time_years * terrestrial_time_years - 0.00000213944 * pow(terrestrial_time_years, 3)
		+ 0.00000001650 * pow(terrestrial_time_years, 4);
	double ps_part = 282.93734098 + 1.71945766667 * terrestrial_time_years + 0.00045688889 * terrestrial_time_years
		* terrestrial_time_years - 0.00000001778 * pow(terrestrial_time_years, 3) - 0.00000000334
		* pow(terrestrial_time_years, 4);

	/*
	solid.f [LN 465–472]
	```
	|*** reduce angles to between 0 and 360
	|
	|      s=  dmod(  s,360.d0)
	|      tau=dmod(tau,360.d0)
	|      h=  dmod(  h,360.d0)
	|      p=  dmod(  p,360.d0)
	|      zns=dmod(zns,360.d0)
	|      ps= dmod( ps,360.d0)
	```
	*/
	double s = std::fmod(s_less_pr + pr, 360.0);
	double tau = std::fmod(tau_part, 360.0);
	double h = std::fmod(h_part, 360.0);
	double p = std::fmod(p_part, 360.0);
	double zns = std::fmod(zns_part, 360.0);
	double ps = std::fmod(ps_part, 360.0);

	/*
	solid.f [LN 474–480]
	```
	|      rsta=dsqrt(xsta(1)**2+xsta(2)**2+xsta(3)**2)
	|      sinphi=xsta(3)/rsta
	|      cosphi=dsqrt(xsta(1)**2+xsta(2)**2)/rsta
	|
	|      cosla=xsta(1)/cosphi/rsta
	|      sinla=xsta(2)/cosphi/rsta
	|      zla = datan2(xsta(2),xsta(1))
	```
	rsta — geo_distance
	sinphi — sin_ϕ
	cosphi — cos_ϕ
	cosla — cos_latitude
	sinla — sin_latitude
	zla — Z_latitude
	*/
	double geo_distance = geo_coordinate.distance();
	double sin_ϕ = geo_coordinate[Z] / geo_distance;
	double cos_ϕ = sqrt(pow(geo_coordinate[X], 2) + pow(geo_coordinate[Y], 2)) / geo_distance;

	double cos_latitude = geo_coordinate[X] / cos_ϕ / geo_distance;
	double sin_latitude = geo_coordinate[Y] / cos_ϕ / geo_distance;
	double Z_latitude = atan2(geo_coordinate[Y], geo_coordinate[X]);

	/*
	solid.f [LN 481–483]
	```
	|      do i=1,3
	|        xcorsta(i)=0.d0
	|      enddo
	```
	*/
	Coordinate<double> correction;

	/*
	solid.f [LN 484–504]
	```
	|      do j=1,31
	|        thetaf=(tau+datdi(1,j)*s+datdi(2,j)*h+datdi(3,j)*p+
	|     *   datdi(4,j)*zns+datdi(5,j)*ps)*deg2rad
	|        dr=datdi(6,j)*2.d0*sinphi*cosphi*sin(thetaf+zla)+
	|     *     datdi(7,j)*2.d0*sinphi*cosphi*cos(thetaf+zla)
	|        dn=datdi(8,j)*(cosphi**2-sinphi**2)*sin(thetaf+zla)+
	|     *     datdi(9,j)*(cosphi**2-sinphi**2)*cos(thetaf+zla)
	|***** following correction by V.Dehant to match eq.16b, p.81, 2003 Conventions
	|*****   de=datdi(8,j)*sinphi*cos(thetaf+zla)+
	|        de=datdi(8,j)*sinphi*cos(thetaf+zla)-
	|     *     datdi(9,j)*sinphi*sin(thetaf+zla)
	|        xcorsta(1)=xcorsta(1)+dr*cosla*cosphi-de*sinla
	|     *   -dn*sinphi*cosla
	|        xcorsta(2)=xcorsta(2)+dr*sinla*cosphi+de*cosla
	|     *   -dn*sinphi*sinla
	|        xcorsta(3)=xcorsta(3)+dr*sinphi+dn*cosphi
	|      enddo
	|
	|      do i=1,3
	|         xcorsta(i)=xcorsta(i)/1000.d0
	|      enddo
	```
	*/
	for(unsigned int row = 0; row < 31; row++)
	{
		double thetaf = (tau + IERS_conversion[row][0] * s + IERS_conversion[row][1] * h
			+ IERS_conversion[row][2] * p + IERS_conversion[row][3] * zns + IERS_conversion[row][4] * ps)
			* RADIANS_PER_DEGREE;
		double dr = IERS_conversion[row][5] * 2.0 * sin_ϕ * cos_ϕ * sin(thetaf + Z_latitude) + IERS_conversion[row][6]
			* 2.0 * sin_ϕ * cos_ϕ * cos(thetaf + Z_latitude);
		double dn = IERS_conversion[row][7] * (pow(cos_ϕ, 2) - pow(sin_ϕ, 2)) * sin(thetaf + Z_latitude) + 
			IERS_conversion[row][8] * (pow(cos_ϕ, 2) - pow(sin_ϕ, 2)) * cos(thetaf + Z_latitude);
		double de = IERS_conversion[row][7] * sin_ϕ * cos(thetaf + Z_latitude)
			- IERS_conversion[row][8] * sin_ϕ * sin(thetaf + Z_latitude);
		correction[X] += dr * cos_latitude * cos_ϕ - de * sin_latitude - dn * sin_ϕ * cos_latitude;
		correction[Y] += dr * sin_latitude * cos_ϕ + de * cos_latitude - dn * sin_ϕ * sin_latitude;
		correction[Z] += dr * sin_ϕ + dn * cos_ϕ;
	}

	return correction / 1000.0;
}


Coordinate<double> Geolocation::second_step_longitudinal_correction(Coordinate<double>& geo_coordinate,
	double terrestrial_time_hours, double terrestrial_time_years
)
/*
solid.f [LN 509]
```
|      subroutine step2lon(xsta,fhr,t,xcorsta)
```
xsta — geo_coordinate
fhr — terrestrial_time_hours
t — terrestrial_time_years
xcorsta — [returned]
*/
{
	/*
	solid.f [LN 516–526]
	```
	|*** cf. table 7.5b of IERS conventions 2003 (TN.32, pg.82)
	|*** columns are s,h,p,N',ps, dR(ip),dT(ip),dR(op),dT(op)
	|*** IERS cols.= s,h,p,N',ps, dR(ip),dR(op),dT(ip),dT(op)
	|*** units of mm
	|
	|      data ((datdi(i,j),i=1,9),j=1,5)/
	|     *   0, 0, 0, 1, 0,   0.47, 0.23, 0.16, 0.07,
	|     *   0, 2, 0, 0, 0,  -0.20,-0.12,-0.11,-0.05,
	|     *   1, 0,-1, 0, 0,  -0.11,-0.08,-0.09,-0.04,
	|     *   2, 0, 0, 0, 0,  -0.13,-0.11,-0.15,-0.07,
	|     *   2, 0, 0, 1, 0,  -0.05,-0.05,-0.06,-0.03/
	*/
	double IERS_conversion[5][9] = {
		{0.0, 0.0,  0.0, 1.0, 0.0,  0.47,  0.23,  0.16,  0.07},
		{0.0, 2.0,  0.0, 0.0, 0.0, -0.20, -0.12, -0.11, -0.05},
		{1.0, 0.0, -1.0, 0.0, 0.0, -0.11, -0.08, -0.09, -0.04},
		{2.0, 0.0,  0.0, 0.0, 0.0, -0.13, -0.11, -0.15, -0.07},
		{2.0, 0.0,  0.0, 1.0, 0.0, -0.05, -0.05, -0.06, -0.03}
	};

	/*
	solid.f [LN 528–540]
	```
	|      s=218.31664563d0+481267.88194d0*t-0.0014663889d0*t*t
	|     * +0.00000185139d0*t**3
	|      pr=1.396971278*t+0.000308889*t*t+0.000000021*t**3
	|     * +0.000000007*t**4
	|      s=s+pr
	|      h=280.46645d0+36000.7697489d0*t+0.00030322222d0*t*t
	|     * +0.000000020*t**3-0.00000000654*t**4
	|      p=83.35324312d0+4069.01363525d0*t-0.01032172222d0*t*t
	|     * -0.0000124991d0*t**3+0.00000005263d0*t**4
	|      zns=234.95544499d0 +1934.13626197d0*t-0.00207561111d0*t*t
	|     * -0.00000213944d0*t**3+0.00000001650d0*t**4
	|      ps=282.93734098d0+1.71945766667d0*t+0.00045688889d0*t*t
	|     * -0.00000001778d0*t**3-0.00000000334d0*t**4
	```
	*/
	double s_less_pr = 218.31664563 + 481267.88194 * terrestrial_time_years - 0.0014663889 * terrestrial_time_years
		* terrestrial_time_years + 0.00000185139 * pow(terrestrial_time_years, 3);
	double pr = 1.396971278 * terrestrial_time_years + 0.000308889 * terrestrial_time_years
		* terrestrial_time_years + 0.000000021 * pow(terrestrial_time_years, 3) + 0.000000007
		* pow(terrestrial_time_years, 4);
	double h_part = 280.46645 + 36000.7697489 * terrestrial_time_years + 0.00030322222 * terrestrial_time_years
		* terrestrial_time_years + 0.000000020 * pow(terrestrial_time_years, 3) - 0.00000000654
		* pow(terrestrial_time_years, 4);
	double p_part = 83.35324312 + 4069.01363525 * terrestrial_time_years - 0.01032172222 * terrestrial_time_years
		* terrestrial_time_years - 0.0000124991 * pow(terrestrial_time_years, 3) + 0.00000005263
		* pow(terrestrial_time_years, 4);
	double zns_part = 234.95544499  + 1934.13626197 * terrestrial_time_years - 0.00207561111
		* terrestrial_time_years * terrestrial_time_years - 0.00000213944 * pow(terrestrial_time_years, 3)
		+ 0.00000001650 * pow(terrestrial_time_years, 4);
	double ps_part = 282.93734098 + 1.71945766667 * terrestrial_time_years + 0.00045688889 * terrestrial_time_years
		* terrestrial_time_years - 0.00000001778 * pow(terrestrial_time_years, 3) - 0.00000000334
		* pow(terrestrial_time_years, 4);

	/*
	solid.f [LN 541–545]
	```
	|      rsta=dsqrt(xsta(1)**2+xsta(2)**2+xsta(3)**2)
	|      sinphi=xsta(3)/rsta
	|      cosphi=dsqrt(xsta(1)**2+xsta(2)**2)/rsta
	|      cosla=xsta(1)/cosphi/rsta
	|      sinla=xsta(2)/cosphi/rsta
	```
	rsta — geo_distance
	sinphi — sin_ϕ
	cosphi — cos_ϕ
	cosla — cos_latitude
	sinla — sin_latitude
	*/
	double geo_distance = geo_coordinate.distance();
	double sin_ϕ = geo_coordinate[Z] / geo_distance;
	double cos_ϕ = sqrt(pow(geo_coordinate[X], 2) + pow(geo_coordinate[Y], 2)) / geo_distance;
	double cos_latitude = geo_coordinate[X] / cos_ϕ / geo_distance;
	double sin_latitude = geo_coordinate[Y] / cos_ϕ / geo_distance;

	/*
	solid.f [LN 547–554]
	```
	|*** reduce angles to between 0 and 360
	|
	|      s=  dmod(  s,360.d0)
	|***** tau=dmod(tau,360.d0)       !*** tau not used here--09jul28
	|      h=  dmod(  h,360.d0)
	|      p=  dmod(  p,360.d0)
	|      zns=dmod(zns,360.d0)
	|      ps= dmod( ps,360.d0)
	```
	*/
	double s = std::fmod(s_less_pr + pr, 360.0);
	double h = std::fmod(h_part, 360.0);
	double p = std::fmod(p_part, 360.0);
	double zns = std::fmod(zns_part, 360.0);
	double ps = std::fmod(ps_part, 360.0);

	/*
	solid.f [LN 556...560]
	```
	|      dr_tot=0.d0
	⋮
	|      enddo
	```
	*/
	double dr_tot = 0, dn_tot = 0;
	Coordinate<double> partial_correction;

	/*
	solid.f [LN 562–584]
	```
	|***             1 2 3 4   5   6      7      8      9
	|*** columns are s,h,p,N',ps, dR(ip),dT(ip),dR(op),dT(op)
	|
	|      do j=1,5
	|        thetaf=(datdi(1,j)*s+datdi(2,j)*h+datdi(3,j)*p+
	|     *   datdi(4,j)*zns+datdi(5,j)*ps)*deg2rad
	|        dr=datdi(6,j)*(3.d0*sinphi**2-1.d0)/2.*cos(thetaf)+
	|     *       datdi(8,j)*(3.d0*sinphi**2-1.d0)/2.*sin(thetaf)
	|        dn=datdi(7,j)*(cosphi*sinphi*2.d0)*cos(thetaf)+
	|     *       datdi(9,j)*(cosphi*sinphi*2.d0)*sin(thetaf)
	|        de=0.d0
	|        dr_tot=dr_tot+dr
	|        dn_tot=dn_tot+dn
	|        xcorsta(1)=xcorsta(1)+dr*cosla*cosphi-de*sinla
	|     *   -dn*sinphi*cosla
	|        xcorsta(2)=xcorsta(2)+dr*sinla*cosphi+de*cosla
	|     *   -dn*sinphi*sinla
	|        xcorsta(3)=xcorsta(3)+dr*sinphi+dn*cosphi
	|      enddo
	|
	|      do i=1,3
	|        xcorsta(i)=xcorsta(i)/1000.d0
	|      enddo
	```
	*/
	for(unsigned int x = 0; x < 5; x++)
	{
		double thetaf = (IERS_conversion[x][0] * s + IERS_conversion[x][1] * h + IERS_conversion[x][2] * p + 
			IERS_conversion[x][3] * zns + IERS_conversion[x][4] * ps) * RADIANS_PER_DEGREE;
		double dr = IERS_conversion[x][5] * (3.0 * pow(sin_ϕ, 2) - 1.0) / 2.0 * cos(thetaf) + 
			IERS_conversion[x][7] * (3.0 * pow(sin_ϕ, 2) - 1.0) / 2.0 * sin(thetaf);
		double dn = IERS_conversion[x][6] * (cos_ϕ * sin_ϕ * 2.0) * cos(thetaf) + IERS_conversion[x][8]
			* (cos_ϕ * sin_ϕ * 2.0) * sin(thetaf);
		dr_tot = dr_tot + dr;
		dn_tot = dn_tot + dn;

		partial_correction[X] += dr * cos_latitude * cos_ϕ - 0.0 * sin_latitude - dn * sin_ϕ * cos_latitude;
		partial_correction[Y] += dr * sin_latitude * cos_ϕ + 0.0 * cos_latitude - dn * sin_ϕ * sin_latitude;
		partial_correction[Z] += dr * sin_ϕ + dn * cos_ϕ;
	}

	return partial_correction / 1000;
}
