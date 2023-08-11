

#include "Coordinate.hpp"


class Datetime;


class JulianDate
{
	public:
		static const unsigned int MJDUPPER;
		static const unsigned int MJDLOWER;

		JulianDate(unsigned int modified_julian_date, double fractional_modified_julian_date);
		operator Datetime();

		unsigned int modified_julian_date();
		double JulianCenturies(unsigned int initial_modified_julian_date);
		double TerrestrialTime(unsigned int initial_modified_julian_date);
		double UTC_to_TAI(unsigned int initial_modified_julian_date);

		double GreenwichHourAngleRadians();

	private:
		const unsigned int _modified_julian_date;
		const double _fractional_modified_julian_date;
};
