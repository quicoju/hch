#!/usr/bin/env python3

import sys
import os

BACKEND = os.environ.get("BACKEND", "memory")
print(f"The BACKEND is *{BACKEND}*")

# expect the ".so" module to be in the current dir
sys.path.insert(0, '.')

try:
    import hch
    print(" Import the hotel module", end="")
except ImportError as e:
    print(f" ...not ok: {e}")
    sys.exit(1)
print(" ...ok")

def test_bindings():
    try:
        # Test the "concepts"
        # ===================
        date = hch.Date(2024, 1, 15)
        print(f" Create a `Date': {date}", end="")
        print(" ...ok")
    except Exception as e:
        print(f" ...not ok: {e}")

if __name__ == "__main__":
    test_bindings()
