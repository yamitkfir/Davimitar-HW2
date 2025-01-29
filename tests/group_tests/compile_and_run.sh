#!/bin/bash

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
RESET='\033[0m'

# Define the script directory
SCRIPT_DIR=$(dirname "$0")

# Function to run a test
run_test() {
    local RUN_COMMAND=$1
    local expectedName=$2

    echo "Running test >>>" ${RUN_COMMAND}
    eval ${RUN_COMMAND} | diff ${expectedName} - && echo -e "${GREEN}Test Passed.${RESET}" && return 0 || echo -e "${RED}Python TEST FAILED!!!${RESET}"
    echo "--- actual ---"
    eval ${RUN_COMMAND}
    echo "--- expected ---"
    cat ${expectedName}
    echo "________________"
}

# Navigate to the assignment directory
pushd ${SCRIPT_DIR}/325712917_326680345_325149383_assignment2/ || exit

echo "------- Setup -------"
rm -rf ./build || echo "No build to remove"
rm -f mykmeanssp.cpython*.so || echo "No so to remove"

echo "------- Compile -------"
python3 setup.py build_ext --inplace

echo "------- Test -------"
# Run pytest with elevated permissions if needed
sudo pytest || pytest

echo "------- Run -------"
run_test "python3 kmeans_pp.py 3 333 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_1.txt
run_test "python3 kmeans_pp.py 49 2 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_1__49_2_0.txt
run_test "python3 kmeans_pp.py 2 2 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_1__2_2_0.txt
run_test "python3 kmeans_pp.py 7 0 ../tests/input_2_db_1.txt ../tests/input_2_db_2.txt" ../tests/output_2.txt
run_test "python3 kmeans_pp.py 15 750 0 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt" ../tests/output_3_fixed_rounding.txt
run_test "python3 kmeans_pp.py 3 2 0 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt" ../tests/output_3__3_2_0.txt
run_test "python3 kmeans_pp.py 3 3 0 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt" ../tests/output_3__3_3_0.txt
run_test "python3 kmeans_pp.py 3 3 1.91 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt" ../tests/output_3__3_3_0.txt
run_test "python3 kmeans_pp.py 3 3 1.92 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt" ../tests/output_3__3_3_1.92.txt

run_test "python3 kmeans_pp.py 50 2 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_invalid_clusters.txt
run_test "python3 kmeans_pp.py 1 2 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_invalid_clusters.txt
run_test "python3 kmeans_pp.py -2 2 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_invalid_clusters.txt
run_test "python3 kmeans_pp.py a 2 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_invalid_clusters.txt

run_test "python3 kmeans_pp.py 2 1000 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_invalid_maxIter.txt
run_test "python3 kmeans_pp.py 2 1 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt" ../tests/output_invalid_maxIter.txt
run_test "python3 kmeans_pp.py 2 0 0