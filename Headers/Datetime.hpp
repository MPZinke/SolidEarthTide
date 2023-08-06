

class JulianDate;


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

		operator JulianDate();

		unsigned int initial_modified_julian_date();

	private:
		const unsigned int _year;
		const unsigned int _month;
		const unsigned int _day;
		const unsigned int _hour;
		const unsigned int _minute;
		const unsigned int _second;
};
