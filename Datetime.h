
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

#ifndef _Datetime_
#define _Datetime_


#include"Global.h"
#include"Date.h"

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

		// conversion
		double fractional_modified_julian_date();
		void modified_julian_date_to_civil_time();
		double modified_julian_date_to_Terrestrial_Time_julian_date_centuries();
		double greenwhich_hour_angle_radians();

		Date date();

	private:
		int _hour;
		int _minute;
		double _second;

		double _fractional_modified_julian;
		bool _leap_second_flag = false;

		int* _time = NULL;

		// conversion
		double UTC_seconds_to_Terrestrial_Time(double);
		double UTC_to_International_Atomic_Time(double);
};

#endif
