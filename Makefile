CXXFLAGS = -Wall -g -std=c++2a \
	-I/usr/local/include

LDFLAGS = -L/usr/local/lib \
	-lCatch2 \
	-lCatch2Main \
	-lboost_date_time

HotelTests: src/Hotel.cc src/Room.cc t/t.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $>

clean:
	rm -v HotelTests src/*.o

.PHONY : clean
