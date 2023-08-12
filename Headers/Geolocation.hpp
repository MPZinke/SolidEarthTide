

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
		|      a=6378137.d0
		|      e2=6.69438002290341574957d-03
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

		/*
		solid.f [LN 160—162]
		```
		|*** nominal second degree and third degree love numbers and shida numbers
		|
		|      data h20/0.6078d0/,l20/0.0847d0/,h3/0.292d0/,l3/0.015d0/
		```
		h20 — SECOND_DEGREE_LOVE
		l20 — SECOND_DEGREE_SHIDA
		h3 — THIRD_DEGREE_LOVE
		l3 — THIRD_DEGREE_SHIDA

		https://www.eas.slu.edu/GGP/BIM_Recent_Issues/bim145-2009/spicakova_etal_deg2_Love-Shida_VLBI_bim145_09.pdf
		*/
		static const double SECOND_DEGREE_LOVE;  // 0.6078: h20/0.6078d0/
		static const double SECOND_DEGREE_SHIDA;  // 0.0847: l20/0.0847d0/
		static const double THIRD_DEGREE_LOVE;  // 0.292: h3/0.292d0/
		static const double THIRD_DEGREE_SHIDA;  // 0.015: l3/0.015d0/

		/*
		solid.f [LN 214–216]
		```
		|      mass_ratio_sun=332945.943062d0
		|      mass_ratio_moon=0.012300034d0
		|      re =6378136.55d0
		```
		*/
		static const double SOLAR_MASS_RATIO;  // 332945.943062: mass_ratio_sun=332945.943062d0
		static const double LUNAR_MASS_RATIO;  // 0.012300034: mass_ratio_moon=0.012300034d0
		static const double RE;  // 6378136.55: re=6378136.55d0

		Geolocation(double latitude_degrees, double longitude_degrees);
		// friend void operator<<(double cartesian_coordinates[3], Geolocation& geolocation);
		operator Coordinate<double>();

		Coordinate<double> sun_coordinates(unsigned int initial_modified_julian_date, JulianDate& julian_date);
		Coordinate<double> moon_coordinates(unsigned int initial_modified_julian_date, JulianDate& julian_date);
		Coordinate<double> tidal_displacement(unsigned int initial_modified_julian_date, JulianDate& julian_date);

		Coordinate<double> mantle_inelasticity_1st_diurnal_band_correction(Coordinate<double> geo_coordinate, 
			Coordinate<double> solar_coordinate, Coordinate<double> lunar_coordinate, double solar_factor2,
			double lunar_factor2 
		);
		Coordinate<double> mantle_inelasticity_semi_diurnal_band_correction(Coordinate<double> geo_coordinate, 
			Coordinate<double> solar_coordinate, Coordinate<double> lunar_coordinate, double solar_factor2,
			double lunar_factor2 
		);

	private:
		const double _latitude;  // Radians
		const double _longitude;  // Radians
};
