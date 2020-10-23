wget https://raw.githubusercontent.com/iiot-tw/igt-20-utils/master/grow_partition.sh
#newer image seem modified generic-startup.sh
#wget -O /neousys/generic-startup.sh https://raw.githubusercontent.com/iiot-tw/igt-20-utils/master/neousys/generic-startup.sh
chmod +x grow_partition.sh
./grow_partition.sh
reboot
