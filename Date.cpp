
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

#include<iostream>

#include"Date.h"


// ———————————————— CONSTRUCTORS & DESTRUCTOR —————————————————

// constructor.
// takes year, month, day for object attributes.
// sets attributes and validates values, throwing errrors if value out of range.
Date::Date(int year, int month, int day)
: _year{year}, _month{month}, _day{day}
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


// destructor.
// deallocates array if exists.
Date::~Date()
{
	if(_date_array) delete[] _date_array;
}


// —————————————————————— GETTERS ——————————————————————

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
	if(!_date_array) _date_array = new int[3];

	_date_array[YEAR] = _year;
	_date_array[MONTH] = _month;
	_date_array[DAY] = _day;

	return _date_array;
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



// —————————————————————— SETTERS ——————————————————————

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


void Date::add_days(unsigned int days)
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
		std::runtime_error("Year is out of range");
	}
}


// ——————————————————— TIME CONVERSION ————————————————————

// solid.f: LN1152: subroutine civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
// ARGS: 	iyr = _year, imo = _month, idy = _day, ihr = _hour, imn = _minute, sec = _second, 
//			mjd = modified_julian, fmjd = fractional_modified_julian
// *** convert civil date to modified julian date
// *** imo in range 1-12, idy in range 1-31
// *** only valid in range mar-1900 thru feb-2100     (leap year protocols)
// *** ref: hofmann-wellenhof, 2nd ed., pg 34-35
// *** operation confirmed against table 3.3 values on pg.34
int Date::modified_julian_date()
{
	int year = _year;
	int month = _month;
	if(_month < 2)
	{
		year--;
		month += 12;
	}
	int days_of_era = DAYS_IN_YEAR * (double)year;
	int months_of_year_in_days = JULIAN_DAYS_IN_MONTH * (double)(month + 1);
	int modified_julian = days_of_era + months_of_year_in_days + _day - ERA_DAYS_TO_1901;

	_modified_julian = modified_julian;
	return modified_julian;
}