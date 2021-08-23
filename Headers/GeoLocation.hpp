
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on ..                                                                                                              *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#ifndef _GeoLocation_
#define _GeoLocation_


#include <cmath>


#include "Datetime.hpp"
#include "Global.hpp"


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
		GeoLocation(double, double, double, double, double);
		GeoLocation(double, double, double, double, double, double);

		// GETTERS
		Datetime* datetime();
		double* ECEF_coordinates();
		void ECEF_coordinates(double[]);
		double* solar_coordinates();
		void solar_coordinates(double[]);
		double* lunar_coordinates();
		void lunar_coordinates(double[]);
		double* tide_coordinates();
		void tide_coordinates(double[]);

		// SUN, MOON, TIDE
		void calculate_geocentric_solar_coordinates();
		void calculate_geocentric_lunar_coordinates();
		void calculate_geocentric_tidal_coordinates();


	private:
		// ATTRIBUTES
		// ATTRIBUTES::TIME
		Datetime _datetime;
		// ATTRIBUTES::GEOCOORDINATES
		// ATTRIBUTES::GEOCOORDINATES::LLA
		const double _latitude;
		const double _longitude;
		const double _altitude;
		// ATTRIBUTES::GEOCOORDINATES::ECEF
		double _ECEF[3];
		// ATTRIBUTES::GEOCOORDINATES::CALCULATED
		double _solar_coordinates[3];
		double _lunar_coordinates[3];
		double _tide_coordinates[3];

		// double _prime_vertical_radius;

		// METHODS
		// METHODS::CALCULATION
		// METHODS::CALCULATION::LUNAR
		double Y2K_lunar_ecliptic_long(double, double, double, double, double);
		double Y2K_lunar_ecliptic_lat(double, double, double, double, double, double);
		double Earth_Moon_distance(double, double, double, double, double);
		// METHODS::TRANSFORMS
		void rotate_coordinates_about_GreenwichHourAngle_radians(double[]);
		void rotate_coordinates_about_obliquity(double[]);
};

#endif
