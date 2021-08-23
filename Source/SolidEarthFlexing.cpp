
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
	/* LN27–48
	|*** query section
	|
	|    1 write(*,'(a$)') 'Enter year    [1901-2099]: '
	|      read(*,*) iyr
	|      if(iyr.lt.1901.or.iyr.gt.2099) go to 1
	|
	|    2 write(*,'(a$)') 'Enter month number [1-12]: '
	|      read(*,*) imo
	|      if(imo.lt.1.or.imo.gt.12) go to 2
	|
	|    3 write(*,'(a$)') 'Enter day          [1-31]: '
	|      read(*,*) idy
	|      if(idy.lt.1.or.idy.gt.31) go to 3
	|
	|    4 write(*,'(a$)') 'Lat. (pos N.) [- 90, +90]: '
	|      read(*,*) glad
	|      if(glad.lt.-90.d0.or.glad.gt.90.d0) go to 4
	|
	|    5 write(*,'(a$)') 'Lon. (pos E.) [-360,+360]: '
	|      read(*,*) glod
	|      if(glod.lt.-360.d0.or.glod.gt.360.d0) go to 5
	*/
	double latitude = get_number_from_cin("Latitude [-90, +90] °N: ", -90.0, 90.0);
	double longitude = get_number_from_cin("Longitude [-180, +180] °E: ", -180.0, 180.0);

	int year = get_number_from_cin("Year [1901-2099]: ", 1901, 2099);
	int month = get_number_from_cin("Month [1-12]: ", 1, 12);
	int day = get_number_from_cin("Day [1-31]: ", 1, 31);

	return GeoLocation(latitude, longitude, year, month, day);
}



int main(int arg_count, char* arg_variables[])
{
	GeoLocation geolocation = create_geolocation_from_user_input();

	// for(int x = 0; x < MAX_STEPS; x += STEP_SIZE)
	// {
		geolocation.calculate_geocentric_solar_coordinates();
		geolocation.calculate_geocentric_lunar_coordinates();

		double sun[3];
		geolocation.solar_coordinates(sun);
		double moon[3];
		geolocation.lunar_coordinates(moon);

	// }
	
	return 0;
}
