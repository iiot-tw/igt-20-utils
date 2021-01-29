# Not all mPCIe WiFi cards works on IGT series.
# this script works for specific models only.
cp ./88x2bu.ko.xz  /lib/modules/$(uname -r)
cp ./88x2bu.conf /etc/modprobe.d/
depmod
