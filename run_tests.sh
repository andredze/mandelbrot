# dedicate a CPU core for my program
# nice -n -20  = run with highest priority 
# taskset -c 2 = use CPU core #2

# ——————————————————————————————————————————————————————————————————————————————————————————

# script compiles + runs all versions like this:
# make CXX=g++ VERSION=AVX OPT=-O2
# sudo nice -n -20 taskset -c 2 ./run data/g++/AVX_-O2.csv

sudo sh s-tui.sh

run_test()
{
    local compiler="$1"
    local version="$2"
    local optimization_flag="$3"
    local output_file_path="data/${compiler}/${version}_${optimization_flag}.csv"

    make CXX="$compiler" VERSION="$version" OPT="$optimization_flag"
    sudo nice -n -20 taskset -c 2 ./run "$output_file_path"
}

for compiler in g++ clang++; do
    for version in NAIVE ARRAYS AVX; do
        for optimization_flag in -O2 -O3 -Ofast; do
            run_test "$compiler" "$version" "$optimization_flag"
        done
    done
done

# check throttling counter
grep . /sys/devices/system/cpu/cpu*/thermal_throttle/*_throttle_count

# close s-tui running in another tty
sudo pkill s-tui

# ——————————————————————————————————————————————————————————————————————————————————————————