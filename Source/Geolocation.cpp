

#include "Geolocation.hpp"


#include <cmath>


#include "Coordinate.hpp"
#include "JulianDate.hpp"


// FROM: https://stackoverflow.com/a/57285400
const long double Geolocation::PI = 3.141592653589793238462643383279502884197169399375105820974944;
const long double Geolocation::RADIAN = 180.0 / Geolocation::PI;

/*
solid.f [LN 899–904]
```
|*** mean elements for year 2000, sun ecliptic orbit wrt. Earth
|
|      obe =23.43929111d0/rad        !*** obliquity of the J2000 ecliptic
|      sobe=dsin(obe)
|      cobe=dcos(obe)
|      opod=282.9400d0               !*** RAAN + arg.peri.  (deg.)
```
obe — OBLIQUITY
sobe — SIN_OBLIQUITY
cobe — COS_OBLIQUITY
opod — OPOD
*/
const double Geolocation::OBLIQUITY = 23.43929111 / (180.0 / PI);  // Radians
const double Geolocation::COS_OBLIQUITY = cos(OBLIQUITY);
const double Geolocation::SIN_OBLIQUITY = sin(OBLIQUITY);
const double Geolocation::OPOD = 282.9400;

const double Geolocation::EQUITORIAL_RADIUS = 6378137.0;
const double Geolocation::GEODETIC_ELLIPSOID = 6.69438002290341574957e-3;


Geolocation::Geolocation(double latitude_degrees, double longitude_degrees)
	/*
	solid.f [LN 55–56]
	|      gla0=glad/rad
	|      glo0=glod/rad
	*/
: _latitude{latitude_degrees / static_cast<double>(RADIAN)},
  _longitude{longitude_degrees / static_cast<double>(RADIAN)}
{}


// void operator<<(double cartesian_coordinates[3], Geolocation& geolocation)
// /*
// solid.f [LN 57–61]
// ```
// |      eht0=0.d0
// |      call geoxyz(gla0,glo0,eht0,x0,y0,z0)
// |      xsta(1)=x0
// |      xsta(2)=y0
// |      xsta(3)=z0
// ```
// */
// {
// 	/*
// 	solid.f [LN 974–980]
// 	```
// 	|      common/comgrs/a,e2
// 	|
// 	|      sla=dsin(gla)
// 	|      cla=dcos(gla)
// 	|      w2=1.d0-e2*sla*sla
// 	|      w=dsqrt(w2)
// 	|      en=a/w
// 	```
// 	en — prime_vertical_radius
// 	*/
// 	double sin_latitude = 0;
// 	double cos_latitude = 0;
// 	double w_squared = 1.0 - Geolocation::GEODETIC_ELLIPSOID * sin_latitude * sin_latitude;
// 	double w = pow(w_squared, 0.5);
// 	double prime_vertical_radius = Geolocation::EQUITORIAL_RADIUS / w;

// 	/*
// 	solid.f [LN 57]
// 	```
// 	|      eht0=0.d0
// 	```
// 	eht0 <=> eht — Altitude
// 	*/
// 	const double altitude = 0.0;

// 	/*
// 	solid.f [LN 974...982–984]
// 	```
// 	|      common/comgrs/a,e2
// 	⋮
// 	|      x=(en+eht)*cla*dcos(glo)
// 	|      y=(en+eht)*cla*dsin(glo)
// 	|      z=(en*(1.d0-e2)+eht)*sla
// 	```
// 	e2 — GEODETIC_ELLIPSOID

// 	*/
// 	cartesian_coordinates[X] = (prime_vertical_radius+altitude) * cos_latitude * cos(geolocation._longitude * Geolocation::RADIAN);
// 	cartesian_coordinates[Y] = (prime_vertical_radius+altitude) * cos_latitude * sin(geolocation._longitude * Geolocation::RADIAN);
// 	cartesian_coordinates[Z] = (prime_vertical_radius*(1.0-Geolocation::GEODETIC_ELLIPSOID) + altitude) * sin_latitude;
// }


