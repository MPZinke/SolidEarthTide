

#include <iostream>


#include "Geolocation.hpp"


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


// Date date_from_user()
// {
// 	/*
// 	solid.f [LN 30–40]
// 	```
// 	|    1 write(*,'(a$)') 'Enter year    [1901-2099]: '
// 	|      read(*,*) iyr
// 	|      if(iyr.lt.1901.or.iyr.gt.2099) go to 1
// 	|
// 	|    2 write(*,'(a$)') 'Enter month number [1-12]: '
// 	|      read(*,*) imo
// 	|      if(imo.lt.1.or.imo.gt.12) go to 2
// 	|
// 	|    3 write(*,'(a$)') 'Enter day          [1-31]: '
// 	|      read(*,*) idy
// 	|      if(idy.lt.1.or.idy.gt.31) go to 3
// 	```
// 	*/
// }


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
{
	/*
	solid.f [LN 1–12]
	```
	|	      program solid
	|
	|*** driver to test solid earth tide
	|*** UTC version
	|
	|      implicit double precision(a-h,o-z)
	|      dimension rsun(3),rmoon(3),etide(3),xsta(3)
	|      logical lflag                    !*** leap second table limit flag
	|      common/stuff/rad,pi,pi2
	|      common/comgrs/a,e2
	|
	|      write(*,*) 'program solid -- UTC version -- 2018jun01'
	```
	*/
	double xyz[3];
	Geolocation location = geolocation_from_user();
	xyz << location;
}

