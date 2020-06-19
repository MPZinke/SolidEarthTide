CXX=g++
FLAGS=-std=c++14
C_FILES=Date.cpp Datetime.cpp GeoLocation.cpp SolidEarthFlexing.cpp

all:
	$(CXX) $(FLAGS) $(C_FILES) -o SolidEarthFlexing