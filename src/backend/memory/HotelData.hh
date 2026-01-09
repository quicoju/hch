#pragma once

#include <chrono>
#include <list>
#include <set>
#include <vector>

using string_view = std::string_view;

using Amenity = std::string;
using Amenities = std::set<Amenity>;
static inline Amenity AirConditioning_ = "AirConditioning";
static inline Amenity Balcony_ = "Balcony";
static inline Amenity Wifi_ = "Wifi";
static inline Amenity MiniBar_ = "MiniBar";


/* Rate information
 * ================
 * See the memory/RateCalculator.cc for a description of how
 * the rate types are used.
 */
namespace Rate {
  enum class Type {
    Base,
    Capacity,
    Amenity
  };

  struct Entry {
    Type type;
    std::string key;
    double value;
  };
  using Table = std::vector<Entry>;
}

/* Room information
 * ================
 */
struct RoomData {
  std::string id;
  size_t capacity;
  Amenities amenities;
};
using RoomsData = std::vector<RoomData>;

/* Guest information
 * =================
 */
struct GuestData{
  std::string id;
};
using GuestsData = std::vector<GuestData>;

/* Reservation information
 * =======================
 */
using YMD = std::chrono::year_month_day;
using DateTime = std::chrono::system_clock::time_point;
using Days = std::chrono::days;

struct ReservationData{
  ReservationData(
   string_view id, string_view g, string_view r, YMD d, Days days, string_view n="")
    : id{id}, guest_id{g}, room_id{r}, start{d}, days{days}, notes{n}
  {}
  std::string id;       // client facing reservation id
  std::string guest_id;
  std::string room_id;
  YMD start;
  Days days;
  std::string notes;
  std::optional<DateTime> checkin_at = std::nullopt;
  std::optional<DateTime> checkout_at = std::nullopt;
};
using ReservationsData = std::list<ReservationData>;

struct HotelData {
  HotelData(string_view _="")
    : rooms{}
    , reservations{}
    , rates{}
    , guests{} {
  // This constructor and data were made to feed
  // hch's src default hotel, it takes a string
  // as an argument to mirror other backends that may
  // require a connection string.
    rooms = {
      {"101", 1, {Wifi_}},
      {"102", 1, {Wifi_}},
      {"103", 3, {Balcony_, Wifi_}},
      {"201", 1, {Balcony_, AirConditioning_}},
      {"202", 1, {Wifi_}},
      {"203", 1, {MiniBar_}},
      {"301", 1, {}},
      {"302", 3, {}},
    };

    guests = {
      {"juan.camaney@aol.com"}
    };

    rates = {
      {Rate::Type::Base,     ""       , 58.99}, // default nightly rate
      {Rate::Type::Base,     "101"    ,100.99}, // premium room rate
      {Rate::Type::Capacity, ""       , 0.20},  // percent surcharge per extra bed
      {Rate::Type::Amenity,  "Wifi"   ,  5.00}, // per night
      {Rate::Type::Amenity,  "Balcony", 15.00}, // per night
    };
  }

  // Properties
  RoomsData rooms;
  ReservationsData reservations;
  Rate::Table rates;
  GuestsData guests;
};
