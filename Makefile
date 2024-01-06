CXX = g++
# CXXFLAGS = -std=c++11 -g -O3
CXXFLAGS = -std=c++11 -g -O3
TARGET = Lab4
SRCDIR = src
OBJS = main.o Channel.o # Add your other object files here

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp $(SRCDIR)/Channel.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Channel.o: $(SRCDIR)/Channel.cpp $(SRCDIR)/Channel.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/Channel.cpp

clean:
	rm -f $(OBJS) $(TARGET)
