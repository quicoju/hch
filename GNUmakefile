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

VPATH = src src/backend src/hch $(BACKEND_DIR) t

# Backend-specific flags
ifeq ($(BACKEND),sqlite)
LDFLAGS += -lsqlite3
CXXFLAGS += -DUSE_$(BACKEND)
endif

OBJS = Hotel.o RateCalculator.o Room.o Room_common.o

%.o: %.cc src/concepts.hh GNUmakefile
	$(CXX) $(CXXFLAGS) -c $<

main.o: src/hch/*.hh
hch: $(OBJS) main.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

t.o: src/*.hh t/backend/*hh

# The rule to build the test executable
HotelTests: $(OBJS) t.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lCatch2 -lCatch2Main -o $@ $^

db/hotel.db: db/schema.sql
	sqlite3 db/hotel.db ".read $^"

# A clean target to remove the built files
.PHONY: clean

test: HotelTests hch
	@echo
	@echo "UNIT TESTS"
	@echo "----------"
	./HotelTests -a --colour-mode=none
	@echo "INTEGRATION TESTS"
	@echo "-----------------"
	bash t/t_hch.bash

database: db/hotel.db

clean:
	rm -f *.o HotelTests hch db/*.db

help:
	@echo "Available backends: memory, sqlite"
	@echo "Usage: make BACKEND=memory (default)"
	@echo "       make BACKEND=sqlite"
