

#pragma once


#include <cmath>


template<class T>
class Coordinate
{
	public:
		enum
		{
			X,
			Y,
			Z
		};

		Coordinate(T x, T y, T z)
		: x{x}, y{y}, z{z}
		{}

		Coordinate<T> rotate3(double theta_radians)
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

	private:
		T x;
		T y;
		T z;
};
