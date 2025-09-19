CXX = c++

CXXFLAGS = -Wall -g -std=c++2a \
	-I/usr/local/include -I./src

LDFLAGS = -L/usr/local/lib \
	-lboost_date_time \
	-lreadline

OBJS = Room.o Hotel.o

.SUFFIXES: .o .cc .hh
.PATH.cc: src t

.cc.o: src/concepts.hh Makefile
	$(CXX) $(CXXFLAGS) -c $<

hch: $(OBJS) hch.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $>

# The rule to build the test executable
HotelTests: $(OBJS) t.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lCatch2 -lCatch2Main -o $@ $>

db/hotel.db: db/Hotel.sql
	sqlite3 db/hotel.db ".read $>"

# A clean target to remove the built files
.PHONY: clean

test: HotelTests
	./HotelTests

database: db/hotel.db

clean:
	rm -f *.o HotelTests hch db/hotel.db
