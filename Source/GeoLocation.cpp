
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


#include "GeoLocation.hpp"


// ——————————————————————————————————————————— CONSTRUCTORS & DESTRUCTORS ——————————————————————————————————————————— //

// LN969:      subroutine geoxyz(gla,glo,eht,x,y,z)
// LN971: *** convert geodetic lat, long, ellip ht. to x,y,z
// Constructor for Geolocation to default altitude to 0 by calling GeoLocation::Geolocation(....).
GeoLocation::GeoLocation(double latitude, double longitude, Datetime datetime)
: GeoLocation(latitude, longitude, 0, datetime)
{}


// LN969:      subroutine geoxyz(gla,glo,eht,x,y,z)
// LN971: *** convert geodetic lat, long, ellip ht. to x,y,z
// Constructor for Geolocation with altitude parameter.
// Converts LLA (Latitude, Longitude, Altitude) to ECEF.
// Formula from solid.f confirmed by: http://danceswithcode.net/engineeringnotes/geodetic_to_ecef/geodetic_to_ecef.html
// Labels: https://en.wikipedia.org/wiki/Geographic_coordinate_conversion#From_geodetic_to_ECEF_coordinates
GeoLocation::GeoLocation(double latitude, double longitude, double altitude, Datetime datetime)
: _latitude{latitude}, _longitude{longitude}, _altitude{altitude}, _datetime{datetime}
{
	// LN976–980
	//      sla=dsin(gla)
	//      cla=dcos(gla)
	//      w2=1.d0-e2*sla*sla
	//      w=dsqrt(w2)
	//      en=a/w
	double sin_latitude = sin(_latitude * RADIAN);
	double cos_latitude = cos(_latitude * RADIAN);
	double _prime_vertical_radius = EQUITORIAL_RADIUS / pow((double)1.0 - E_SQR * sin_latitude * sin_latitude, 0.5);

	// LN982–984
	//      x=(en+eht)*cla*dcos(glo)
	//      y=(en+eht)*cla*dsin(glo)
	//      z=(en*(1.d0-e2)+eht)*sla
	_ECEF[X] = (_prime_vertical_radius+_altitude) * cos_latitude * cos(_longitude * RADIAN);
	_ECEF[Y] = (_prime_vertical_radius+_altitude) * cos_latitude * sin(_longitude * RADIAN);
	_ECEF[Z] = (_prime_vertical_radius * ONE_MINUS_E_SQR + _altitude) * sin_latitude;
}


GeoLocation::~GeoLocation()
{
	if(_ECEF_coordinates) delete[] _ECEF_coordinates;
	if(_sun_coordinates) delete[] _sun_coordinates;
	if(_moon_coordinates) delete[] _moon_coordinates;
	if(_tide_coordinates) delete[] _tide_coordinates;
}


// ———————————————————————————————————————————————————— GETTERS ————————————————————————————————————————————————————— //

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


// ———————————————————————————————————————————————————— SETTERS ————————————————————————————————————————————————————— //


// ———————————————————————————————————————————————— SUN, MOON, TIDE ————————————————————————————————————————————————— //

