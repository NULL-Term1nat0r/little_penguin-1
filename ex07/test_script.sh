#!/bin/bash

set -u

DEBUGFS="/sys/kernel/debug"
DIR="$DEBUGFS/fortytwo"
ID="$DIR/id"
JIFFIES="$DIR/jiffies"
FOO="$DIR/foo"

PASS=0
FAIL=0

pass()
{
	echo "[PASS] $1"
	PASS=$((PASS + 1))
}

fail()
{
	echo "[FAIL] $1"
	FAIL=$((FAIL + 1))
}

echo "========================================"
echo " Assignment 07 - debugfs test"
echo "========================================"
echo

# ------------------------------------------------------------
# 1. Check debugfs
# ------------------------------------------------------------

echo "[1] Checking debugfs..."

if mountpoint -q "$DEBUGFS"; then
	pass "debugfs is mounted at $DEBUGFS"
else
	fail "debugfs is not mounted at $DEBUGFS"
	echo "Run:"
	echo "  mount -t debugfs none $DEBUGFS"
	exit 1
fi

# ------------------------------------------------------------
# 2. Check fortytwo directory
# ------------------------------------------------------------

echo
echo "[2] Checking fortytwo directory..."

if [ -d "$DIR" ]; then
	pass "fortytwo directory exists"
else
	fail "fortytwo directory does not exist"
	exit 1
fi

# ------------------------------------------------------------
# 3. Check files
# ------------------------------------------------------------

echo
echo "[3] Checking debugfs files..."

for file in "$ID" "$JIFFIES" "$FOO"
do
	if [ -e "$file" ]; then
		pass "$(basename "$file") exists"
	else
		fail "$(basename "$file") does not exist"
	fi
done

# ------------------------------------------------------------
# 4. Show permissions
# ------------------------------------------------------------

echo
echo "[4] File permissions:"
ls -ld "$DIR"
ls -l "$ID" "$JIFFIES" "$FOO"

# ------------------------------------------------------------
# 5. Test id read
# ------------------------------------------------------------

echo
echo "[5] Testing id read..."

ID_VALUE=$(cat "$ID")

echo "id returned: '$ID_VALUE'"

if [ "$ID_VALUE" = "estruckm" ]; then
	pass "id returns correct student login"
else
	fail "id returned '$ID_VALUE' instead of 'estruckm'"
fi

# ------------------------------------------------------------
# 6. Test id write
# ------------------------------------------------------------

echo
echo "[6] Testing id write..."

if echo -n "estruckm" > "$ID"; then
	pass "id accepts student login"
else
	fail "id rejected student login"
fi

# ------------------------------------------------------------
# 7. Test jiffies
# ------------------------------------------------------------

echo
echo "[7] Testing jiffies..."

JIFFIES1=$(cat "$JIFFIES")

echo "First jiffies value: $JIFFIES1"

if [[ "$JIFFIES1" =~ ^[0-9]+$ ]]; then
	pass "jiffies returns a numeric value"
else
	fail "jiffies did not return a numeric value"
fi

sleep 1

JIFFIES2=$(cat "$JIFFIES")

echo "Second jiffies value: $JIFFIES2"

if [[ "$JIFFIES2" =~ ^[0-9]+$ ]]; then
	pass "jiffies remains readable"
else
	fail "second jiffies value is invalid"
fi

if [ "$JIFFIES2" -gt "$JIFFIES1" ]; then
	pass "jiffies increased"
else
	fail "jiffies did not increase"
fi

# ------------------------------------------------------------
# 8. Test jiffies is read-only
# ------------------------------------------------------------

echo
echo "[8] Testing jiffies write protection..."

if echo -n "123" > "$JIFFIES" 2>/dev/null; then
	fail "jiffies is writable"
else
	pass "jiffies rejects writes"
fi

# ------------------------------------------------------------
# 9. Test foo write
# ------------------------------------------------------------

echo
echo "[9] Testing foo write..."

TEST_DATA="Hello from Assignment 07"

if echo -n "$TEST_DATA" > "$FOO"; then
	pass "foo accepts data"
else
	fail "foo rejected data"
fi

# ------------------------------------------------------------
# 10. Test foo read
# ------------------------------------------------------------

echo
echo "[10] Testing foo read..."

FOO_VALUE=$(cat "$FOO")

echo "foo returned: '$FOO_VALUE'"

if [ "$FOO_VALUE" = "$TEST_DATA" ]; then
	pass "foo returned the stored data"
else
	fail "foo returned '$FOO_VALUE' instead of '$TEST_DATA'"
fi

# ------------------------------------------------------------
# 11. Test foo overwrite
# ------------------------------------------------------------

echo
echo "[11] Testing foo overwrite..."

TEST_DATA2="Second test string"

echo -n "$TEST_DATA2" > "$FOO"

FOO_VALUE2=$(cat "$FOO")

echo "foo returned: '$FOO_VALUE2'"

if [ "$FOO_VALUE2" = "$TEST_DATA2" ]; then
	pass "foo data can be overwritten"
else
	fail "foo did not return the new data"
fi

# ------------------------------------------------------------
# 12. Test foo one-page limit
# ------------------------------------------------------------

echo
echo "[12] Testing foo size limit..."

PAGE_SIZE=$(getconf PAGE_SIZE)

echo "System page size: $PAGE_SIZE bytes"

TEST_FILE=$(mktemp)

# Generate exactly one page of data
dd if=/dev/zero of="$TEST_FILE" bs="$PAGE_SIZE" count=1 \
	2>/dev/null

if cat "$TEST_FILE" > "$FOO" 2>/dev/null; then
	pass "foo accepts one page of data"
else
	fail "foo rejected one page of data"
fi

rm -f "$TEST_FILE"

# ------------------------------------------------------------
# 13. Check current foo content
# ------------------------------------------------------------

echo
echo "[13] Reading foo after page-sized write..."

FOO_SIZE=$(wc -c < "$FOO")

echo "foo contains $FOO_SIZE bytes"

if [ "$FOO_SIZE" -le "$PAGE_SIZE" ]; then
	pass "foo contains no more than one page"
else
	fail "foo contains more than one page"
fi

# ------------------------------------------------------------
# 14. Check module information
# ------------------------------------------------------------

echo
echo "[14] Module information..."

MODULE_NAME="debugfs"

if lsmod | grep -q "^$MODULE_NAME"; then
	pass "$MODULE_NAME module is loaded"
else
	echo "[INFO] Could not find module named '$MODULE_NAME'"
	echo "       Check lsmod for your actual module name."
fi

# ------------------------------------------------------------
# Summary
# ------------------------------------------------------------

echo
echo "========================================"
echo " Test summary"
echo "========================================"
echo "Passed: $PASS"
echo "Failed: $FAIL"
echo

if [ "$FAIL" -eq 0 ]; then
	echo "ALL TESTS PASSED"
	exit 0
else
	echo "SOME TESTS FAILED"
	exit 1
fi