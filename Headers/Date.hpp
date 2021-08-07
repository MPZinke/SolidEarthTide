
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


class Datetime;  // "declare"


class Date
{
	public:
		Date(int, int, int);
		~Date();

		// getters
		int year();  // returns _year attr
		int month();  // returns _month attr
		int day();  // returns _day attr
		int* date_array();  // returns a pointer to an array of {_year, _month, _day} copies
		void date_array(int[]);  // copies _year, _month, _day to array

		// setters
		void year(int);
		void month(int);
		void day(int);
		void date(int, int, int);
		void add_days(unsigned int);

		// time conversion
		int modified_julian_date();

		Datetime datetime();
		Datetime datetime(int, int, double);


	protected:
		int _year;
		int _month;
		int _day;

		int _modified_julian;
		int _initial_modified_julian;

		int* _date_array = NULL;
};

#endif
