#pragma once

#ifdef USE_sqlite
#include "SQLite.hh"
using Backend = SQLite;
#else
#include "HotelData.hh"
using Backend = HotelData;
#endif
