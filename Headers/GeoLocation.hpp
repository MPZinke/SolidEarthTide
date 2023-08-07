

#include "Coordinate.hpp"


class Datetime;
class JulianDate;


class Geolocation
{
	public:
		static const long double PI;
		static const long double RADIAN;

		/*
		solid.f [LN 22–23]
		```
		      a=6378137.d0
		      e2=6.69438002290341574957d-03
		```
		a = EQUITORIAL_RADIUS
		e2 = E_SQR
		*/
		static const double EQUITORIAL_RADIUS;  // KM: https://www.space.com/17638-how-big-is-earth.html
		// E_SQR: geodetic ellipsoid (Moritz, 1980b)
		// E_SQR: square of the first numerical eccentricity of the ellipsoid
		static const double GEODETIC_ELLIPSOID;

		/*
		solid.f [LN 899...904]
		```
		|*** mean elements for year 2000, sun ecliptic orbit wrt. Earth
		⋮
		|      opod=282.9400d0               !*** RAAN + arg.peri.  (deg.)
		```
		obe — OBLIQUITY
		sobe — SIN_OBLIQUITY
		cobe — COS_OBLIQUITY
		opod — OPOD
		*/
		static const double OBLIQUITY;  // 0.409092804202936: (23.43929111 / rad) obliquity of the J2000 ecliptic
		static const double COS_OBLIQUITY;  // 0.999974510191339: cos(obliquity of the J2000 ecliptic)
		static const double SIN_OBLIQUITY;  // 0.00713995571357085: sin(obliquity of the J2000 ecliptic)
		static const double OPOD;  // 282.9400: RAAN + arg.peri. (deg.)

		Geolocation(double latitude_degrees, double longitude_degrees);
		// friend void operator<<(double cartesian_coordinates[3], Geolocation& geolocation);
		operator Coordinate<double>();

		Coordinate<double> sun_coordinates(unsigned int initial_modified_julian_date, JulianDate& julian_date);

	private:
		const double _latitude;  // Radians
		const double _longitude;  // Radians
};
