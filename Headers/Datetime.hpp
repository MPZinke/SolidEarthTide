


class Datetime
{
	public:
		enum
		{
			JANUARY = 1,
			FEBRUARY,
			MARCH,
			APRIL,
			MAY,
			JUNE,
			JULY,
			AUGUST,
			SEPTEMBER,
			OCTOBER,
			NOVEMBER,
			DECEMBER
		};

		Datetime(unsigned int year, unsigned int month=1, unsigned int day=1, unsigned int hour=0,
		  unsigned int minute=0, unsigned int second=0);

		Datetime normalize();

		void civil_datetime_to_modified_julian_datetime(unsigned int& modified_julian_date, double& fractional_modified_julian_date);
		Datetime modified_julian_datetime_to_civil_datetime(unsigned int modified_julian_date, double fractional_modified_julian_date);

	private:
		unsigned int _year;
		unsigned int _month;
		unsigned int _day;
		unsigned int _hour;
		unsigned int _minute;
		unsigned int _second;
};
