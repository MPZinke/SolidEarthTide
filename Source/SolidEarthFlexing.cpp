

#include <iostream>


#include "Geolocation.hpp"
#include "Datetime.hpp"
#include "JulianDate.hpp"


template<class T>
T get_number_from_cin(std::string message, T min, T max)
{
	std::cout << message;
	T input;
	std::cin >> input;
	if(!std::cin.fail() && min <= input && input <= max)
	{
		return input;
	}

	std::cin.clear();
	std::cin.ignore(1024, '\n');
	std::cout << "That is an incorrect value\n";
	return get_number_from_cin(message, min, max);
}


Datetime date_from_user()
{
	/*
	solid.f [LN 30–40]
	```
	|    1 write(*,'(a$)') 'Enter year    [1901-2099]: '
	|      read(*,*) iyr
	|      if(iyr.lt.1901.or.iyr.gt.2099) go to 1
	|
	|    2 write(*,'(a$)') 'Enter month number [1-12]: '
	|      read(*,*) imo
	|      if(imo.lt.1.or.imo.gt.12) go to 2
	|
	|    3 write(*,'(a$)') 'Enter day          [1-31]: '
	|      read(*,*) idy
	|      if(idy.lt.1.or.idy.gt.31) go to 3
	```
	*/
	int year = get_number_from_cin("Year [1901-2099]: ", 1901, 2099);
	int month = get_number_from_cin("Month [1-12]: ", 1, 12);
	int day = get_number_from_cin("Day [1-31]: ", 1, 31);

	/*
	solid.f [LN 70–72]
	```
	|      ihr=   0
	|      imn=   0
	|      sec=0.d0                                         !*** UTC time system
	```
	is achieved by
	Datetime.hpp [LN 7–8]
	```
	|		Datetime(unsigned int year, unsigned int month=1, unsigned int day=1, unsigned int hour=0,
	|		  unsigned int minute=0, unsigned int second=0);
	```
	*/
	return Datetime(year, month, day);
}


Geolocation geolocation_from_user()
{
	/*
	solid.f [LN 42–48]
	```
	|    4 write(*,'(a$)') 'Lat. (pos N.) [- 90, +90]: '
	|      read(*,*) glad
	|      if(glad.lt.-90.d0.or.glad.gt.90.d0) go to 4
	|
	|    5 write(*,'(a$)') 'Lon. (pos E.) [-360,+360]: '
	|      read(*,*) glod
	|      if(glod.lt.-360.d0.or.glod.gt.360.d0) go to 5
	```
	*/
	double latitude_degrees = get_number_from_cin("Latitude [-90, +90] °N: ", -90.0, 90.0);
	double longitude_degrees = get_number_from_cin("Longitude [-180, +180] °E: ", -180.0, 180.0);

	/*
	Convert longitude to a double in range [0.0, 360.0)
	solid.f [LN 52–53]
	```
	|      if(glod.lt.  0.d0) glod=glod+360.d0
	|      if(glod.ge.360.d0) glod=glod-360.d0
	```
	*/
	longitude_degrees += (180.0 * (longitude_degrees < 0.0)) + (-180.0 * (longitude_degrees >= 180.0));

	return Geolocation(latitude_degrees, longitude_degrees);
}


int main()
/*
solid.f [LN 1...612]
```
|	      program solid
⋮
|      implicit double precision(a-h,o-z)
|      dimension rsun(3),rmoon(3),etide(3),xsta(3)
|      logical lflag                    !*** leap second table limit flag
|      common/stuff/rad,pi,pi2
|      common/comgrs/a,e2
|
|      write(*,*) 'program solid -- UTC version -- 2018jun01'
```
*/
{
/*
solid.f [LN 42...61]
```
|    4 write(*,'(a$)') 'Lat. (pos N.) [- 90, +90]: '
⋮
|      xsta(3)=z0
```
*/
	Geolocation location = geolocation_from_user();

	/*
	solid.f [LN 30...40,70...75]
	```
	|    1 write(*,'(a$)') 'Enter year    [1901-2099]: '
	⋮
	|      if(idy.lt.1.or.idy.gt.31) go to 3

	|      ihr=   0
	⋮
	|      call setjd0(iyr,imo,idy)
	```
	*/
	Datetime date = date_from_user();

	/*
	solid.f [LN 73–75]
	```
	|      call civmjd(iyr,imo,idy,ihr,imn,sec,mjd,fmjd)
	|      call mjdciv(mjd,fmjd,iyr,imo,idy,ihr,imn,sec)    !*** normalize civil time
	|      call setjd0(iyr,imo,idy)
	```
	*/
	JulianDate julian_date = (JulianDate)date;  // achieves writing to `mjd`, `fmjd`
	Datetime normalized_datetime = (Datetime)julian_date;  // achieves writing to `iyr`,`imo`,`idy`,`ihr`,`imn`,`sec`
	int initial_modified_julian_date = ((JulianDate)normalized_datetime).modified_julian_date();
	// ^ achieves setting `mjd0`

	/*
	solid.f [LN 77–78]
	```
	|      tdel2=1.d0/60.d0/24.d0                           !*** 1 minute steps
	|      do iloop=0,60*24
	```
	*/
	for(int minute = 0; minute < 1440 /* minutes in a day */; minute++)
	{
		
	}
}

