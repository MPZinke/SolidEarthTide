

namespace Trig
{
	/*
	solid.f [LN 16–18]
	```
	      pi=4.d0*datan(1.d0)
	      pi2=pi+pi
	      rad=180.d0/pi
	```
	*/
	const double PI = 3.14159265358979;
	const double DOUBLE_PI = PI + PI;
	const double RADIANS = 57.2957795130824;  // this too
	const double TO_RADIANS = 0.0174532925199433;  // used with sine & cosine: http://www.cplusplus.com/reference/cmath/cos/
}


enum
{
	X,
	Y,
	Z
};
