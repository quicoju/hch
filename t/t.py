#!/usr/bin/env python3

import datetime as dt

import sys
import os

BACKEND = os.environ.get("BACKEND", "memory")
print(f"The backend is *{BACKEND}*")

# expect the ".so" module to be in the current dir
sys.path.insert(0, '.')

try:
    from hch import Hotel, Period
    print("ok - Import the hotel module")
except ImportError as e:
    print(f" ...not ok: {e}")
    sys.exit(1)

def test_bindings():
    def ok(assertion, msg):
        assert assertion, f"no ok - {msg}"
        print(f"ok - {msg}")

    def throws_ok(f, expected_error, msg):
        try:
            f()
        except BaseException as e:
            ok(str(e) == expected_error, msg)

    try:
        # Test the "concepts"
        # ===================
        date = dt.date(2024, 1, 15)
        duration = dt.timedelta(days=5)

        period = Period(date, duration)
        ok(f"{period}" == "[2024-01-15/2024-01-20]", "Create a `Period'")

        # Hotel
        # =====
        hotel = Hotel(conn_str=conn);

        email = "john.bedney@mail.com"
        ok(hotel.record_guest(email) == email, "hotel.record_guest")

        rsv_id = hotel.reserve(email, "101", date)
        ok(rsv_id == "W-0001", "hotel.reserve")

        reservations = hotel.room_reservations("101");
        ok(reservations.pop().id == "W-0001", "hotel.room_reservations")

        reservations = hotel.guest_reservations(email)
        ok(reservations.pop().id == "W-0001", "hotel.guest_reservations")

        reservations = hotel.reservations_starting_on(dt.date(2024, 1, 15))
        ok(reservations.pop().id == "W-0001", "hotel.reservations_starting_on")

        reservations = hotel.reservations_ending_on(dt.date(2024, 1, 16))
        ok(reservations[0].id == "W-0001", "hotel.reservations_ending_on")

        ok(hotel.reservation_notes(f"{reservations[0]}") ==
'''Rates:
  date: 2024-01-15
  days: 1
  total: 105.99
  details:
    Base: 100.99
    Capacity: 0.00
    Wifi: 5.00''', "hotel.reservation_notes")

        hotel.patch_reservation_notes(reservations[0].id, "Discount", "5%")
        ok(hotel.reservation_notes(f"{reservations[0]}") ==
'''Rates:
  date: 2024-01-15
  days: 1
  total: 105.99
  details:
    Base: 100.99
    Capacity: 0.00
    Wifi: 5.00
Discount: 5%''', "hotel.patch_reservation_notes")

        amenities = hotel.room_amenities("201")
        ok(amenities == {'AirConditioning', 'Balcony'}, "hotel.room_amenities")

        report = hotel.rate_report_for("101", dt.date(2025,12,12))
        ok(str(report.date) == "2025-12-12", "hotel.rate_report_for")
        ok(int(report.duration.days) == 1, "report.duration")
        ok(report.total == 105.99, "report.total")
        ok(report.details == {
            'Base': 100.99,
            'Capacity': 0.00,
            'Wifi': 5.00,
        }, "report.details")

        checkin_dt = dt.datetime.combine(dt.date(2024, 12, 15), dt.time(12, 30))
        hotel.checkin(rsv_id, checkin_dt)
        rsv = hotel.reservation(rsv_id)
        ok(f"{rsv.checkin_at}" == "2024-12-15 12:30:00", "hotel.checkin")

        checkout_dt = dt.datetime.combine(dt.date(2024, 12, 16), dt.time(11, 00))
        hotel.checkout(rsv_id, checkout_dt)
        rsv = hotel.reservation(rsv_id)
        ok(f"{rsv.checkout_at}" == "2024-12-16 11:00:00", "hotel.checkout")

        hotel.cancel(rsv_id)
        throws_ok(lambda: hotel.reservation(rsv_id),
                  f"Reservation {rsv_id} doesn't exist", "hotel.cancel")

        # Reservation
        # ===========
        rsv_id = hotel.reserve(email, "101", date)
        reservation = hotel.reservation(rsv_id)
        ok(reservation.id == rsv_id, "reservation.id")
        ok(f"{reservation}" == rsv_id, "reservation.__str__()")
        ok(str(reservation.period) == "[2024-01-15/2024-01-16]", "reservation.period")
        ok(reservation.checkin_at == None, "reservation.checkin_at")
        ok(reservation.checkout_at == None, "reservation.checkout_at")

        # XXX: This test will fail if it's executed at night when the day changes
        r = hotel.reservation(hotel.reserve(email, "102"))
        ok(r.period.start == dt.datetime.now().date(), "reservation default date")

        # "Room"
        # ======
        room = hotel.room("101")
        ok(str(room) == "101", "hotel.room")
        ok(room.name == "101", "room.name")
        ok(room.capacity == 1, "room.capacity")

    except Exception as e:
        print(f" ...not ok: {e}")
        exit(1);

if __name__ == "__main__":
    conn = ""
    if (BACKEND == 'sqlite'):
        conn = "db/bindings.db"
        os.system(f'sqlite3 {conn} ".read db/schema.sql"')
        os.system(f'sqlite3 {conn} ".read db/mockhotel.sql"')

    test_bindings()
