CXX=g++
FLAGS=-std=c++14 -Wall
HEADER=-I./Headers/
SOURCE=./Source/*.cpp


all:
	$(CXX) $(FLAGS) $(HEADER) $(SOURCE) -o SolidEarthTide


fortran:
	gfortran solid.f -o solid
