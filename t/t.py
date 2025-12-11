#!/usr/bin/env python3

import sys
import os

BACKEND = os.environ.get("BACKEND", "memory")
print(f"The backend is *{BACKEND}*")

# expect the ".so" module to be in the current dir
sys.path.insert(0, '.')

try:
    from Hch import Hotel, Date, Days, Period
    print("ok - Import the hotel module")
except ImportError as e:
    print(f" ...not ok: {e}")
    sys.exit(1)

def test_bindings():
    def ok(assertion, msg):
        assert assertion, f"no ok - {msg}"
        print(f"ok - {msg}")

    try:
        # Test the "concepts"
        # ===================
        date = Date(2024, 1, 15)
        ok(f"{date}" == "2024-01-15", "Create a `Date'")

        duration = Days(5);
        ok(int(duration) == 5, "Create `Days'")

        period = Period(date, duration)
        ok(f"{period}" == "[2024-Jan-15/2024-Jan-19]", "Create a `Period'")

        # Hotel
        # =====
        hotel = Hotel(conn_str=conn);

        email = "john.bedney@mail.com"
        ok(hotel.record_guest(email) == email, "hotel.record_guest")

        rsv_id = hotel.reserve(email, "101", date)
        ok(rsv_id == "W-0001", "hotel.reserve")

        # Reservation
        # ===========
        reservation = hotel.reservation(rsv_id)
        ok(reservation.id == rsv_id, "reservation.id")
        ok(f"{reservation}" == rsv_id, "reservation.__str__()")
        ok(str(reservation.period) == "[2024-Jan-15/2024-Jan-15]", "reservation.period")
        ok(reservation.checkin_at == None, "reservation.checkin_at")
        ok(reservation.checkout_at == None, "reservation.checkout_at")


        # "Room"
        # ======

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
