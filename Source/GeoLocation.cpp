

#include "Geolocation.hpp"


#include <cmath>


#include "Global.hpp"


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
: _latitude{latitude_degrees / Trig::RADIANS}, _longitude{longitude_degrees / Trig::RADIANS}
{}


void operator<<(double cartesian_coordinates[3], Geolocation& geolocation)
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
	double sin_latitude = 0;
	double cos_latitude = 0;
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
	cartesian_coordinates[X] = (prime_vertical_radius+altitude) * cos_latitude * cos(geolocation._longitude * Trig::RADIANS);
	cartesian_coordinates[Y] = (prime_vertical_radius+altitude) * cos_latitude * sin(geolocation._longitude * Trig::RADIANS);
	cartesian_coordinates[Z] = (prime_vertical_radius*(1.0-Geolocation::GEODETIC_ELLIPSOID) + altitude) * sin_latitude;
}


Geolocation::operator Coordinate()
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
	return (Coordinate){
		/* X = */ (prime_vertical_radius+altitude) * cos_latitude * cos(this -> _longitude * Trig::RADIANS),
		/* Y = */ (prime_vertical_radius+altitude) * cos_latitude * sin(this -> _longitude * Trig::RADIANS),
		/* Z = */ (prime_vertical_radius*(1.0-Geolocation::GEODETIC_ELLIPSOID) + altitude) * sin_latitude
	};
}


Coordinate Geolocation::sun_coordinates(Datetime& datetime)
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

	double time_seconds_UTC = datetime.fractional_m
	/*
	|      emdeg = 357.5256d0 + 35999.049d0*t          !*** degrees
	|      em    = emdeg/rad                           !*** radians
	|      em2   = em+em                               !*** radians
	|
	|*** series expansions in mean anomaly, em   (eq. 3.43, p.71)
	|
	|      r=(149.619d0-2.499d0*dcos(em)-0.021d0*dcos(em2))*1.d9      !*** m.
	|      slond=opod + emdeg + (6892.d0*dsin(em)+72.d0*dsin(em2))/3600.d0
	|
	|*** precession of equinox wrt. J2000   (p.71)
	|
	|      slond=slond + 1.3972d0*t                              !*** degrees
	|
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
	|
	|*** convert position vector of sun to ECEF  (ignore polar motion/LOD)
	|
	|      call getghar(mjd,fmjd,ghar)                        !*** sec 2.3.1,p.33
	|      call rot3(ghar,rs1,rs2,rs3,rs(1),rs(2),rs(3))      !*** eq. 2.89, p.37
	|
	|      return
	|      end
	```
	*/


}