// LN880–890
//       subroutine sunxyz(mjd,fmjd,rs,lflag)
// 
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
	// The following block can and has been statically defined
	// LN899–904
	// 	*** mean elements for year 2000, sun ecliptic orbit wrt. Earth
	// 
	//       obe =23.43929111d0/rad        !*** obliquity of the J2000 ecliptic
	//       sobe=dsin(obe)
	//       cobe=dcos(obe)
	//       opod=282.9400d0               !*** RAAN + arg.peri.  (deg.)

	// LN906
	// *** use TT for solar ephemerides
	// LN918–921
	//       t     = (tjdtt - 2451545.d0)/36525.d0       !*** julian centuries, TT
	//       emdeg = 357.5256d0 + 35999.049d0*t          !*** degrees
	//       em    = emdeg/rad                           !*** radians
	//       em2   = em+em                               !*** radians
	double JulianCenturies = _datetime.JulianCenturies_since_1stJanuary2000();
	double solar_ephemerides_degrees = (357.5256 + 35999.049 * JulianCenturies);
	double solar_ephemerides = solar_ephemerides_degrees * TO_RADIANS;

	// LN923–926
	// *** series expansions in mean anomaly, em   (eq. 3.43, p.71)
	// 
	//       r=(149.619d0-2.499d0*dcos(em)-0.021d0*dcos(em2))*1.d9      !*** m.
	//       slond=opod + emdeg + (6892.d0*dsin(em)+72.d0*dsin(em2))/3600.d0
	double radius = (149.619 - 2.499 * cos(solar_ephemerides) - 0.021 * cos(solar_ephemerides * 2)) * 1.0e9;
	double solar_longitude_part = (6892.0 * sin(solar_ephemerides) + 72.0 * sin(solar_ephemerides * 2)) / 3600.0;
	double solar_longitude = OPOD + solar_ephemerides_degrees + solar_longitude_part;

	// LN928–930
	// *** precession of equinox wrt. J2000   (p.71)
	// 
	//       slond=slond + 1.3972d0*t                              !*** degrees
	solar_longitude += 1.3972 * JulianCenturies;

	// *** position vector of sun (mean equinox & ecliptic of J2000) (EME2000, ICRF)
	// ***                        (plus long. advance due to precession -- eq. above)
	// 
	//       slon =slond/rad                                       !*** radians
	//       sslon=dsin(slon)
	//       cslon=dcos(slon)
	// 
	//       rs1 = r*cslon              !*** meters             !*** eq. 3.46, p.71
	//       rs2 = r*sslon*cobe         !*** meters             !*** eq. 3.46, p.71
	//       rs3 = r*sslon*sobe         !*** meters             !*** eq. 3.46, p.71

}


// // solid.f: LN880: subroutine sunxyz(mjd,fmjd,rs,lflag)
// // ARGS:	mjd = _modified_julian, fmjd = _fractional_modified_julian, rs = _sun,
// //			lflag = _datetime._leap_second_flag
// // *** get low-precision, geocentric coordinates for sun (ECEF)
// //
// // *** input, mjd/fmjd, is Modified Julian Date (and fractional) in UTC time
// // *** output, rs, is geocentric solar position vector [m] in ECEF
// // ***	flag  -- leap second table limit flag,  false:flag not raised
// // *** 1."satellite orbits: models, methods, applications" montenbruck & gill(2000)
// // *** section 3.3.2, pg. 70-71
// // *** 2."astronomy on the personal computer, 4th ed." montenbruck & pfleger (2005)
// // *** section 3.2, pg. 39  routine MiniSun
// void GeoLocation::calculate_geocentric_solar_coordinates()
// {
// 	// *** mean elements for year 2000, sun ecliptic orbit wrt. Earth
// 	// LN898–905 replaced with defined values
// 	// LN910: !*** TT  time (sec of day)
// 	double centuries_TT = _datetime.modified_julian_date_to_Terrestrial_Time_julian_date_centuries();

// 	// *** julian centuries since 1.5 january 2000 (J2000)
// 	// ***   (note: also low precision use of mjd --> tjd)
// 	double em_degrees = centuries_TT * 35999.049 + 357.5256;  // LN919: !*** degrees
// 	double em = em_degrees / RADIAN;  // LN919: !*** radians

// 	// LN923: *** series expansions in mean anomaly, em   (eq. 3.43, p.71)
// 	double radius = (149.619 - 2.499 * cos(em) - .021 * cos(2*em)) * 1000000000;  // meters
// 	double solar_longitude_degrees = OPOD + em_degrees + (6892 * sin(em) + 72 * sin(2*em)) / SECONDS_IN_HOUR;

// 	// LN928: *** precession of equinox wrt. J2000   (p.71)
// 	solar_longitude_degrees += 1.3972 * centuries_TT;

// 	// LN932: *** position vector of sun (mean equinox & ecliptic of J2000) (EME2000, ICRF)
// 	// ***	lus long. advance due to precession -- eq. above)
// 	double solar_longitude = solar_longitude_degrees / RADIAN;
// 	double sin_solar_longitude = sin(solar_longitude);
// 	double cos_solar_longitude = cos(solar_longitude);

// 	double solar_radius[3];
// 	solar_radius[X] = radius * cos_solar_longitude;  // LN939: !*** meters	*** eq. 3.46, p.71
// 	// LN940: !*** meters	*** eq. 3.46, p.71
// 	solar_radius[Y] = radius * sin_solar_longitude * COS_OBLIQUITY;
// 	// LN941: !*** meters	*** eq. 3.46, p.71
// 	solar_radius[Z] = radius * sin_solar_longitude * SIN_OBLIQUITY;

