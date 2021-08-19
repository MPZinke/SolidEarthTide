
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


#ifndef _Datetime_
#define _Datetime_


#include "Global.hpp"
#include "Date.hpp"


enum
{
	HOUR,
	MINUTE,
	SECOND
};


class Datetime : public Date
{
	public:
		Datetime(int, int, int);
		Datetime(int, int, int, int, int, double);
		~Datetime();

		// usage
		void standardize_civil_time_and_set_initial_julian_date();

		// getters
		int hour();
		int minute();
		double second();
		int* time_array();  // returns a pointer to an array of {hour, minute, second}
		void time_array(int[]);  // copies to an array of {hour, minute, second}
		int* datetime_array();  // returns a pointer to an array of {year, month, day, hour, minute, second}
		void datetime_array(int[]);  // copies to an array of {year, month, day, hour, minute, second}

		// setters
		void hour(int);
		void minute(int);
		void second(int);
		void time(int, int, double);
		void add_time(double);

		// CONVERSION
		double FMJD_to_TerrestrialTime();
		double FMJD_to_UTC();
		double InternationalAtomicTime();
		double InternationalAtomicTime_to_TerrestrialTime(double);
		double JulianDate_converter();
		double TerrestrialTime_seconds();
		// CONVERSION::MODIFIERS
		void CivilTime_to_ModifiedJulianDate();
		void ModifiedJulianDate_to_CivilTime();
		void set_initial_JulianDate();

		double fractional_modified_julian_date();
		void modified_julian_date_to_civil_time();
		double modified_julian_date_to_Terrestrial_Time_julian_date_centuries();
		double greenwhich_hour_angle_radians();

		Date date();

	private:
		int _hour;
		int _minute;
		double _second;

		double _initial_mod_julian;  // initial Modified Julian Date
		double _mod_julian;  // Modified Julian Date
		double _fractional_mod_julian;  // Fractional Modified Julian Date

		bool _leap_second_flag = false;

		int* _time = NULL;

		// conversion
		double UTC_seconds_to_Terrestrial_Time(double);
		double UTC_to_International_Atomic_Time(double);
};


#endif
