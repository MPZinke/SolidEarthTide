
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

#include"GeoLocation.h"


// ———————————————— CONSTRUCTORS & DESTRUCTOR —————————————————

GeoLocation::GeoLocation(double latitude, double longitude, Datetime datetime)
: GeoLocation(latitude, longitude, 0, datetime)
{}


GeoLocation::GeoLocation(double latitude, double longitude, double altitude, Datetime datetime)
: _latitude{latitude}, _longitude{longitude}, _altitude{altitude}, _datetime{datetime}
{
	// prime vertical radius (the appox radius for the rotational deformation of Earth)
	// N(lat) = 1 / ((cos(lat)^2) + E_SQR * (sin(lat)^2)) ^ .5
	// http://mathforum.org/library/drmath/view/51832.html
	double cos_latitude = cos(_latitude * RADIAN);
	double cos_latitude_squ = cos_latitude * cos_latitude;
	double sin_latitude = sin(_latitude * RADIAN);
	double sin_latitude_squ = sin_latitude * sin_latitude;
	double inverse_prime_vertical_radius = pow(cos_latitude_squ + E_SQR * sin_latitude_squ, .5);
	_prime_vertical_radius = 1 / inverse_prime_vertical_radius;

	// convert geodetic coordinates to ECEF (X, Y, Z) coordinates in 
	// take latitude, longitude, height, ECEF array pointer as (double)
	// X = (N(FI) + h) * cos(FI) * cos(LMDA)
	// Y = (N(FI) + h) * cos(FI) * sin(LMDA)
	// Z = ((e^2) * N(FI) + h) * sin(LMDA)
	// insert X, Y, Z into ECEF array
	double latitude_radians = _latitude * RADIAN;
	double longitude_radians = _longitude * RADIAN;
	_ECEF[X] = (EQUITORIAL_RADIUS * _prime_vertical_radius + altitude) * cos(latitude_radians) * cos(longitude_radians);
	_ECEF[Y] = (EQUITORIAL_RADIUS * _prime_vertical_radius + altitude) * cos(latitude_radians) * sin(longitude_radians);
	_ECEF[Z] = (EQUITORIAL_RADIUS * E_SQR * _prime_vertical_radius + altitude) * sin(latitude_radians);
}


GeoLocation::~GeoLocation()
{
	if(_ECEF_coordinates) delete[] _ECEF_coordinates;
	if(_sun_coordinates) delete[] _sun_coordinates;
	if(_moon_coordinates) delete[] _moon_coordinates;
	if(_tide_coordinates) delete[] _tide_coordinates;
}


// —————————————————————— GETTERS ——————————————————————

Datetime* GeoLocation::datetime()
{
	return &_datetime;
}


double* GeoLocation::ECEF_coordinates()
{
	if(!_ECEF_coordinates) _ECEF_coordinates = new double[3];

	_ECEF_coordinates[X] = _ECEF[X];
	_ECEF_coordinates[Y] = _ECEF[Y];
	_ECEF_coordinates[Z] = _ECEF[Z];

	return _ECEF_coordinates;
}


void GeoLocation::ECEF_coordinates(double copy_array[])
{
	copy_array[X] = _ECEF[X];
	copy_array[Y] = _ECEF[Y];
	copy_array[Z] = _ECEF[Z];
}


double* GeoLocation::sun_coordinates()
{
	if(!_sun_coordinates) _sun_coordinates = new double[3];

	_sun_coordinates[X] = _sun[X];
	_sun_coordinates[Y] = _sun[Y];
	_sun_coordinates[Z] = _sun[Z];

	return _sun_coordinates;
}


void GeoLocation::sun_coordinates(double copy_array[])
{
	copy_array[X] = _sun[X];
	copy_array[Y] = _sun[Y];
	copy_array[Z] = _sun[Z];
}


double* GeoLocation::moon_coordinates()
{
	if(!_moon_coordinates) _moon_coordinates = new double[3];

	_moon_coordinates[X] = _moon[X];
	_moon_coordinates[Y] = _moon[Y];
	_moon_coordinates[Z] = _moon[Z];

	return _moon_coordinates;
}


void GeoLocation::moon_coordinates(double copy_array[])
{
	copy_array[X] = _moon[X];
	copy_array[Y] = _moon[Y];
	copy_array[Z] = _moon[Z];
}


