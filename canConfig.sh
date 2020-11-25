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
