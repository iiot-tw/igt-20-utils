wlan0=$(cat /etc/network/interfaces | grep wlan0)
if [ "x$wlan0" == "x" ]; then
  echo "Append wlan0 configuration to /etc/network/interfaces"
  echo "auto wlan0" >> /etc/network/interfaces
  echo "allow-hotplug wlan0" >> /etc/network/interfaces
  echo "iface wlan0 inet dhcp" >> /etc/network/interfaces
  echo "wpa-conf /neousys/wpaIgt.conf" >> /etc/network/interfaces
fi

if [ "x$1" == "x" ]; then
  echo "Usage: sudo ./$0 {SSID} {PASS}"
  exit 1
fi

if [ "x$2" == "x" ]; then
  echo "Usage: sudo ./$0 {SSID} {PASS}"
  exit 2
fi

if [ -a /neousys/wpaIgt.conf ]; then
  echo "/neousys/wpaIgt.conf EXIST"
  exit 3
fi



cat << EOF > /neousys/wpaIgt.conf
ctrl_interface=/var/run/wpa_supplicant
eapol_version=1
ap_scan=1

network={
ssid="$1"
scan_ssid=1
proto=RSN
key_mgmt=WPA-PSK
pairwise=CCMP
group=CCMP
psk="$2"
}
EOF
