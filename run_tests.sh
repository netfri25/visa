#!/usr/bin/env bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

TEST_DIR=tests

run_test() {
    set -e

    SOURCE=$1
    TARGET_NAME=$(basename $1 .c)
    TARGET="/tmp/$TARGET_NAME"

    clang -fcolor-diagnostics -std=c23 $SOURCE -o $TARGET

    $TARGET &&
        printf "test $TARGET_NAME ${GREEN}OK${NC}\n" ||
        printf "test $TARGET_NAME ${RED}ERROR${NC}\n"

    rm $TARGET
}

for test_source_file in $(find $TEST_DIR -name "*.c")
do
    run_test $test_source_file &
done

wait
