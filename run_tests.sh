#!/usr/bin/env bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

TEST_EXE_PATH=/tmp/test
TEST_DIR=tests

run_test() {
    clang -fcolor-diagnostics -std=c23 $1 -o $TEST_EXE_PATH
    $TEST_EXE_PATH && printf "test $1 ${GREEN}OK${NC}\n" || printf "test $1 ${RED}ERROR${NC}\n"
}

for test_source_file in $(find $TEST_DIR -name "*.c")
do
    run_test $test_source_file &
done

wait
