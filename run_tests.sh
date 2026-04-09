bash prepare_system.sh

bash bench.sh

bash return_system.sh

# check throttling counter
grep . /sys/devices/system/cpu/cpu*/thermal_throttle/*_throttle_count
