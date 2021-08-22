
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
		~GeoLocation();

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


	private:
		const double _latitude;
		const double _longitude;
		const double _altitude;

		double _ECEF[3];
		double* _ECEF_coordinates = NULL;

		// double _prime_vertical_radius;

		double _solar_coordinates[3];
		double* _solar_coordinates_dynamic = NULL;
		double _lunar_coordinates[3];
		double* _lunar_coordinates_dynamic = NULL;
		double _tide_coordinates[3];
		double* _tide_coordinates_dynamic = NULL;

		Datetime _datetime;

		// TRANSFORMS
		void rotate_coordinates_about_GreenwichHourAngle_radians(double[]);

		double lunar_ecliptic_longitude_for_year_2000(double, double, double, double, double);
		double lunar_ecliptic_latitude_for_year_2000(double, double, double, double, double, double);
		double distance_from_earth_to_moon(double, double, double, double, double, double);
};

#endif