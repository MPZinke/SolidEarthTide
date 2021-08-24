
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


#include <iostream>


#include "Date.hpp"


// ——————————————————————————————————————————— CONSTRUCTORS  & DESTRUCTOR ——————————————————————————————————————————— //

// constructor.
// takes year, month, day for object attributes.
// sets attributes and validates values, throwing errrors if value out of range.
Date::Date(int year, int month, int day)
: _year{year}, _month{month}, _day{day}
{
	/* LN1166–1176
	|     if(imo.le.2) then
	|       y=iyr-1
	|       m=imo+12
	|     else
	|       y=iyr
	|       m=imo
	|     endif
	|
	|     it1=365.25d0*y
	|     it2=30.6001d0*(m+1)
	|     mjd=it1+it2+idy-679019
	*/
	int years_to_days = APPR_DAY_IN_YEAR * ((double)_year - (_month <= 2 ? 1.0 : 0.0));
	int months_to_days = JULIAN_DAYS_IN_MONTH * (1.0 + (double)_month + (_month <= 2 ? 12.0 : 0.0));
	_mod_julian = years_to_days + months_to_days + _day - ERA_DAYS_TO_1901;
}


void Date::validate()
{
	if(_year < 1901 || 2099 < _year) throw std::runtime_error("Year is out of range");  // validate year
	if(_month < 1 || 12 < _month) throw std::runtime_error("Month is out of range");  // validate month

	// validate day
	if(_day < 1) throw std::runtime_error("Day is out of range");
	// February
	if(_month == 2)
	{
		if((_year % 4 && 28 < _day) || (29 < _day)) throw std::runtime_error("Day is out of range");
	}
	// 31 day months
	else if((_month <= 7 && _month % 2) || (8 <= _month && _month % 2 == 0))
	{
		if(31 < _day) throw std::runtime_error("Day is out of range");
	}
	// 30 day months
	else if(30 < _day) throw std::runtime_error("Day is out of range");
}


// ———————————————————————————————————————————————————— GETTERS ————————————————————————————————————————————————————— //

// gets the _year attr.
int Date::year()
{
	return _year;
}


// gets the _month attr.
int Date::month()
{
	return _month;
}


// gets the _day attr.
int Date::day()
{
	return _day;
}


// creates a copy array for {_year, _month, _day}.
// returns a pointer to an array of copies.
int* Date::date_array()
{
	int* date_array_copy = new int[3];

	date_array_copy[YEAR] = _year;
	date_array_copy[MONTH] = _month;
	date_array_copy[DAY] = _day;

	return date_array_copy;
}


// copies attributes to array.
// takes destination of copy (must be 3 <= size).
// places _year, _month, _day into 0, 1, 2.
void Date::date_array(int copy_array[])
{
	copy_array[YEAR] = _year;
	copy_array[MONTH] = _month;
	copy_array[DAY] = _day;
}


int Date::ModJulianDate()
{
	return _mod_julian;
}



// ———————————————————————————————————————————————————— SETTERS ————————————————————————————————————————————————————— //

// sets _year attribute.
// takes new year value.
void Date::year(int year)
{
	_year = year;
}


// sets _month attribute.
// takes new month value.
void Date::month(int month)
{
	_month = month;
}


// sets _day attribute.
// takes new day value.
void Date::day(int day)
{
	_day = day;
}


// sets _year, _month, _day attributes.
void Date::date(int year, int month, int day)
{
	_year = year;
	_month = month;
	_day = day;
}


void Date::add_days(int days)
{
	if(365 < _day) throw std::runtime_error("Day range is too large");  // make things easier

	int original_date[3] = {_year, _month, _day};

	int days_in_months[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if((_year % 4 == 0 && _month <= 2) || (_year % 4 == 3 && 2 < _month)) days_in_months[2] = 29;

	_day += days;
	while(days_in_months[_month] < _day)
	{
		_day -= days_in_months[_month];
		_month++;
		if(12 < _month)
		{
			_year++;
			_month = 1;
		}
	}

	if(MAX_YEAR < _year)
	{
		_year = original_date[YEAR];
		_month = original_date[MONTH];
		_day = original_date[DAY];
		throw std::runtime_error("Year is out of range");
	}
}
