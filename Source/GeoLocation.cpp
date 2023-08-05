

#include "Geolocation.hpp"
#include <cmath>


#include "Global.hpp"


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
	eht0 — altitude
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
