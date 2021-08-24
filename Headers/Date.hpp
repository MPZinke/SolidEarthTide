
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


#ifndef _Date_
#define _Date_

#include "Global.hpp"


enum
{
	YEAR,
	MONTH,
	DAY
};


class Datetime;  // tell compiler that class exists


class Date
{
	public:
		Date(int, int, int);

		// GETTERS
		// GETTERS::SINGLE
		int year();
		int month();
		int day();
		// GETTERS::SINGLE::SPECIFIC
		int ModJulianDate();
		// GETTERS::MULTIPLE
		int* date_array();  // returns a pointer to an array of {_year, _month, _day} copies
		void date_array(int copy_array[]);  // copies _year, _month, _day to array
		// GETTERS::TYPE CONVERSION
		Datetime datetime();
		Datetime datetime(int, int, double);

		// SETTERS
		// SETTERS::SINGLE
		void year(int);
		void month(int);
		void day(int);
		// SETTERS::MULTIPLE
		void date(int, int, int);
		void add_days(int);

	protected:
		// ATTRIBUTES
		// ATTRIBUTES::BASIC
		int _year;
		int _month;
		int _day;
		// ATTRIBUTES::SPECIFIC
		int _mod_julian;
		int _initial_mod_julian;

		// METHODS
		void validate();
};

#endif
