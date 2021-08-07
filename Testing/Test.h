

#include <iostream>
#include <iomanip>

#define _TEST_PRCNT_ERROR_LWR -.00001
#define _TEST_PRCNT_ERROR_UPR .00001


template<class TEMP>
bool _TEST_equals(TEMP actual, TEMP expected)
{
	double percent_error = ((double)expected - (double)actual) / (double)expected;
	return _TEST_PRCNT_ERROR_LWR < percent_error && percent_error < _TEST_PRCNT_ERROR_UPR;
}


template<class TEMP>
double _TEST_percent_error(TEMP actual, TEMP expected)
{
	return ((double)expected - (double)actual) / (double)expected;
}


template<class TEMP>
void _TEST_matches(TEMP actual, TEMP expected)
{
	std::cout << std::setprecision(8) << std::fixed;
	if(_TEST_equals(actual, expected)) std::cout << "PASS\n";
	else std::cout << "FAIL: " << _TEST_percent_error(actual, expected) << std::endl;
	std::cout << "\t\tActual:\t" << actual << "\t\tExpected:\t" << expected << std::endl;
}


template<class TEMP>
void _TEST_matches(TEMP actual1, TEMP actual2, TEMP expected1, TEMP expected2)
{
	std::cout << std::setprecision(8) << std::fixed;
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2)) std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl;
}


template<class TEMP>
void _TEST_matches(TEMP actual1, TEMP actual2, TEMP actual3, TEMP expected1, TEMP expected2,
TEMP expected3)
{
	std::cout << std::setprecision(8) << std::fixed;
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2) 
	&& _TEST_equals(actual3, expected3)) 
		std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << "\t\t"
					<< _TEST_percent_error(actual3, expected3) << std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl
				<< "\t\tActual3:\t" << actual3 << "\t\tExpected3:\t" << expected3 << std::endl;
}


template<class TEMP>
void _TEST_matches(TEMP actual1, TEMP actual2, TEMP actual3, TEMP actual4, TEMP expected1, 
TEMP expected2, TEMP expected3, TEMP expected4)
{
	std::cout << std::setprecision(8) << std::fixed;
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2) 
	&& _TEST_equals(actual3, expected3) && _TEST_equals(actual4, expected4))
		std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << "\t\t"
					<< _TEST_percent_error(actual3, expected3) << "\t\t"
					<< _TEST_percent_error(actual4, expected4) <<  std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl
				<< "\t\tActual3:\t" << actual3 << "\t\tExpected3:\t" << expected3 << std::endl
				<< "\t\tActual4:\t" << actual4 << "\t\tExpected4:\t" << expected4 << std::endl;
}


template<class TEMP>
void _TEST_matches(TEMP actual1, TEMP actual2, TEMP actual3, TEMP actual4, TEMP actual5, 
TEMP expected1, TEMP expected2, TEMP expected3, TEMP expected4, TEMP expected5)
{
	std::cout << std::setprecision(8) << std::fixed;
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2) 
	&& _TEST_equals(actual3, expected3) && _TEST_equals(actual4, expected4)
	&& _TEST_equals(actual5, expected5))
		std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << "\t\t"
					<< _TEST_percent_error(actual3, expected3) << "\t\t"
					<< _TEST_percent_error(actual4, expected4) << "\t\t"
					<< _TEST_percent_error(actual5, expected5) << std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl
				<< "\t\tActual3:\t" << actual3 << "\t\tExpected3:\t" << expected3 << std::endl
				<< "\t\tActual4:\t" << actual4 << "\t\tExpected4:\t" << expected4 << std::endl
				<< "\t\tActual5:\t" << actual5 << "\t\tExpected5:\t" << expected5 << std::endl;
}


// ——————————————————— WITH MESSAGE ———————————————————

template<class TEMP>
void _TEST_matches(std::string string, TEMP actual, TEMP expected)
{
	std::cout << string << "\t";
	std::cout << std::setprecision(8) << std::fixed;
	if(_TEST_equals(actual, expected)) std::cout << "PASS\n";
	else std::cout << "FAIL: " << _TEST_percent_error(actual, expected) << std::endl;
	std::cout << "\t\tActual:\t" << actual << "\t\tExpected:\t" << expected << std::endl;
}


template<class TEMP>
void _TEST_matches(std::string string, TEMP actual1, TEMP actual2, TEMP expected1, TEMP expected2)
{
	std::cout << std::setprecision(8) << std::fixed;
	std::cout << string << "\t";
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2)) std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl;
}


template<class TEMP>
void _TEST_matches(std::string string, TEMP actual1, TEMP actual2, TEMP actual3, TEMP expected1, 
TEMP expected2, TEMP expected3)
{
	std::cout << std::setprecision(8) << std::fixed;
	std::cout << string << "\t";
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2)
	&& _TEST_equals(actual3, expected3))
		std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << "\t\t"
					<< _TEST_percent_error(actual3, expected3) << std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl
				<< "\t\tActual3:\t" << actual3 << "\t\tExpected3:\t" << expected3 << std::endl;
}


template<class TEMP>
void _TEST_matches(std::string string, TEMP actual1, TEMP actual2, TEMP actual3, TEMP actual4, 
TEMP expected1, TEMP expected2, TEMP expected3, TEMP expected4)
{
	std::cout << std::setprecision(8) << std::fixed;
	std::cout << string << "\t";
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2) 
	&& _TEST_equals(actual3, expected3) && _TEST_equals(actual4, expected4))
		std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << "\t\t"
					<< _TEST_percent_error(actual3, expected3) << "\t\t"
					<< _TEST_percent_error(actual4, expected4) <<  std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl
				<< "\t\tActual3:\t" << actual3 << "\t\tExpected3:\t" << expected3 << std::endl
				<< "\t\tActual4:\t" << actual4 << "\t\tExpected4:\t" << expected4 << std::endl;
}


template<class TEMP>
void _TEST_matches(std::string string, TEMP actual1, TEMP actual2, TEMP actual3, TEMP actual4, 
TEMP actual5, TEMP expected1, TEMP expected2, TEMP expected3, TEMP expected4, TEMP expected5)
{
	std::cout << std::setprecision(8) << std::fixed;
	std::cout << string << "\t";
	if(_TEST_equals(actual1, expected1) && _TEST_equals(actual2, expected2) 
	&& _TEST_equals(actual3, expected3) && _TEST_equals(actual4, expected4)
	&& _TEST_equals(actual5, expected5))
		std::cout << "PASS\n";
	else std::cout	<< "FAIL: " << _TEST_percent_error(actual1 , expected1) << "\t\t"
					<< _TEST_percent_error(actual2, expected2) << "\t\t"
					<< _TEST_percent_error(actual3, expected3) << "\t\t"
					<< _TEST_percent_error(actual4, expected4) << "\t\t"
					<< _TEST_percent_error(actual5, expected5) << std::endl;
	std::cout	<< "\t\tActual1:\t" << actual1 << "\t\tExpected1:\t" << expected1 << std::endl
				<< "\t\tActual2:\t" << actual2 << "\t\tExpected2:\t" << expected2 << std::endl
				<< "\t\tActual3:\t" << actual3 << "\t\tExpected3:\t" << expected3 << std::endl
				<< "\t\tActual4:\t" << actual4 << "\t\tExpected4:\t" << expected4 << std::endl
				<< "\t\tActual5:\t" << actual5 << "\t\tExpected5:\t" << expected5 << std::endl;
}