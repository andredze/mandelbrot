# disable cpu limit
sudo cpupower frequency-set -g performance

# return gui
sudo systemctl isolate graphical.target