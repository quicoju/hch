#!/usr/bin/env bash

run_test() {
    local test_name="$1"
    local input="$2"
    local expected="$3"

    echo -n "Test $test_name ..."
    local got=$(echo -e "$input" | ./hch 2>&1)

    if echo "$got" | grep -q -- "$expected"; then
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
