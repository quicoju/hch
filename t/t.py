#!/usr/bin/env python3

import sys
import os

BACKEND = os.environ.get("BACKEND", "memory")
print(f"The backend is *{BACKEND}*")

# expect the ".so" module to be in the current dir
sys.path.insert(0, '.')

try:
    import hch
    print("ok - Import the hotel module", end="")
except ImportError as e:
    print(f" ...not ok: {e}")
    sys.exit(1)

def test_bindings():
    msg = ""
    try:
        # Test the "concepts"
        # ===================
        msg = "Create a `Date'"
        date = hch.Date(2024, 1, 15)
        assert f"{date}" == "2024-01-15", f"not ok - {msg}"
        print(f"ok - {msg}")

        msg = "Create `Days'"
        duration = hch.Days(5);
        # TODO: there should not be need to int() it
        assert int(duration) == 5, f"not ok - {msg}"
        print(f"ok - {msg}")

        msg = "Create a `Period'"
        period = hch.Period(date, duration)
        assert f"{period}" == "[2024-Jan-15/2024-Jan-19]", f"not ok - {msg}"
        print(f"ok - {msg}")

    except Exception as e:
        print(f" ...not ok: {e}")

if __name__ == "__main__":
    test_bindings()
