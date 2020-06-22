
/********************************************************************************************************************************
*
*	created by: MPZinke
*	on ..
*
*	DESCRIPTION: TEMPLATE
*	BUGS:
*	FUTURE:
*
********************************************************************************************************************************/

#ifndef _GeoLocation_
#define _GeoLocation_

#include"Datetime.h"

#include<cmath>


#define E_SQR .99330562  // flattening deformation of earth from rotation (even though the Earth is flat (JK))
#define TO_RADIANS .0174532825  // used with sine & cosine: http://www.cplusplus.com/reference/cmath/cos/
#define EQUITORIAL_RADIUS 6378137.0  // KM: https://www.space.com/17638-how-big-is-earth.html
#define POLAR_RADIUS 6356000.0  // KM: https://www.space.com/17638-how-big-is-earth.html
#define SIN_OBLIQUITY .397777  // sin(obliquity of the J2000 ecliptic)
#define COS_OBLIQUITY .917482  // cos(obliquity of the J2000 ecliptic)
#define OPOD 282.9400  // RAAN + arg.peri. (deg.)

enum
{
	X,
	Y,
	Z
};


class GeoLocation
{
	public:
		GeoLocation(double, double, Datetime);
		GeoLocation(double, double, double, Datetime);
		~GeoLocation();

		// getters
		Datetime* datetime();
		double* ECEF_coordinates();
		void ECEF_coordinates(double[]);
		double* sun_coordinates();
		void sun_coordinates(double[]);
		double* moon_coordinates();
		void moon_coordinates(double[]);
		double* tide_coordinates();
		void tide_coordinates(double[]);

		// sun, moon, tide
		void calculate_geocentric_solar_coordinates();
		void calculate_geocentric_lunar_coordinates();

		// transforms
		void rotate_around_3_axis(double[], double[], double);

	private:
		const double _latitude;
		const double _longitude;
		const double _altitude;

		double _ECEF[3];
		double* _ECEF_coordinates = NULL;

		double _prime_vertical_radius;

		double _sun[3];
		double* _sun_coordinates = NULL;
		double _moon[3];
		double* _moon_coordinates = NULL;
		double _tide[3];
		double* _tide_coordinates = NULL;

		Datetime _datetime;

		double lunar_ecliptic_longitude_for_year_2000(double, double, double, double, double);
		double lunar_ecliptic_latitude_for_year_2000(double, double, double, double, double, double);
		double distance_from_earth_to_moon(double, double, double, double, double, double);
};

#endif