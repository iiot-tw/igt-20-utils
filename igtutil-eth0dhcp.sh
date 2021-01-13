# This script is created as an EXAMPLE for specific purpose on Neousys IGT series.
#
# It works only when eth0 is configured in /etc/network/interfaces
# Note that some network manager, such as connman, will ignore the configuration
# in /etc/network/interface and make this script not work as expected.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHORS SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
#
sed -i 's|iface eth0 inet static|iface eth0 inet dhcp|' /etc/network/interfaces
systemctl restart networking
