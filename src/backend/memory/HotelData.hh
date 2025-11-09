#pragma once

#include <list>
#include <vector>

#include "Hotel.hh"

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
// XXX: this isn't in the concepts because
// I can't think of an immediate case where
// we want to access to multiple guests at one;
// except for a generic searching of guests, but
// this is not to be implemented soon.
using Guests = std::vector<Guest>;

/* Reservation information
 * =======================
 */
struct ReservationData{
  std::string id;       // client facing reservation id
  std::string guest_id;
  std::string room_id;
  Period period;
  // TODO: add a rate snapshot that captures
  // the rates at the time of the resrvation,
  // don't want to charge a different rate
  // if rates change between the reservation
  // and the check-out date.
  std::string notes;
};
using ReservationsData = std::list<ReservationData>;

struct HotelData {
  HotelData(std::string _)
    : rooms{}
    , reservations{}
    , rates{}
    , guests{} {
  // This constructor and data were made to feed
  // hch's src default hotel, it takes a string
  // as an argument to mirror the sqlite backend
  // constructor, this helps to define a "Backend"
  // type and make it very simple to build the backend
  // specific source without using generic programming
    rooms = {
      {"101", 1, {Wifi}},
      {"102", 1, {Balcony, Wifi},},
      {"103", 1, {MiniBar, Wifi},},
      {"201", 1, {AirConditioning, Balcony},},
      {"202", 1, {Wifi},},
      {"203", 1, {MiniBar},}
    };
    rates = {
      {Rate::Type::Base,     ""       , 58.99}, // default nightly rate
      {Rate::Type::Base,     "101"    ,100.99}, // premium room rate
      {Rate::Type::Capacity, ""       , 0.20},  // percent surcharge per extra bed
      {Rate::Type::Amenity,  "Wifi"   ,  5.00}, // per night
      {Rate::Type::Amenity,  "Balcony", 15.00}, // per night
    };
  }

  // This constructor is relevant for testing (see t_memory.hh)
  HotelData(RoomsData r,
            ReservationsData rsv={},
            Rate::Table rates={},
            GuestsData guests={})
    : rooms{r}, reservations{rsv}, rates{rates}, guests{guests}
  {}

  // Properties
  RoomsData rooms;
  ReservationsData reservations;
  Rate::Table rates;
  GuestsData guests;
};
