
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
		// CONSTRUCTOR::CALLED
		void CivilTime_to_ModifiedJulianDate();
		void ModifiedJulianDate_to_CivilTime();
		void set_initial_JulianDate();
		// DESTRUCTOR
		~Datetime();

		// GETTERS
		int hour();
		int minute();
		double second();
		int ModJulianDate();
		double FractionalModJulianDate();
		int* time_array();  // returns a pointer to an array of {hour, minute, second}
		void time_array(int[]);  // copies to an array of {hour, minute, second}
		int* datetime_array();  // returns a pointer to an array of {year, month, day, hour, minute, second}
		void datetime_array(int[]);  // copies to an array of {year, month, day, hour, minute, second}

		// SETTERS
		void hour(int);
		void minute(int);
		void second(int);
		void time(int, int, double);
		void add_time(double);

		// CONVERSION
		double FMJD_to_TerrestrialTime();
		double FMJD_to_UTC();
		double GreenwichHourAngle_radians();
		double InternationalAtomicTime();
		double InternationalAtomicTime_to_TerrestrialTime(double);
		double JulianDate_converter();
		double JulianCenturies_since_1stJanuary2000();
		double TerrestrialTime_seconds();

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
};


#endif
