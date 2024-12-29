CXXFLAGS = -Wall -g -std=c++2a -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lCatch2 -lCatch2Main -lboost_date_time

HotelTests: Hotel.o Room.o main.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) *.o main.cc -o HotelTests

clean:
	rm -v HotelTests *.o

.PHONY : clean