Geolocation::operator Coordinate<double>()
/*
solid.f [LN 57–61]
```
|      eht0=0.d0
|      call geoxyz(gla0,glo0,eht0,x0,y0,z0)
|      xsta(1)=x0
|      xsta(2)=y0
|      xsta(3)=z0
```
*/
{
	/*
	solid.f [LN 974–980]
	```
	|      common/comgrs/a,e2
	|
	|      sla=dsin(gla)
	|      cla=dcos(gla)
	|      w2=1.d0-e2*sla*sla
	|      w=dsqrt(w2)
	|      en=a/w
	```
	en — prime_vertical_radius
	*/
	double sin_latitude = 0;  // TODO
	double cos_latitude = 0;  // TODO
	double w_squared = 1.0 - Geolocation::GEODETIC_ELLIPSOID * sin_latitude * sin_latitude;
	double w = pow(w_squared, 0.5);
	double prime_vertical_radius = Geolocation::EQUITORIAL_RADIUS / w;

	/*
	solid.f [LN 57]
	```
	|      eht0=0.d0
	```
	eht0 <=> eht — Altitude
	*/
	const double altitude = 0.0;

	/*
	solid.f [LN 974...982–984]
	```
	|      common/comgrs/a,e2
	⋮
	|      x=(en+eht)*cla*dcos(glo)
	|      y=(en+eht)*cla*dsin(glo)
	|      z=(en*(1.d0-e2)+eht)*sla
	```
	e2 — GEODETIC_ELLIPSOID

	*/
	return Coordinate<double>(
	  /* X = */(prime_vertical_radius+altitude) * cos_latitude * cos(this -> _longitude * (double)Geolocation::RADIAN),
	  /* Y = */(prime_vertical_radius+altitude) * cos_latitude * sin(this -> _longitude * (double)Geolocation::RADIAN),
	  /* Z = */(prime_vertical_radius*(1.0-Geolocation::GEODETIC_ELLIPSOID) + altitude) * sin_latitude
	);
}