double* GeoLocation::tide_coordinates()
{
	if(!_tide_coordinates) _tide_coordinates = new double[3];

	_tide_coordinates[X] = _tide[X];
	_tide_coordinates[Y] = _tide[Y];
	_tide_coordinates[Z] = _tide[Z];

	return _tide_coordinates;
}


void GeoLocation::tide_coordinates(double copy_array[])
{
	copy_array[X] = _tide[X];
	copy_array[Y] = _tide[Y];
	copy_array[Z] = _tide[Z];
}


// —————————————————————— SETTERS ——————————————————————





// ———————————————————— SUN, MOON, TIDE ————————————————————

// solid.f: LN880: subroutine sunxyz(mjd,fmjd,rs,lflag)
// ARGS:	mjd = _modified_julian, fmjd = _fractional_modified_julian, rs = _sun,
//			lflag = _datetime._leap_second_flag
// *** get low-precision, geocentric coordinates for sun (ECEF)
//
// *** input, mjd/fmjd, is Modified Julian Date (and fractional) in UTC time
// *** output, rs, is geocentric solar position vector [m] in ECEF
// ***         lflag  -- leap second table limit flag,  false:flag not raised
// *** 1."satellite orbits: models, methods, applications" montenbruck & gill(2000)
// *** section 3.3.2, pg. 70-71
// *** 2."astronomy on the personal computer, 4th ed." montenbruck & pfleger (2005)
// *** section 3.2, pg. 39  routine MiniSun
void GeoLocation::calculate_geocentric_solar_coordinates()
{
	// *** mean elements for year 2000, sun ecliptic orbit wrt. Earth
	// LN898-905 replaced with defined values
	// LN910: !*** TT  time (sec of day)
	double centuries_TT = _datetime.modified_julian_date_to_Terrestrial_Time_julian_date_centuries();

	// *** julian centuries since 1.5 january 2000 (J2000)
	// ***   (note: also low precision use of mjd --> tjd)
	double em_degrees = centuries_TT * 35999.049 + 357.5256;  // LN919: !*** degrees
	double em = em_degrees / RADIAN;  // LN919: !*** radians

	// LN923: *** series expansions in mean anomaly, em   (eq. 3.43, p.71)
	double radius = (149.619 - 2.499 * cos(em) - .021 * cos(2*em)) * 1000000000;  // meters
	double solar_longitude_degrees = OPOD + em_degrees + (6892 * sin(em) + 72 * sin(2*em)) / SECONDS_IN_HOUR;

	// LN928: *** precession of equinox wrt. J2000   (p.71)
	solar_longitude_degrees += 1.3972 * centuries_TT;

	// LN932: *** position vector of sun (mean equinox & ecliptic of J2000) (EME2000, ICRF)
	// ***                        (plus long. advance due to precession -- eq. above)
	double solar_longitude = solar_longitude_degrees / RADIAN;
	double sin_solar_longitude = sin(solar_longitude);
	double cos_solar_longitude = cos(solar_longitude);

	double solar_radius[3];
	solar_radius[X] = radius * cos_solar_longitude;  // LN939: !*** meters             !*** eq. 3.46, p.71
	// LN940: !*** meters             !*** eq. 3.46, p.71
	solar_radius[Y] = radius * sin_solar_longitude * COS_OBLIQUITY;
	// LN941: !*** meters             !*** eq. 3.46, p.71
	solar_radius[Z] = radius * sin_solar_longitude * SIN_OBLIQUITY;

	// LN943: *** convert position vector of sun to ECEF  (ignore polar motion/LOD)
	double greenwhich_hour_angle = _datetime.greenwhich_hour_angle_radians();  // LN945 !*** sec 2.3.1,p.33
	// LN946: !*** eq. 2.89, p.37
	rotate_around_3_axis(solar_radius, _sun, greenwhich_hour_angle);
}


// solid.f: LN719: subroutine moonxyz(mjd,fmjd,rm,lflag)
// ARGS:	mjd = _modified_julian,  fmjd = _fractional_modified_julian, 
// *** get low-precision, geocentric coordinates for moon (ECEF)
// *** UTC version

