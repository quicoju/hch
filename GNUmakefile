CXX = c++

CXXFLAGS = -Wall -g -std=c++23 -fPIC\
	-I/usr/local/include \
	-I./src \
    -I./src/views \
	-I./t/backend

LDFLAGS = -L/usr/lib -L/usr/local/lib -L. \
	-lhch \
	-lreadline \
	-lyaml-cpp

# Possible backends are:
# - memory
# - sqlite
BACKEND ?= memory
BACKEND_DIR = src/backend/$(BACKEND)
CXXFLAGS += -I$(BACKEND_DIR)

# Backend-specific flags
ifeq ($(BACKEND),sqlite)
CXXFLAGS += -DUSE_$(BACKEND)
LDFLAGS += -lsqlite3
endif

# Possible log formatters:
# - fmt
# - std (fallback)
LOG_FMT ?= std

# Log formatter
ifeq ($(LOG_FMT),fmt)
CXXFLAGS += -DSPDLOG_FMT_EXTERNAL
LDFLAGS += -lfmt
else
CXXFLAGS += -DSPDLOG_USE_STD_FORMAT
endif

# Possible GUI backends
# - Qt
GUI ?= 0

ifeq ($(GUI), Qt)
QT_CXXFLAGS := $(shell pkg-config --cflags Qt6Widgets Qt6Core)
QT_LDFLAGS := $(shell pkg-config --libs Qt6Widgets Qt6Core)
CXXFLAGS += $(QT_CXXFLAGS)
LDFLAGS += $(QT_LDFLAGS)
endif


OBJS = Guest.o Hotel.o RateCalculator.o Reservation.o Room.o Room_common.o

VPATH = src src/backend src/hch $(BACKEND_DIR) t

%.o: %.cc src/concepts.hh GNUmakefile
	$(CXX) $(CXXFLAGS) -c $<

libhch.a: $(OBJS)
	ar rcs $@ $^

main.o: src/hch/*.hh src/views/*.hh
hch: libhch.a main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

t.o: src/*.hh t/backend/*hh

# The rule to build the test executable
HotelTests: libhch.a t.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -lCatch2Main -lCatch2

db/hotel.db: db/schema.sql
	sqlite3 db/hotel.db ".read $^"

# The bindings and test_py targets are included
# from the relevant makefile.
# MAKECMDGOALS contains all the goals selected
# by the user in the command line
ifneq (,$(filter bindings test_py,$(MAKECMDGOALS)))
include src/bindings/Makefile.inc
endif

# A clean target to remove the built files
.PHONY: clean cleandb

test: HotelTests hch
	@echo
	@echo "UNIT TESTS"
	@echo "----------"
	./HotelTests -a --colour-mode=none
	@echo "INTEGRATION TESTS"
	@echo "-----------------"
	bash t/t_hch.sh

database: cleandb db/hotel.db

mockhotel: database
	sqlite3 db/hotel.db ".read db/mockhotel.sql"

cleandb:
	rm -f db/hotel.db 2>/dev/null

clean:
	rm -f *.o HotelTests hch db/*.db *.a *.so

help:
	@echo "Available backends: memory, sqlite"
	@echo "Usage: make BACKEND=memory (default)"
	@echo "       make BACKEND=sqlite"
