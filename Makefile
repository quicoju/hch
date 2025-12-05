CXX = c++

CXXFLAGS = -Wall -std=c++23 -fPIC \
	-I/usr/local/include \
	-I./src \
    -I./src/views \
	-I./t/backend

LDFLAGS = -L/usr/local/lib -L. \
	-lboost_date_time \
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
LDFLAGS += -lsqlite3
CXXFLAGS += -DUSE_$(BACKEND)
.endif

OBJS = Guest.o Hotel.o RateCalculator.o Reservation.o Room.o Room_common.o

.SUFFIXES: .o .cc .hh
.PATH.cc: src src/backend src/hch $(BACKEND_DIR) t

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

# build the Python bindings (by "so" name)
# use the PHONY "bindings" target instead)
PY_SUFFIX != python3-config --extension-suffix
PY_INCLUDE != python3 -m pybind11 --includes
PY_BINDINGS = src/bindings/py_bindings.cc

hch$(PY_SUFFIX): libhch.a $(PY_BINDINGS)
	$(CXX) $(CXXFLAGS) -shared $(PY_INCLUDE) $(PY_BINDINGS) $(LDFLAGS) -L. -lhch -o $@

# A clean target to remove the built files
.PHONY: clean cleandb bindings

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

bindings: hch$(PY_SUFFIX)

clean:
	rm -f *.o HotelTests hch db/*.db *.a *so

help:
	@echo "Available backends: memory, sqlite"
	@echo "Usage: make BACKEND=memory (default)"
	@echo "       make BACKEND=sqlite"
