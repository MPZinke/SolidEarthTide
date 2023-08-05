

class Geolocation
{
	public:
		/*
		solid.f [LN 22–23]
		```
		      a=6378137.d0
		      e2=6.69438002290341574957d-03
		```
		a = EQUITORIAL_RADIUS
		e2 = E_SQR
		*/
		static const double EQUITORIAL_RADIUS;  // KM: https://www.space.com/17638-how-big-is-earth.html
		// E_SQR: geodetic ellipsoid (Moritz, 1980b)
		// E_SQR: square of the first numerical eccentricity of the ellipsoid
		static const double GEODETIC_ELLIPSOID;

		Geolocation(double latitude_degrees, double longitude_degrees);
		friend void operator<<(double cartesian_coordinates[3], Geolocation& geolocation);

	private:
		const double _latitude;  // Radians
		const double _longitude;  // Radians
};
