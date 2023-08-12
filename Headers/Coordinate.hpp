

#pragma once


#include <assert.h>
#include <cmath>
#include <iostream>


enum
{
	X,
	Y,
	Z
};


template<typename T>
class Coordinate
{
	public:
		enum
		{
			X,
			Y,
			Z
		};

		Coordinate();
		Coordinate(T x, T y, T z);
		T distance();
		Coordinate<T> rotate1(double theta_radians);
		Coordinate<T> rotate3(double theta_radians);
		T operator+(Coordinate<T>& right);
		Coordinate<T>& operator+=(Coordinate<T>& right);
		T operator*(Coordinate<T>& right);
		T operator[](unsigned int index) const;
		T& operator[](unsigned int index);

	private:
		T x;
		T y;
		T z;
};


template<typename T>
Coordinate<T>::Coordinate(T x, T y, T z)
: x{x}, y{y}, z{z}
{}


template<typename T>
Coordinate<T>::Coordinate()
: x{(T)0.0}, y{(T)0.0}, z{(T)0.0}
{}


template<typename T>
Coordinate<T> Coordinate<T>::rotate1(double theta_radians)
/*
solid.f [LN 1025–1040]
```
|      subroutine rot1(theta,x,y,z,u,v,w)
|
|*** rotate coordinate axes about 1 axis by angle of theta radians
|*** x,y,z transformed into u,v,w
|
|      implicit double precision(a-h,o-z)
|
|      s=dsin(theta)
|      c=dcos(theta)
|
|      u=x
|      v=c*y+s*z
|      w=c*z-s*y
|
|      return
|      end
```
*/
{
	double sin_theta = sin(theta_radians);  // Check if these need to be converted to Radians
	double cos_theta = cos(theta_radians);  // Check if these need to be converted to Radians

	return Coordinate<T>(x, cos_theta * y + sin_theta * z, cos_theta * z - sin_theta * y);
}


template<typename T>
Coordinate<T> Coordinate<T>::rotate3(double theta_radians)
/*
solid.f [LN 1025–1040]
```
|      subroutine rot3(theta,x,y,z,u,v,w)
|      
|*** rotate coordinate axes about 3 axis by angle of theta radians
|*** x,y,z transformed into u,v,w
|
|      implicit double precision(a-h,o-z)
|      
|      s=dsin(theta)
|      c=dcos(theta)
|            
|      u=c*x+s*y
|      v=c*y-s*x
|      w=z
|            
|      return
|      end
```
*/
{
	double sin_theta = sin(theta_radians);  // Check if these need to be converted to Radians
	double cos_theta = cos(theta_radians);  // Check if these need to be converted to Radians

	return Coordinate<T>(cos_theta * x + sin_theta * y, cos_theta * y - sin_theta * x, z);
}


template<typename T>
T Coordinate<T>::operator+(Coordinate<T>& right)
{
	return Coordinate<T>(x + right.x, y + right.y, z + right.z);
}


template<typename T>
Coordinate<T>& Coordinate<T>::operator+=(Coordinate<T>& right)
{
	x += right.x;
	y += right.y;
	z += right.z;
	return *this;
}


template<typename T>
T Coordinate<T>::operator*(Coordinate<T>& right)
/*
Dot product
*/
{
	return x * right.x + y * right.y + z * right.z;
}


template<typename T>
T Coordinate<T>::distance()
/*
solid.f [LN 693–702]
```
|      double precision function enorm8(a)
|
|*** compute euclidian norm of a vector (of length 3)
|
|      double precision a(3)
|
|      enorm8=dsqrt(a(1)*a(1) + a(2)*a(2) + a(3)*a(3))
|
|      return
|      end
```
*/
{
	return sqrt(x * x + y * y + z * z);
}


template<typename T>
T Coordinate<T>::operator[](unsigned int index) const
{
	switch(index)
	{
		case X:
			return x;
		case Y:
			return y;
		case Z:
			return z;
		default:
			throw std::runtime_error("Index must be 'X', 'Y', 'Z'");
	}
}


template<typename T>
T& Coordinate<T>::operator[](unsigned int index)
{
	switch(index)
	{
		case X:
			return x;
		case Y:
			return y;
		case Z:
			return z;
		default:
			throw std::runtime_error("Index must be 'X', 'Y', 'Z'");
	}
}
