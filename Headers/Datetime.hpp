
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
		// CONSTRUCTOR
		Datetime(int, int, int);
		Datetime(int, int, int, int, int, double);

		// GETTERS
		// GETTERS::BASIC
		// GETTERS::BASIC::SINGLE
		int hour();
		int minute();
		double second();
		// GETTERS::BASIC::MULTIPLE
		double* time_array();  // returns a pointer to an array of {hour, minute, second}
		void time_array(double copy_array[]);  // copies to an array of {hour, minute, second}
		double* datetime_array();  // returns a pointer to an array of {year, month, day, hour, minute, second}
		void datetime_array(double copy_array[]);  // copies to an array of {year, month, day, hour, minute, second}
		// GETTERS::SPECIFIC
		double FractionalModJulianDate();
		bool leap_second_flag();

		// SETTERS
		// SETTERS::BASIC
		// SETTERS::BASIC::SINGLE
		void hour(int);
		void minute(int);
		void second(double);
		// SETTERS::BASIC::MULTIPLE
		void time(int, int, double);
		void add_time(double);
		// SETTERS::SPECIFIC
		void leap_second_flag(bool state);

		// CONVERSION
		double GreenwichHourAngle_radians();
		double JulianCenturies_since_1stJanuary2000();
		Date date();

	private:
		// ATTRIBUTES
		// ATTRIBUTES::BASIC
		int _hour;
		int _minute;
		double _second;
		// ATTRIBUTES::SPECIFIC
		// ATTRIBUTES::SPECIFIC::JULIAN
		double _fractional_mod_julian;  // Fractional Modified Julian Date
		// ATTRIBUTES::SPECIFIC::CHANGES
		bool _leap_second_flag = false;

		// CONSTRUCTOR
		// CONSTRUCTOR::CALLED
		void CivilTime_to_ModifiedJulianDate();
		void ModifiedJulianDate_to_CivilTime();
		void set_initial_JulianDate();
		// METHODS
		// METHODS::CONVERSION
		double FMJD_to_TerrestrialTime();
		double FMJD_to_UTC();
		double InternationalAtomicTime();
		double InternationalAtomicTime_to_TerrestrialTime(double);
		double JulianDate_converter();
		double TerrestrialTime_seconds();
};


#endif
