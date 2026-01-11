CXX = c++

CXXFLAGS = -g2 -O0 -Wall -std=c++23 -fPIC \
	-I/usr/local/include \
	-I./src \
    -I./src/views \
	-I./t/backend

LDFLAGS = -L/usr/local/lib -L. \
	-lreadline \
    -lyaml-cpp \
	-lhch

# Possible backends are:
# - memory
# - sqlite
BACKEND ?= memory
BACKEND_DIR = src/backend/$(BACKEND)
CXXFLAGS += -I$(BACKEND_DIR)

# Backend-specific flags
.if $(BACKEND) == "sqlite"
CXXFLAGS += -DUSE_$(BACKEND)
LDFLAGS += -lsqlite3
.endif

# Possible log formatters:
# - fmt
# - std (fallback)
LOG_FMT ?= std

# Log formatter
.if $(LOG_FMT) == "fmt"
CXXFLAGS += -DSPDLOG_FMT_EXTERNAL
LDFLAGS += -lfmt
.else
CXXFLAGS += -DSPDLOG_USE_STD_FORMAT
.endif

OBJS = Guest.o Hotel.o RateCalculator.o Reservation.o Room.o Room_common.o

.SUFFIXES: .o .cc .hh
.PATH.cc: src src/backend src/hch src/gui $(BACKEND_DIR) t

.cc.o: src/concepts.hh Makefile
	$(CXX) $(CXXFLAGS) -c $<

libhch.a: $(OBJS)
	ar rcs $@ $>

main.o: src/hch/*.hh src/views/*.hh
hch: libhch.a main.o
	$(CXX) $(CXXFLAGS) -o $@ $> $(LDFLAGS)

t.o: src/*.hh t/backend/*hh

# The rule to build the test executable
HotelTests: libhch.a t.o
	$(CXX) $(CXXFLAGS) -o $@ $> $(LDFLAGS) -lCatch2Main -lCatch2

db/hotel.db: db/schema.sql
	sqlite3 db/hotel.db ".read $>"

# Build the GUI tests
.if make(hch-gui) || make(test_gui)
.include "src/gui/Makefile.inc"
.endif

# The bindings and test_py targets are included
# from the relevant makefile.
.if make(bindings) || make(test_py)
.include "src/bindings/Makefile.inc"
.endif

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
	rm -f *.o *Tests hch hch-gui db/*.db *.a *.so *.moc

help:
	@echo "Available backends: memory, sqlite"
	@echo "Usage: make BACKEND=memory (default)"
	@echo "       make BACKEND=sqlite"
