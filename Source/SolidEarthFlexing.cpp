
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.08.07                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include <iostream>
#include <fstream>


#include "Global.hpp"
#include "Datetime.hpp"
#include "GeoLocation.hpp"


using namespace std;


template<class TEMP>
TEMP get_number_from_cin(string message, TEMP min, TEMP max)
{
	cout << message;
	TEMP input;
	cin >> input;
	if(!cin.fail() && min <= input && input <= max) return input;

	cin.clear();
	cin.ignore(1024, '\n');
	cout << "That is an incorrect value\n";
	return get_number_from_cin(message, min, max);
}


// get user input for date, time
GeoLocation create_geolocation_from_user_input()
{
	double latitude = get_number_from_cin("Latitude [-90, +90] °N: ", -90.0, 90.0);
	double longitude = get_number_from_cin("Longitude [-180, +180] °E: ", -180.0, 180.0);

	int year = get_number_from_cin("Year [1901-2099]: ", 1901, 2099);
	int month = get_number_from_cin("Month [1-12]: ", 1, 12);
	int day = get_number_from_cin("Day [1-31]: ", 1, 31);

	return GeoLocation(latitude, longitude, Datetime(year, month, day));
}



int main(int arg_count, char* arg_variables[])
{
	GeoLocation geolocation = create_geolocation_from_user_input();
	geolocation.datetime()->standardize_civil_time_and_set_initial_julian_date();
	geolocation.calculate_geocentric_solar_coordinates();
	geolocation.calculate_geocentric_lunar_coordinates();

	double sun[3];
	geolocation.sun_coordinates(sun);
	double moon[3];
	geolocation.moon_coordinates(moon);

	// double ecef[3];  //TESTING
	// geolocation.ECEF_coordinates(ecef);  //TESTING


	// for(int x = 0; x < MAX_STEPS; x += STEP_SIZE)
	// {

	// }
	
	return 0;
}
