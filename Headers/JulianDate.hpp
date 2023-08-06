

class Datetime;


class JulianDate
{
	public:
		JulianDate(unsigned int modified_julian_date, double fractional_modified_julian_date);
		operator Datetime();

		double to_TerrestrialTime();

	private:
		const unsigned int _modified_julian_date;
		const double _fractional_modified_julian_date;
};