// 	// LN943: *** convert position vector of sun to ECEF  (ignore polar motion/LOD)
// 	double greenwhich_hour_angle = _datetime.GreenwhichHour_angle_radians();  // LN945 !*** sec 2.3.1,p.33

// 	// LN946: !*** eq. 2.89, p.37
// 	rotate_around_3_axis(solar_radius, _sun, greenwhich_hour_angle);
// }


// solid.f: LN719: subroutine moonxyz(mjd,fmjd,rm,lflag)
// ARGS:	mjd = _modified_julian,  fmjd = _fractional_modified_julian, 
// *** get low-precision, geocentric coordinates for moon (ECEF)
// *** UTC version

// *** input:  mjd/fmjd, is Modified Julian Date (and fractional) in UTC time
// *** output: rm, is geocentric lunar position vector [m] in ECEF
// ***	flag  -- leap second table limit flag,  false:flag not raised
// *** 1."satellite orbits: models, methods, applications" montenbruck & gill(2000)
// *** section 3.3.2, pg. 72-73
// *** 2."astronomy on the personal computer, 4th ed." montenbruck & pfleger (2005)
// *** section 3.2, pg. 38-39  routine MiniMoon
void GeoLocation::calculate_geocentric_lunar_coordinates()
{
	// // LN745–749: *** julian centuries since 1.5 january 2000 (J2000)
	// // ***   (note: also low precision use of mjd --> tjd)
	// double centuries_TT = _datetime.modified_julian_date_to_Terrestrial_Time_julian_date_centuries();
	
	// // mean_lunar_longitude *** el0 -- mean longitude of Moon (deg)
	// // mean_lunar_anomaly *** el  -- mean anomaly of Moon (deg)
	// // mean_solar_anomaly *** elp -- mean anomaly of Sun  (deg)
	// // mean_angular_distance *** f   -- mean angular distance of Moon from ascending node (deg)
	// // mean_solar_lunar_longitude *** d   -- difference between mean longitudes of Sun and Moon (deg)
	// // *** equations 3.47, p.72
	// double mean_lunar_longitude = 218.31617 + centuries_TT * (481267.88088 - 1.3972);
	// double mean_lunar_anomaly = 134.96292 + 477198.86753 * centuries_TT;
	// double mean_solar_anomaly = 357.52543 + 35999.04944 * centuries_TT;
	// double mean_angular_distance = 93.27283 + 483202.01873 * centuries_TT;
	// double mean_solar_lunar_longitude = 297.85027 + 445267.11135 * centuries_TT;

	// // LN765: *** longitude w.r.t. equinox and ecliptic of year 2000
	// // LN767: *** eq 3.48, p.72
	// double lunar_ecliptic_longitude = lunar_ecliptic_longitude_for_year_2000(mean_lunar_longitude,
	// 	mean_lunar_anomaly, mean_solar_anomaly, mean_angular_distance, mean_solar_lunar_longitude);

	// double lunar_ecliptic_latitude = lunar_ecliptic_latitude_for_year_2000(mean_lunar_longitude,
	// 	mean_lunar_anomaly, mean_solar_anomaly, mean_angular_distance, mean_solar_lunar_longitude,
	// 	lunar_ecliptic_longitude);

	// // LN800: *** distance from Earth center to Moon (m)
	// double earth_moon_distance = distance_from_earth_to_moon(mean_lunar_longitude, mean_lunar_anomaly,
	// 	mean_solar_anomaly, mean_angular_distance, mean_solar_lunar_longitude, lunar_ecliptic_longitude);
}


// solid.f: LN765: *** longitude w.r.t. equinox and ecliptic of year 2000
// LN767: *** eq 3.48, p.72
	// mean_lunar_longitude *** el0 -- mean longitude of Moon (deg)
	// mean_lunar_anomaly *** el  -- mean anomaly of Moon (deg)
	// mean_solar_anomaly *** elp -- mean anomaly of Sun  (deg)
	// mean_angular_distance *** f   -- mean angular distance of Moon from ascending node (deg)
	// mean_solar_lunar_longitude *** d   -- difference between mean longitudes of Sun and Moon (deg)
