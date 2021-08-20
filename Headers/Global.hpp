
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


#ifndef _Global_
#define _Global_


// TIME::CALENDAR::CONVERSION
#define INTERNATION_ATOMIC_TIME_TO_TERRESTRIAL_TIME 32.184

#define ERA_DAYS_TO_1901 679019
#define GREGORIAN_DAYS_TO_JULIAN_DAYS 2451545.0  // solid.f: LN918
#define JULIAN_DAYS_IN_MONTH 30.6001
#define JULIAN_DAYS_TO_CENTURIES 36525.0  // solid.f: LN918
#define JULIAN_TO_REDUCED_JULIAN -0.5
#define LOWER_MODIFIED_JULIAN 41317.0  // solid.f: LN1266: !*** lower limit, leap second table, 1972jan01
#define MODIFIED_JULIAN_TO_JULIAN 2400000.5  // solid.f: LN917
#define MODIFIED_JULIAN_START_TO_J2000 51544
#define UPPER_MODIFIED_JULIAN 58664.0  // solid.f: LN1264: !*** upper limit, leap second table, 2019jun30

// TIME::BASIC::CONVERSION
#define APPR_DAY_IN_YEAR 365.25
#define DAYS_IN_YEAR 365.2425
#define DAYS_PER_MINUTE .0006944444
#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60


// TIME::BASIC::RANGE
// TIME::BASIC::RANGE::YEAR
#define MAX_YEAR 2099
#define MIN_YEAR 1901

// TIME::BASIC::RANGE::SECONDS
#define STEP_SIZE 60
#define MAX_STEPS 86400


// GEOGRAPHY
#define COS_OBLIQUITY .917482  // cos(obliquity of the J2000 ecliptic)
#define EQUITORIAL_RADIUS 6378137.0  // KM: https://www.space.com/17638-how-big-is-earth.html
// E_SQR: geodetic ellipsoid (Moritz, 1980b)
// E_SQR: square of the first numerical eccentricity of the ellipsoid
#define E_SQR .00669438002290341574957
#define ONE_MINUS_E_SQR 0.99330561997709658425043  // rotational flattening deform.: 1.0 - 6.69438002290341574957d-03
#define OPOD 282.9400  // RAAN + arg.peri. (deg.)
#define POLAR_RADIUS 6356000.0  // KM: https://www.space.com/17638-how-big-is-earth.html
#define SIN_OBLIQUITY .397777  // sin(obliquity of the J2000 ecliptic)


// TRIG
#define PI 3.1415926  // you know what this is
#define PI_TIMES_TWO 6.28318530718
#define RADIAN 57.29578  // this too
#define TO_RADIANS .0174532825  // used with sine & cosine: http://www.cplusplus.com/reference/cmath/cos/


// TYPE
#define PROM (double)


#ifndef NULL
#define NULL 0  // why import an entire library when you can define NULL
#endif

#endif
