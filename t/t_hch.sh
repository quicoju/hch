#!/usr/bin/env sh

TEST_CMD="./hch --repl"
db="db/integration_test.db"

cleanup() {
    if [[ x$BACKEND == "xsqlite" ]]; then
        rm $db 2>/dev/null
        sqlite3 $db ".read db/schema.sql"
        sqlite3 $db ".read db/mockhotel.sql"
    fi
}

prepare_test() {
    if [[ x$BACKEND == "xsqlite" ]]; then
        sqlite3 $db "DELETE FROM reservations;"
        TEST_CMD="./hch --repl --db=$db"
    fi
}

run_test() {
    local test_name="$1"; shift
    local input="$1"; shift
    local expected="$@"
    local expected_=$(echo -n "$@" | tr '\n' '@')

    prepare_test
    local got=$(echo -e "$input" | $TEST_CMD 2>&1)
    local got_=$(echo -n "$got" | tr '\n' '@')

    if echo "$got_" | grep -q -- "$expected_"; then
        echo "ok - $test_name"
        return 0
    else
        echo "not ok - $test_name"
        echo
        echo "* EXPECTED:"
        echo "$expected"
        echo
        echo "* GOT:"
        echo "$got"
        exit 1
    fi
}

echo "The backend is *${BACKEND:-memory}*"
cleanup

run_test "Initial prompt" \
"quit" \
"hch>"

run_test "Room context prompt" \
"set-room 101
 quit" \
"hch\[room 101\]>"

run_test "Unset room prompt" \
"set-room 101
 unset-room
 quit" \
"hch>"

run_test "Reserve with context" \
"set-room 101
 reserve --guest=juan.camaney@aol.com --during=2024-12-01
 quit" \
"---
Reservation: W-0001"

run_test "List reservations shows period" \
"set-room 101
 reserve --guest=juan.camaney@aol.com --during=2024-12-01+3d
 list-reservations
 quit" \
'W-0001: "\[2024-Dec-01/2024-Dec-03\]"'

run_test "List reservations by guest" \
"reserve --guest=juan.camaney@aol.com --room=101 --during=2024-12-01+3d
 reserve --guest=juan.camaney@aol.com --room=102 --during=2024-12-01+3d
 list-reservations --guest=juan.camaney@aol.com
 quit" \
'---
W-0001: "\[2024-Dec-01/2024-Dec-03\]"
W-0002: "\[2024-Dec-01/2024-Dec-03\]"'

run_test "List reservations by starting date" \
"reserve --guest=juan.camaney@aol.com --room=101 --during=2024-12-01+3d
 reserve --guest=juan.camaney@aol.com --room=102 --during=2024-12-01+3d
 reserve --guest=juan.camaney@aol.com --room=102 --during=2024-11-01+5d
 list-reservations --starting-on=2024-12-01
 quit" \
'---
W-0001: "\[2024-Dec-01/2024-Dec-03\]"
W-0002: "\[2024-Dec-01/2024-Dec-03\]"'

run_test "List reservations by ending date" \
"reserve --guest=juan.camaney@aol.com --room=101 --during=2024-12-01+3d
 reserve --guest=juan.camaney@aol.com --room=102 --during=2024-11-01+5d
 list-reservations --ending-on=2024-11-06
 quit" \
'---
W-0002: "\[2024-Nov-01/2024-Nov-05\]"'

run_test "List amenities" \
"set-room 103
 list-amenities
 quit" \
"---
- Balcony
- Wifi"

run_test "Cancel reservation" \
"set-room 101
 reserve --guest=juan.camaney@aol.com --during=2024-12-01+3d
 reserve --guest=juan.camaney@aol.com --during=2025-01-05+5d
 cancel-reservation --id=W-0001
 list-reservations
 quit" \
'Reservation cancelled
.*---
W-0002: "\[2025-Jan-05/2025-Jan-09\]"'

run_test "Check-in/out reservation" \
"set-room 101
 reserve --guest=juan.camaney@aol.com --during=2024-12-01+3d
 checkin --id=W-0001
 checkout --id=W-0001
 list-reservations
 quit" \
'Reservation checked-in
.*---
Reservation checked-out
.*---
W-0001: "\[2024-Dec-01/2024-Dec-03\]"'

run_test "show-reservation" \
"reserve --guest=juan.camaney@aol.com --room=101
 show-reservation --id=W-0001
 quit" \
"---
id: W-0001
guest: juan.camaney@aol.com
room: 101
checkin_at: ~
checkout_at: ~"

run_test "show-reservation-notes" \
"reserve --guest=juan.camaney@aol.com --room=101 --during=2025-11-28+2d
 show-reservation-notes --id=W-0001
 quit" \
"---
Rates:
  date: 2025-11-28
  days: 2
  total: 211.98
  details:
    Base: 201.98
    Capacity: 0
    Wifi: 10
"
run_test "add-reservation-notes" \
"reserve --guest=juan.camaney@aol.com --room=101 --during=2025-11-28+2d
 patch-reservation-notes --id=W-0001 --title=Discount --content='10% promo'
 show-reservation-notes --id=W-0001
 quit" \
"---
Rates:
  date: 2025-11-28
  days: 2
  total: 211.98
  details:
    Base: 201.98
    Capacity: 0
    Wifi: 10
Discount: 10% promo
"

run_test "list rate" \
"set-room 101
 list-rate --during=2025-11-02+2d
 quit" \
"---
date: 2025-11-02
days: 2
total: 211.98
details:
  Base: 201.98
  Capacity: 0
  Wifi: 10"

run_test "record-guest" \
"record-guest --email=john.bedney@yahoo.com" \
"--
Guest: john.bedney@yahoo.com"
