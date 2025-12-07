#!/usr/bin/env python3

import sys
import os

BACKEND = os.environ.get("BACKEND", "memory")
print(f"The backend is *{BACKEND}*")

# expect the ".so" module to be in the current dir
sys.path.insert(0, '.')

try:
    import hch
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
        date = hch.Date(2024, 1, 15)
        ok(f"{date}" == "2024-01-15", "Create a `Date'")

        duration = hch.Days(5);
        ok(int(duration) == 5, "Create `Days'")

        period = hch.Period(date, duration)
        ok(f"{period}" == "[2024-Jan-15/2024-Jan-19]", "Create a `Period'")

    except Exception as e:
        print(f" ...not ok: {e}")

if __name__ == "__main__":
    test_bindings()
