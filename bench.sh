# dedicate a CPU core for my program
# nice -n -20  = run with highest priority 
# taskset -c 2 = use CPU core #2

make NAIVE=1
sudo nice -n -20 taskset -c 2 ./run data/test_unoptimized_o2.csv

make WITH_O3=1 NAIVE=1
sudo nice -n -20 taskset -c 2 ./run data/test_unoptimized_o3.csv

make ARRAYS=1
sudo nice -n -20 taskset -c 2 ./run data/test_arrays_o2.csv

make WITH_O3=1 ARRAYS=1
sudo nice -n -20 taskset -c 2 ./run data/test_arrays_o3.csv

make AVX=1
sudo nice -n -20 taskset -c 2 ./run data/test_avx_o2.csv

make WITH_O3=1 AVX=1
sudo nice -n -20 taskset -c 2 ./run data/test_avx_o3.csv
