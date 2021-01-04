# works only when eth0 is configured in /etc/network/interfaces
# note that some network manager, such as connman, will ignore configuration in /etc/network/interfaces
sed -i 's|iface eth0 inet static|iface eth0 inet dhcp|' /etc/network/interfaces
systemctl restart networking