// LN767–81
//	selond=el0                                      !*** eq 3.48, p.72
//	* +22640.d0/3600.d0*dsin((el        )/rad)
//	* +  769.d0/3600.d0*dsin((el+el     )/rad)
//	* - 4586.d0/3600.d0*dsin((el-d-d    )/rad)
//	* + 2370.d0/3600.d0*dsin((d+d       )/rad)
//	* -  668.d0/3600.d0*dsin((elp       )/rad)
//	* -  412.d0/3600.d0*dsin((f+f       )/rad)
//	* -  212.d0/3600.d0*dsin((el+el-d-d )/rad)
//	* -  206.d0/3600.d0*dsin((el+elp-d-d)/rad)
//	* +  192.d0/3600.d0*dsin((el+d+d    )/rad)
//	* -  165.d0/3600.d0*dsin((elp-d-d   )/rad)
//	* +  148.d0/3600.d0*dsin((el-elp    )/rad)
//	* -  125.d0/3600.d0*dsin((d         )/rad)
//	* -  110.d0/3600.d0*dsin((el+elp    )/rad)
//	* -   55.d0/3600.d0*dsin((f+f-d-d   )/rad)
// it would be better to split this into parts, but I do not know what each part is.
double GeoLocation::lunar_ecliptic_longitude_for_year_2000(double mean_lunar_longitude, double mean_lunar_anomaly,
double mean_solar_anomaly, double mean_angular_distance, double mean_solar_lunar_longitude)
{
	double lunar_ecliptic_longitude = mean_lunar_longitude 
		+ 6.2888888889 * sin(mean_lunar_anomaly)
		+ .2136111111 * sin(mean_lunar_anomaly + mean_lunar_anomaly) 
		- 1.2738888889 * sin(mean_lunar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude)
		+ .6583333333 * sin(mean_solar_lunar_longitude + mean_solar_lunar_longitude)
		- .1855555556 * sin(mean_solar_anomaly) 
		- .1144444444 * sin(mean_angular_distance + mean_angular_distance)
		- .0588888889 * sin(mean_lunar_anomaly + mean_lunar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude)
		- .0572222222 * sin(mean_lunar_anomaly + mean_solar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude)
		+ .0533333333 * sin(mean_lunar_anomaly + mean_solar_lunar_longitude + mean_solar_lunar_longitude)
		- .0458333333 * sin(mean_solar_anomaly - mean_solar_lunar_longitude - mean_solar_lunar_longitude)
		+ .0411111111 * sin(mean_lunar_anomaly - mean_solar_anomaly)
		- .0347222222 * sin(mean_solar_lunar_longitude)
		- .0305555556 * sin(mean_lunar_anomaly + mean_solar_anomaly)
		- .0152777778 * sin(mean_angular_distance + mean_angular_distance - mean_solar_lunar_longitude - mean_solar_lunar_longitude);

	return lunar_ecliptic_longitude;
}


// solid.f: LN783: *** latitude w.r.t. equinox and ecliptic of year 2000
	// mean_lunar_longitude *** el0 -- mean longitude of Moon (deg)
	// mean_lunar_anomaly *** el  -- mean anomaly of Moon (deg)
	// mean_solar_anomaly *** elp -- mean anomaly of Sun  (deg)
	// mean_angular_distance *** f   -- mean angular distance of Moon from ascending node (deg)
	// mean_solar_lunar_longitude *** d   -- difference between mean longitudes of Sun and Moon (deg)
	// lunar_exliptic_longitude *** selatd
// LN7845–96: 
//	q = 412.d0/3600.d0*dsin((f+f)/rad)              !*** temporary term
//	*   +541.d0/3600.d0*dsin((elp)/rad)

