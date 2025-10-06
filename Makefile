CXX = c++

CXXFLAGS = -Wall -g -std=c++2a \
	-I/usr/local/include \
	-I./src \
	-I./t/backend

LDFLAGS = -L/usr/local/lib \
	-lboost_date_time \
	-lreadline

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

OBJS = Room.o Hotel.o RateCalculator.o Room_common.o

.SUFFIXES: .o .cc .hh
.PATH.cc: src src/hch src/backend $(BACKEND_DIR) t

.cc.o: src/concepts.hh Makefile
	$(CXX) $(CXXFLAGS) -c $<

hch.o: src/hch/Repl.hh
hch: $(OBJS) hch.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $>

t.o: src/RateCalculator.hh t/backend/*hh

# The rule to build the test executable
HotelTests: $(OBJS) t.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lCatch2 -lCatch2Main -o $@ $>

db/hotel.db: db/schema.sql
	sqlite3 db/hotel.db ".read $>"

# A clean target to remove the built files
.PHONY: clean

test: HotelTests hch
	@echo "\n*Unit tests*"
	./HotelTests -a --colour-mode=none
	@echo "*Integration tests*"
	bash t/t_hch.bash

database: db/hotel.db

clean:
	rm -f *.o HotelTests hch db/hotel.db

help:
	@echo "Available backends: memory, sqlite"
	@echo "Usage: make BACKEND=memory (default)"
	@echo "       make BACKEND=sqlite"
