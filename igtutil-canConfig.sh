# This script is created as an EXAMPLE for specific purpose on Neousys IGT series.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHORS SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
#
if [ "x$1" == "x" ]; then
  echo "Usage: $0 {bitRate}"
  exit 1
fi

modprobe can_dev
modprobe can
modprobe can_raw


can0=$(ifconfig | grep can0)
if [ "x$can0" != "x" ]; then
  echo "can0 already set up"
  exit 0
fi

sudo ip link set can0 type can bitrate $1
sudo ip link set up can0
