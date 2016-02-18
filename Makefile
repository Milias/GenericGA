CXXFLAGS += -std=c++14 -pedantic -Wall -O3
LDFLAGS += -I../Shared -Iheader
LIBFLAGS = -shared -fPIC
TARGET = bin/ga
LIB_TARGET = bin/libga.so

CPP = $(wildcard *.cpp) $(wildcard ../json/*.cpp)
OBJ = $(CPP:.cpp=.o)

all : $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ) -o $(TARGET)

library : $(filter-out main.cpp, $(OBJ))
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBFLAGS) $(filter-out main.cpp, $(OBJ)) -o $(LIB_TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $(CPP) $(LDFLAGS)

.PHONY = clean
clean :
	rm -f $(TARGET)
	rm -f *.o