Coordinate<double> Geolocation::sun_coordinates(unsigned int initial_modified_julian_date, JulianDate& julian_date)
/*
solid.f [LN 880–897]
```
|      subroutine sunxyz(mjd,fmjd,rs,lflag)
|
|*** get low-precision, geocentric coordinates for sun (ECEF)
|
|*** input, mjd/fmjd, is Modified Julian Date (and fractional) in UTC time
|*** output, rs, is geocentric solar position vector [m] in ECEF
|***         lflag  -- leap second table limit flag,  false:flag not raised
|*** 1."satellite orbits: models, methods, applications" montenbruck & gill(2000)
|*** section 3.3.2, pg. 70-71
|*** 2."astronomy on the personal computer, 4th ed." montenbruck & pfleger (2005)
|*** section 3.2, pg. 39  routine MiniSun
|
|      implicit double precision(a-h,o-z)
|      dimension rs(3)
|      logical lflag,leapflag
|      save  /limitflag/                !*** leap second table limit flag
|      common/limitflag/leapflag        !*** leap second table limit flag
|      common/stuff/rad,pi,pi2
```
rs<->rsun — sun coordinates: double[3]
*/
{
	/*
	solid.f [LN 899–904] defined in Geolocation.hpp [LN –] as
	```
	|		static const double OBLIQUITY;  // 0.409092804202936: (23.43929111 / rad) obliquity of the J2000 ecliptic
	|		static const double COS_OBLIQUITY;  // 0.999974510191339: cos(obliquity of the J2000 ecliptic)
	|		static const double SIN_OBLIQUITY;  // 0.00713995571357085: sin(obliquity of the J2000 ecliptic)
	|		static const double OPOD;  // 282.9400: RAAN + arg.peri. (deg.)
	```
	*/

	/*
	solid.f [LN 906...918]
	```
	|*** use TT for solar ephemerides
	⋮
	|      t     = (tjdtt - 2451545.d0)/36525.d0       !*** julian centuries, TT
	```
	t — terrestrial_time
	*/
	double terrestrial_time = julian_date.to_TerrestrialTime(initial_modified_julian_date);

	/*
	solid.f [LN ]
	```
	|      emdeg = 357.5256d0 + 35999.049d0*t          !*** degrees
	|      em    = emdeg/rad                           !*** radians
	|      em2   = em+em                               !*** radians
	```
	emdeg — solar_ephemerides_degrees
	em — solar_ephemerides
	em2 — 
	*/
	double solar_ephemerides_degrees = 357.5256 + 35999.049 * terrestrial_time;
	double solar_ephemerides = solar_ephemerides_degrees / Geolocation::RADIAN;

	/*
	solid.f [LN ]
	```
	|*** series expansions in mean anomaly, em   (eq. 3.43, p.71)
	|
	|      r=(149.619d0-2.499d0*dcos(em)-0.021d0*dcos(em2))*1.d9      !*** m.
	|      slond=opod + emdeg + (6892.d0*dsin(em)+72.d0*dsin(em2))/3600.d0
	|
	|*** precession of equinox wrt. J2000   (p.71)
	|
	|      slond=slond + 1.3972d0*t                              !*** degrees
	```
	r — radius
	slond — solar_longitude_degrees
	*/
	double radius = (149.619 - 2.499 * cos(solar_ephemerides) - 0.021 * cos(solar_ephemerides * 2)) * 1000000000;
	double solar_longitude_degrees = (6892.0 * sin(solar_ephemerides) + 72.0 * sin(solar_ephemerides * 2)) / 3600.0 
	  + Geolocation::OPOD + solar_ephemerides_degrees + 1.3972 * terrestrial_time;

	/*
	solid.f [LN ]
	```
	|*** position vector of sun (mean equinox & ecliptic of J2000) (EME2000, ICRF)
	|***                        (plus long. advance due to precession -- eq. above)
	|
	|      slon =slond/rad                                       !*** radians
	|      sslon=dsin(slon)
	|      cslon=dcos(slon)
	|
	|      rs1 = r*cslon              !*** meters             !*** eq. 3.46, p.71
	|      rs2 = r*sslon*cobe         !*** meters             !*** eq. 3.46, p.71
	|      rs3 = r*sslon*sobe         !*** meters             !*** eq. 3.46, p.71
	```
	slon — solar_longitude
	sslon — sin_solar_longitude
	cslon — cos_solar_longitude
	rs1 — radius_solar_coordinates_X
	rs2 — radius_solar_coordinates_Y
	rs3 — radius_solar_coordinates_Z
	*/
	double solar_longitude = solar_longitude_degrees / Geolocation::RADIAN;
	double cos_solar_longitude = cos(solar_longitude);
	double sin_solar_longitude = sin(solar_longitude);

	Coordinate<double> radius_solar_coordinates(
		radius * cos_solar_longitude,
		radius * sin_solar_longitude * Geolocation::COS_OBLIQUITY,
		radius * sin_solar_longitude * Geolocation::SIN_OBLIQUITY
	);

	/*
	solid.f [LN ]
	```
	|*** convert position vector of sun to ECEF  (ignore polar motion/LOD)
	|
	|      call getghar(mjd,fmjd,ghar)                        !*** sec 2.3.1,p.33
	|      call rot3(ghar,rs1,rs2,rs3,rs(1),rs(2),rs(3))      !*** eq. 2.89, p.37
	```
	*/
	double GreenwichHourAngleRadians = julian_date.GreenwichHourAngleRadians();
	return radius_solar_coordinates.rotate3(GreenwichHourAngleRadians);
}


