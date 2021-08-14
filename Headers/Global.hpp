
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

#define UPPER_MODIFIED_JULIAN 58664.0  // solid.f: LN1264: !*** upper limit, leap second table, 2019jun30
#define LOWER_MODIFIED_JULIAN 41317.0  // solid.f: LN1266: !*** lower limit, leap second table, 1972jan01
#define JULIAN_DAYS_IN_MONTH 30.6001
#define MODIFIED_JULIAN_TO_JULIAN 2400000.5
#define MODIFIED_JULIAN_TO_REDUCED_JULIAN 2400000.0
#define JULIAN_TO_REDUCED_JULIAN -0.5
#define ERA_DAYS_TO_1901 679019
#define MODIFIED_JULIAN_START_TO_J2000 51544

// TIME::BASIC::CONVERSION
#define APPR_DAY_IN_YEAR 365.25
#define DAYS_IN_YEAR 365.2425
#define DAYS_PER_MINUTE .0006944444
#define SECONDS_IN_MINUTE 60
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_DAY 86400

#define PROM (double)

// TIME::BASIC::RANGE
// TIME::BASIC::RANGE::YEAR
#define MAX_YEAR 2099
#define MIN_YEAR 1901

// TIME::BASIC::RANGE::YEAR
#define STEP_SIZE 60
#define MAX_STEPS 86400

// TRIG
#define PI_TIMES_TWO 6.28318530718
#define PI 3.1415926  // you know what this is
#define RADIAN 57.29578  // this too


#ifndef NULL
#define NULL 0  // why import an entire library when you can define NULL
#endif

#endif
