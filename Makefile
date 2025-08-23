CXX = c++

CXXFLAGS = -Wall -g -std=c++2a \
	-I/usr/local/include

LDFLAGS = -L/usr/local/lib \
	-lboost_date_time

OBJS = Room.o Hotel.o

.SUFFIXES: .o .cc .hh
.PATH.cc: src t

.cc.o: src/concepts.hh Makefile
	$(CXX) $(CXXFLAGS) -c $<

# The rule to build the test executable
HotelTests: $(OBJS) t.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lCatch2 -lCatch2Main -o $@ $>

# A clean target to remove the built files
.PHONY: clean
test: HotelTests
	./HotelTests

clean:
	rm -f *.o HotelTests