Coordinate<double> moon_coordinates(unsigned int initial_modified_julian_date, JulianDate& julian_date)
/*
```
|      subroutine moonxyz(mjd,fmjd,rm,lflag)
|
|*** get low-precision, geocentric coordinates for moon (ECEF)
|*** UTC version
|
|*** input:  mjd/fmjd, is Modified Julian Date (and fractional) in UTC time
|*** output: rm, is geocentric lunar position vector [m] in ECEF
|***         lflag  -- leap second table limit flag,  false:flag not raised
|*** 1."satellite orbits: models, methods, applications" montenbruck & gill(2000)
|*** section 3.3.2, pg. 72-73
|*** 2."astronomy on the personal computer, 4th ed." montenbruck & pfleger (2005)
|*** section 3.2, pg. 38-39  routine MiniMoon
```
*/
{
	/*
	```
	|      t     = (tjdtt - 2451545.d0)/36525.d0       !*** julian centuries, TT
	```
	*/
		double terrestrial_time = julian_date.to_TerrestrialTime(initial_modified_julian_date);

	/*
	```
	|*** el0 -- mean longitude of Moon (deg)
	|*** el  -- mean anomaly of Moon (deg)
	|*** elp -- mean anomaly of Sun  (deg)
	|*** f   -- mean angular distance of Moon from ascending node (deg)
	|*** d   -- difference between mean longitudes of Sun and Moon (deg)
	|
	|*** equations 3.47, p.72
	|
	|      el0=218.31617d0 + 481267.88088d0*t -1.3972*t
	|      el =134.96292d0 + 477198.86753d0*t
	|      elp=357.52543d0 +  35999.04944d0*t
	|      f  = 93.27283d0 + 483202.01873d0*t
	|      d  =297.85027d0 + 445267.11135d0*t
	```
	el0 — mean_lunar_longitude
	el — mean_lunar_anomaly
	elp — mean_solar_anomaly
	f — mean_lunar_angular_distance
	d — mean_lunar_and_solar_difference
	*/
	double mean_lunar_longitude = 218.31617 + 481267.88088 * terrestrial_time - 1.3972 * terrestrial_time;
	double mean_lunar_anomaly = 134.96292 + 477198.86753 * terrestrial_time;
	double mean_solar_anomaly = 357.52543 + 35999.04944 * terrestrial_time;
	double mean_lunar_angular_distance = 93.27283 + 483202.01873 * terrestrial_time;
	double mean_lunar_and_solar_difference = 297.85027 + 445267.11135 * terrestrial_time;

	double mean_lunar_and_solar_anomaly = mean_lunar_anomaly + mean_solar_anomaly;
	double mean_lunar_distance_minus_anomaly = mean_lunar_angular_distance - mean_lunar_anomaly;

	/*
	```
	|*** longitude w.r.t. equinox and ecliptic of year 2000
	|
	|      selond=el0                                      !*** eq 3.48, p.72
	|     * +22640.d0/3600.d0*dsin((el        )/rad)
	|     * +  769.d0/3600.d0*dsin((el+el     )/rad)
	|     * - 4586.d0/3600.d0*dsin((el-d-d    )/rad)
	|     * + 2370.d0/3600.d0*dsin((d+d       )/rad)
	|     * -  668.d0/3600.d0*dsin((elp       )/rad)
	|     * -  412.d0/3600.d0*dsin((f+f       )/rad)
	|     * -  212.d0/3600.d0*dsin((el+el-d-d )/rad)
	|     * -  206.d0/3600.d0*dsin((el+elp-d-d)/rad)
	|     * +  192.d0/3600.d0*dsin((el+d+d    )/rad)
	|     * -  165.d0/3600.d0*dsin((elp-d-d   )/rad)
	|     * +  148.d0/3600.d0*dsin((el-elp    )/rad)
	|     * -  125.d0/3600.d0*dsin((d         )/rad)
	|     * -  110.d0/3600.d0*dsin((el+elp    )/rad)
	|     * -   55.d0/3600.d0*dsin((f+f-d-d   )/rad)
	```
	selond — solar_ecliptic_longitude_degrees
	*/
	double factors[14] = {
		22640.0 / 3600.0,   769.0 / 3600.0,  -4586.0 / 3600.0,  2370.0 / 3600.0,
		 -668.0 / 3600.0,  -412.0 / 3600.0,  -212.0 / 3600.0,  -206.0 / 3600.0,
		  192.0 / 3600.0,  -165.0 / 3600.0,   148.0 / 3600.0,  -125.0 / 3600.0,
		 -110.0 / 3600.0,   -55.0 / 3600.0
	};

	double solar_ecliptic_longitude_degrees = mean_lunar_longitude
		+ factors[0]  * sin(mean_lunar_anomaly / Geolocation::RADIAN)
		+ factors[1]  * sin(mean_lunar_anomaly * 2 / Geolocation::RADIAN)
		+ factors[2]  * sin((mean_lunar_anomaly - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[3]  * sin(mean_lunar_and_solar_difference * 2 / Geolocation::RADIAN)
		+ factors[4]  * sin(mean_solar_anomaly / Geolocation::RADIAN)
		+ factors[5]  * sin(mean_lunar_angular_distance * 2 / Geolocation::RADIAN)
		+ factors[6]  * sin((mean_lunar_anomaly * 2 - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[7]  * sin((mean_lunar_and_solar_anomaly - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[8]  * sin((mean_lunar_anomaly + mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[9]  * sin((mean_solar_anomaly - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[10] * sin((mean_lunar_anomaly - mean_solar_anomaly) / Geolocation::RADIAN)
		+ factors[11] * sin(mean_lunar_and_solar_difference / Geolocation::RADIAN)
		+ factors[12] * sin((mean_lunar_and_solar_anomaly) / Geolocation::RADIAN)
		+ factors[13] * sin(((mean_lunar_angular_distance * 2) - (mean_lunar_and_solar_difference * 2)) / Geolocation::RADIAN);


	/*
	```
	|*** latitude w.r.t. equinox and ecliptic of year 2000
	|
	|      q = 412.d0/3600.d0*dsin((f+f)/rad)              !*** temporary term
	|     *   +541.d0/3600.d0*dsin((elp)/rad)
	|
	|      selatd=                                         !*** eq 3.49, p.72
	|     * +18520.d0/3600.d0*dsin((f+selond-el0+q)/rad)
	|     * -  526.d0/3600.d0*dsin((f-d-d     )/rad)
	|     * +   44.d0/3600.d0*dsin((el+f-d-d  )/rad)
	|     * -   31.d0/3600.d0*dsin((-el+f-d-d )/rad)
	|     * -   25.d0/3600.d0*dsin((-el-el+f  )/rad)
	|     * -   23.d0/3600.d0*dsin((elp+f-d-d )/rad)
	|     * +   21.d0/3600.d0*dsin((-el+f     )/rad)
	|     * +   11.d0/3600.d0*dsin((-elp+f-d-d)/rad)
	```
	q — temp
	selatd — solar_ecliptic_latitude_degrees
	*/

	double temp = 412.0 / 3600.0 * sin(mean_lunar_angular_distance * 2 / Geolocation::RADIAN)
		+ 541.0 / 3600.0 * sin(mean_solar_anomaly / rad);

	double factors[8] = {
		18520.0 / 3600.0,  526.0 / 3600.0,  44.0 / 3600.0,  -31.0 / 3600.0,
		  -25.0 / 3600.0,  -23.0 / 3600.0,  21.0 / 3600.0,   11.0 / 3600.0
	}
	double solar_ecliptic_latitude_degrees =
		factors[0]   * sin((mean_lunar_angular_distance + solar_ecliptic_longitude_degrees - mean_lunar_longitude + temp) / Geolocation::RADIAN)
		+ factors[1] * sin((mean_lunar_angular_distance - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[2] * sin((mean_lunar_anomaly + mean_lunar_angular_distance - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[3] * sin((mean_lunar_distance_minus_anomaly - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[4] * sin((-mean_lunar_anomaly + mean_lunar_distance_minus_anomaly) / Geolocation::RADIAN)
		+ factors[5] * sin((mean_solar_anomaly + mean_lunar_angular_distance - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[6] * sin((mean_lunar_distance_minus_anomaly) / Geolocation::RADIAN)
		+ factors[7] * sin((-mean_solar_anomaly + mean_lunar_angular_distance - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)

	/*

	```
	|*** distance from Earth center to Moon (m)
	|
	|      rse= 385000.d0*1000.d0                          !*** eq 3.50, p.72
	|     *   -  20905.d0*1000.d0*dcos((el        )/rad)
	|     *   -   3699.d0*1000.d0*dcos((d+d-el    )/rad)
	|     *   -   2956.d0*1000.d0*dcos((d+d       )/rad)
	|     *   -    570.d0*1000.d0*dcos((el+el     )/rad)
	|     *   +    246.d0*1000.d0*dcos((el+el-d-d )/rad)
	|     *   -    205.d0*1000.d0*dcos((elp-d-d   )/rad)
	|     *   -    171.d0*1000.d0*dcos((el+d+d    )/rad)
	|     *   -    152.d0*1000.d0*dcos((el+elp-d-d)/rad)
	```
	el0 — mean_lunar_longitude
	el — mean_lunar_anomaly
	elp — mean_solar_anomaly
	f — mean_lunar_angular_distance
	d — mean_lunar_and_solar_difference
	*/
	double factors[] = {
		385000000.0,  -20905000.0,  -3699000.0,  -2956000.0, -570000.0,  246000.0,     -205000.0,   -171000.0, -152000.0
	};
	double lunar_distance = factors[0]
		+ factors[1] * cos(mean_lunar_anomaly / Geolocation::RADIAN)
		+ factors[2] * cos((mean_lunar_and_solar_difference * 2 - mean_lunar_anomaly) / Geolocation::RADIAN)
		+ factors[3] * cos(mean_lunar_and_solar_difference * 2 / Geolocation::RADIAN)
		+ factors[4] * cos(mean_lunar_anomaly * 2 / Geolocation::RADIAN)
		+ factors[5] * cos((mean_lunar_anomaly * 2 - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[6] * cos((mean_solar_anomaly - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[7] * cos((mean_lunar_anomaly + mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN)
		+ factors[8] * cos((mean_lunar_and_solar_anomaly - mean_lunar_and_solar_difference * 2) / Geolocation::RADIAN);

	/*
	```
	|*** convert spherical ecliptic coordinates to equatorial cartesian
	|
	|*** precession of equinox wrt. J2000   (p.71)
	|
	|      selond=selond + 1.3972d0*t                         !*** degrees
	```
	*/
	double solar_ecliptic_longitude_degrees += 1.3972 * terrestrial_time;

	/*
	```
	|*** position vector of moon (mean equinox & ecliptic of J2000) (EME2000, ICRF)
	|***                         (plus long. advance due to precession -- eq. above)
	|
	|      oblir=23.43929111d0/rad        !*** obliquity of the J2000 ecliptic
	|
	|      sselat=dsin(selatd/rad)
	|      cselat=dcos(selatd/rad)
	|      sselon=dsin(selond/rad)
	|      cselon=dcos(selond/rad)
	|
	|      t1 = rse*cselon*cselat        !*** meters          !*** eq. 3.51, p.72
	|      t2 = rse*sselon*cselat        !*** meters          !*** eq. 3.51, p.72
	|      t3 = rse*       sselat        !*** meters          !*** eq. 3.51, p.72
	```
	*/
	double obliquity_ecliptic_radians = 23.43929111 / Geolocation::RADIAN;

	double sin_solar_ecliptic_latitude = sin(solar_ecliptic_latitude_degrees / Geolocation::RADIAN);
	double cos_solar_ecliptic_latitude = cos(solar_ecliptic_latitude_degrees / Geolocation::RADIAN);

/*
```
|      call rot1(-oblir,t1,t2,t3,rm1,rm2,rm3)             !*** eq. 3.51, p.72
|
|*** convert position vector of moon to ECEF  (ignore polar motion/LOD)
|
|      call getghar(mjd,fmjd,ghar)                        !*** sec 2.3.1,p.33
|      call rot3(ghar,rm1,rm2,rm3,rm(1),rm(2),rm(3))      !*** eq. 2.89, p.37
|
|      return
|      end
```
*/
}