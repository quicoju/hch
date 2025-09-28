#!/usr/bin/env bash

TEST_CMD="./hch"

prepare_test() {
    if [[ x$BACKEND == "xsqlite" ]]; then
        local db="db/integration_test.db"
        [[ ! -f $db ]] &&  sqlite3 $db ".read db/schema.sql"
        sqlite3 $db "DELETE FROM reservations;"
        TEST_CMD="./hch --db=$db"
    fi
}

run_test() {
    local test_name="$1"
    local input="$2"
    local expected="$3"

    echo -n "Test $test_name ..."
    prepare_test
    local got=$(echo -e "$input" | $TEST_CMD 2>&1)

    if echo "$got" | tr '\n' '|' | grep -q -- "$expected"; then
        echo "ok"
        return 0
    else
        echo "FAIL"
        echo "* Expected: $expected"
        echo "* Got:"
        echo "$got"
        exit 1
    fi
}

echo "The backend is *$BACKEND*"

run_test "Initial prompt" \
"quit" \
"(hch)"

run_test "Room context prompt" \
"set-room room-101
 quit" \
"(hch room-101)"

run_test "Unset room prompt" \
"set-room room-101
 unset-room
 quit" \
"(hch) "

run_test "Reserve with context" \
"set-room room-101
 reserve 2024-12-01
 quit" \
"Room reserved successfully"

run_test "List reservations shows period" \
"set-room room-101
 reserve 2024-12-01+3d
 list-reservations
 quit" \
"- \[2024-Dec-01/2024-Dec-03\]"

run_test "Cancel reservation" \
"set-room room-101
 reserve 2024-12-01+3d
 reserve 2025-01-05+5d
 cancel-reservation 2024-12-02
 list-reservations
 quit" \
"Reservation cancelled.\|.* \
 - \[2025-Jan-05/2025-Jan-09\]"

