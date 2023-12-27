CXX = g++
# CXXFLAGS = -std=c++11 -g -O3
CXXFLAGS = -std=c++11 -g
TARGET = Lab4
SRCDIR = src
OBJS = main.o ChannelRouting.o # Add your other object files here

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp $(SRCDIR)/ChannelRouting.h
	$(CXX) $(CXXFLAGS) -c main.cpp

ChannelRouting.o: $(SRCDIR)/ChannelRouting.cpp $(SRCDIR)/ChannelRouting.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/ChannelRouting.cpp

clean:
	rm -f $(OBJS) $(TARGET) *.out
