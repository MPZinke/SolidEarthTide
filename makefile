CXX=g++
FLAGS=-std=c++14 -Wall
HEADER=-I./Headers/
SOURCE=./Source/*.cpp


all:
	$(CXX) $(FLAGS) $(HEADER) $(SOURCE) -o SolidEarthFlexing
	gfortran SolidFortranProject/solid.f -o solid


fortran:
	gfortran SolidFortranProject/solid.f -o solid


diff:
	diff <(./solid < TestSamples/solid/1.txt) <(./SolidEarthFlexing < TestSamples/SolidEarthFlexing/1.txt)
