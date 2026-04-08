# dedicate a CPU for my program
make WITH_O3=1 TEST_AVX=1
sudo nice -n -5 taskset -c 2 ./run test_avx_o3.csv

make TEST_AVX=1
sudo nice -n -5 taskset -c 2 ./run test_avx_o2.csv

make WITH_O3=1 TEST_UNOPTIMIZED=1
sudo nice -n -5 taskset -c 2 ./run test_unoptimized_o3.csv

make TEST_UNOPTIMIZED=1
sudo nice -n -5 taskset -c 2 ./run test_unoptimized_o2.csv

make WITH_O3=1 TEST_ARRAYS=1
sudo nice -n -5 taskset -c 2 ./run test_arrays_o3.csv

make TEST_ARRAYS=1
sudo nice -n -5 taskset -c 2 ./run test_arrays_o2.csv