// *** input:  mjd/fmjd, is Modified Julian Date (and fractional) in UTC time
// *** output: rm, is geocentric lunar position vector [m] in ECEF
// ***         lflag  -- leap second table limit flag,  false:flag not raised
// *** 1."satellite orbits: models, methods, applications" montenbruck & gill(2000)
// *** section 3.3.2, pg. 72-73
// *** 2."astronomy on the personal computer, 4th ed." montenbruck & pfleger (2005)
// *** section 3.2, pg. 38-39  routine MiniMoon
void GeoLocation::calculate_geocentric_lunar_coordinates()
{
	// LN745-749: *** julian centuries since 1.5 january 2000 (J2000)
	// ***   (note: also low precision use of mjd --> tjd)
	double centuries_TT = _datetime.modified_julian_date_to_Terrestrial_Time_julian_date_centuries();

	// mean_lunar_longitude *** el0 -- mean longitude of Moon (deg)
	// mean_lunar_anomaly *** el  -- mean anomaly of Moon (deg)
	// mean_solar_anomaly *** elp -- mean anomaly of Sun  (deg)
	// mean_angular_distance *** f   -- mean angular distance of Moon from ascending node (deg)
	// *** d   -- difference between mean longitudes of Sun and Moon (deg)
	// *** equations 3.47, p.72
	double mean_lunar_longitude = 218.31617 + centuries_TT * (481267.88088 - 1.3972);
	double mean_lunar_anomaly = 134.96292 + 477198.86753 * centuries_TT;
	double mean_solar_anomaly = 357.52543 +  35999.04944 * centuries_TT;
	double mean_angular_distance = 93.27283 + 483202.01873 * centuries_TT;
	double mean_solar_lunar_longitude = 297.85027 + 445267.11135 * centuries_TT;

	// LN765: *** longitude w.r.t. equinox and ecliptic of year 2000
	double 
}


//TODO: finish equations
double solar_ecliptic_for_year_2000(double mean_lunar_longitude, double mean_lunar_anomaly,
double mean_solar_anomaly, double mean_angular_distance, double mean_solar_lunar_longitude)
{
	double solar_ecliptic = mean_lunar_longitude
	+ 22640 / 3600 * dsin((mean_lunar_anomaly)/RADIAN)
	+ 769 / 3600 * dsin((mean_lunar_anomaly + mean_lunar_anomaly)/RADIAN)
      - 4586 / 3600 * dsin((mean_lunar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude)/RADIAN)
      + 2370 / 3600 * dsin((mean_solar_lunar_longitude + mean_solar_lunar_longitude)/RADIAN)
      - 668 / 3600 * dsin((mean_solar_anomaly)/RADIAN)
      - 412 / 3600 * dsin((f+f)/RADIAN)
      - 212 / 3600 * dsin((mean_lunar_anomaly + mean_lunar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude )/RADIAN)
      - 206 / 3600 * dsin((mean_lunar_anomaly + mean_solar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude)/RADIAN)
      + 192 / 3600 * dsin((mean_lunar_anomaly + mean_solar_lunar_longitude + mean_solar_lunar_longitude)/RADIAN)
      - 165 / 3600 * dsin((mean_solar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude)/RADIAN)
      + 148 / 3600 * dsin((mean_lunar_anomaly - mean_solar_anomaly)/RADIAN)
      - 125 / 3600 * dsin((mean_solar_lunar_longitude)/RADIAN)
      - 110 / 3600 * dsin((mean_lunar_anomaly + mean_solar_anomaly)/RADIAN)
      - 55 / 3600 * dsin((f+f - mean_solar_lunar_longitude - mean_solar_lunar_longitude)/RADIAN)
}


// ————————————————————— TRANSFORMS —————————————————————

// solid.f: LN1025: subroutine rot3(theta,x,y,z,u,v,w)
// ARGS:	theta = theta, x = initial[X], y = initial[Y], z = initial[Z], u = transformed[X], v = transformed[Y],
//			w = transformed[Z]
// *** rotate coordinate axes about 3 axis by angle of theta radians
// *** x,y,z transformed into u,v,w
void GeoLocation::rotate_around_3_axis(double initial[], double transformed[], double theta)
{
	double sin_theta = sin(theta);
	double cos_theta = cos(theta);

	transformed[X] = initial[X] * cos_theta + initial[Y] * sin_theta;
	transformed[Y] = initial[Y] * cos_theta - initial[X] * sin_theta;
	transformed[Z] = initial[Z];
}
