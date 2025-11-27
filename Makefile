CXX = c++

CXXFLAGS = -Wall -std=c++23 \
	-I/usr/local/include \
	-I./src \
    -I./src/views \
	-I./t/backend

LDFLAGS = -L/usr/local/lib \
	-lboost_date_time \
	-lreadline \
    -lyaml-cpp

# Possible backends are:
# - memory
# - sqlite
BACKEND ?= memory
BACKEND_DIR = src/backend/$(BACKEND)
CXXFLAGS += -I$(BACKEND_DIR)

# Backend-specific flags
.if $(BACKEND) == "sqlite"
LDFLAGS += -lsqlite3
CXXFLAGS += -DUSE_$(BACKEND)
.endif

OBJS = Guest.o Hotel.o RateCalculator.o Reservation.o Room.o Room_common.o

.SUFFIXES: .o .cc .hh
.PATH.cc: src src/backend src/hch $(BACKEND_DIR) t

.cc.o: src/concepts.hh Makefile
	$(CXX) $(CXXFLAGS) -c $<

main.o: src/hch/*.hh src/views/*.hh
hch: $(OBJS) main.o
	$(CXX) $(CXXFLAGS) -o $@ $> $(LDFLAGS)

t.o: src/*.hh t/backend/*hh

# The rule to build the test executable
HotelTests: $(OBJS) t.o
	$(CXX) $(CXXFLAGS) -o $@ $> $(LDFLAGS) -lCatch2Main -lCatch2

db/hotel.db: db/schema.sql
	sqlite3 db/hotel.db ".read $>"

# A clean target to remove the built files
.PHONY: clean cleandb

test: HotelTests hch
	@echo
	@echo "UNIT TESTS"
	@echo "----------"
	./HotelTests -a --colour-mode=none
	@echo "INTEGRATION TESTS"
	@echo "-----------------"
	sh t/t_hch.sh

database: cleandb db/hotel.db

mockhotel: database
	sqlite3 db/hotel.db ".read db/mockhotel.sql"

cleandb:
	rm -f db/hotel.db 2>/dev/null

clean:
	rm -f *.o HotelTests hch db/*.db

help:
	@echo "Available backends: memory, sqlite"
	@echo "Usage: make BACKEND=memory (default)"
	@echo "       make BACKEND=sqlite"
