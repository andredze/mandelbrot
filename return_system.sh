# disable cpu limit
sudo cpupower frequency-set -g performance

# turn on graphics
sudo systemctl isolate graphical.target
