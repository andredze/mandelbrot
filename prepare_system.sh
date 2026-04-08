# Ctrl + Alt + F3 to enter text console (tty)

# disable graphics
sudo systemctl isolate multi-user.target

# limit processor's frequency
sudo cpupower frequency-set --max 2.20GHz

# turn on performance for all CPU
for f in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
  echo performance | sudo tee "$f"
done

# disable turbo as it is less stable
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo

# check throttling counter
# grep . /sys/devices/system/cpu/cpu*/thermal_throttle/*_throttle_count
