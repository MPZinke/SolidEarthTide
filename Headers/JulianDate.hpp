

class Datetime;


class JulianDate
{
	public:
		static const unsigned int MJDUPPER;
		static const unsigned int MJDLOWER;

		JulianDate(unsigned int modified_julian_date, double fractional_modified_julian_date);
		operator Datetime();

		unsigned int modified_julian_date();
		double to_TerrestrialTime();
		double UTC_to_TerrestrialTime(double time_seconds_UTC);
		double UTC_to_TAI(double time_seconds_UTC);
		double TAI_to_TerrestrialTime(double time_seconds_UTC);
		double UTC_minus_TAI(double time_seconds_UTC);

	private:
		const unsigned int _modified_julian_date;
		const double _fractional_modified_julian_date;
};
