CXX=g++
FLAGS=-std=c++14
C_FILES=Date.cpp Datetime.cpp GeoLocation.cpp SolidEarthFlexing.cpp

all:
	$(CXX) $(FLAGS) $(C_FILES) -o SolidEarthFlexing


fortran:
	gfortran solid.f -o solid


######################################### TESTING #########################################

cpp_test:
	$(CXX) $(FLAGS) $(C_FILES) -D _TESTING_ -o Testing/SolidEarthFlexing

run_cpp_test:
	Testing/SolidEarthFlexing < Testing/cpp_input.txt > Testing/cpp_output.txt


fortran_test:
	gfortran solid.f -o solid

run_fortran_test:
	./solid < fortran_input.txt > fortran_output.txt