//	selatd=                                         !*** eq 3.49, p.72
//	* +18520.d0/3600.d0*dsin((f+selond-el0+q)/rad)
//	* -  526.d0/3600.d0*dsin((f-d-d     )/rad)
//	* +   44.d0/3600.d0*dsin((el+f-d-d  )/rad)
//	* -   31.d0/3600.d0*dsin((-el+f-d-d )/rad)
//	* -   25.d0/3600.d0*dsin((-el-el+f  )/rad)
//	* -   23.d0/3600.d0*dsin((elp+f-d-d )/rad)
//	* +   21.d0/3600.d0*dsin((-el+f     )/rad)
//	* +   11.d0/3600.d0*dsin((-elp+f-d-d)/rad)
double GeoLocation::lunar_ecliptic_latitude_for_year_2000(double mean_lunar_longitude, double mean_lunar_anomaly, 
double mean_solar_anomaly, double mean_angular_distance, double mean_solar_lunar_longitude, 
double lunar_ecliptic_longitude)
{
	// LN785: *** temporary term
	double temp_value = 412 / 3600 * sin(mean_angular_distance + mean_angular_distance) + 541 / 3600
		* sin(mean_angular_distance);

	double mean_solar_lunar_longitude_DBL = 2 * mean_solar_lunar_longitude;

	// LN788: *** eq 3.49, p.72
	double lunar_ecliptic_latitude = 18520 / 3600 
		* sin(mean_angular_distance + lunar_ecliptic_longitude - mean_lunar_longitude + temp_value) - 526 / 3600 
		* sin(mean_angular_distance - mean_solar_lunar_longitude_DBL) + 44 / 3600 
		* sin(mean_lunar_anomaly + mean_angular_distance - mean_solar_lunar_longitude_DBL) - 31 / 3600
		* sin(-mean_lunar_anomaly + mean_angular_distance - mean_solar_lunar_longitude_DBL) - 25 / 3600
		* sin(-mean_lunar_anomaly - mean_lunar_anomaly + mean_angular_distance) - 23 / 3600
		* sin(mean_solar_anomaly + mean_angular_distance - mean_solar_lunar_longitude_DBL) + 21 / 3600
		* sin(-mean_lunar_anomaly + mean_angular_distance) + 11 / 3600 
		* sin(-mean_solar_anomaly + mean_angular_distance - mean_solar_lunar_longitude_DBL);

	return lunar_ecliptic_latitude;
}


// solid.f LN798: *** distance from Earth center to Moon (m)
	// mean_lunar_longitude *** el0 -- mean longitude of Moon (deg)
	// mean_lunar_anomaly *** el  -- mean anomaly of Moon (deg)
	// mean_solar_anomaly *** elp -- mean anomaly of Sun  (deg)
	// mean_angular_distance *** f   -- mean angular distance of Moon from ascending node (deg)
	// mean_solar_lunar_longitude *** d   -- difference between mean longitudes of Sun and Moon (deg)
	// lunar_exliptic_longitude *** selatd
// LN800:
// rse= 385000.d0*1000.d0                          !*** eq 3.50, p.72
// *   -  20905.d0*1000.d0*dcos((el        )/rad)
// *   -   3699.d0*1000.d0*dcos((d+d-el    )/rad)
// *   -   2956.d0*1000.d0*dcos((d+d       )/rad)
// *   -    570.d0*1000.d0*dcos((el+el     )/rad)
// *   +    246.d0*1000.d0*dcos((el+el-d-d )/rad)
// *   -    205.d0*1000.d0*dcos((elp-d-d   )/rad)
// *   -    171.d0*1000.d0*dcos((el+d+d    )/rad)
// *   -    152.d0*1000.d0*dcos((el+elp-d-d)/rad)
double GeoLocation::distance_from_earth_to_moon(double mean_lunar_longitude, double mean_lunar_anomaly, 
double mean_solar_anomaly, double mean_angular_distance, double mean_solar_lunar_longitude, 
double lunar_ecliptic_longitude)
{
	double mean_solar_lunar_longitude_DBL = 2 * mean_solar_lunar_longitude;
	double mean_lunar_anomaly_DBL = 2 * mean_lunar_anomaly;

	// LN800: !*** eq 3.50, p.72
	double earth_moon_distance = 385000000
		- 20905000 * cos(mean_lunar_anomaly)
		- 3699000 * cos(mean_solar_lunar_longitude_DBL - mean_lunar_anomaly)
		- 2956000 * cos(mean_solar_lunar_longitude_DBL)
		- 570000 * cos(mean_lunar_anomaly_DBL)
		+ 246000 * cos(mean_lunar_anomaly_DBL - mean_solar_lunar_longitude_DBL)
		- 205000 * cos(mean_solar_anomaly - mean_solar_lunar_longitude_DBL)
		- 171000 * cos(mean_lunar_anomaly + mean_solar_lunar_longitude_DBL)
		- 152000 * cos(mean_lunar_anomaly + mean_solar_anomaly - mean_solar_lunar_longitude_DBL);

	return earth_moon_distance;
}


// ——————————————————————————————————————————————————— TRANSFORMS ——————————————————————————————————————————————————— //

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
