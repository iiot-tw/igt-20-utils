#!/bin/bash

run_libcomposite () { 
#	if [ ! -d /sys/kernel/config/usb_gadget/g_multi/ ]; then
		echo "${log} Creating g_multi"
		mkdir -p /sys/kernel/config/usb_gadget/g_multi || true
		cd /sys/kernel/config/usb_gadget/g_multi
		echo ${usb_bcdUSB} > bcdUSB
		echo ${usb_idVendor} > idVendor # Linux Foundation
		echo ${usb_idProduct} > idProduct # Multifunction Composite Gadget
		echo ${usb_bcdDevice} > bcdDevice
		#0x409 = english strings...
		mkdir -p strings/0x409
		echo ${usb_iserialnumber} > strings/0x409/serialnumber
		echo ${usb_imanufacturer} > strings/0x409/manufacturer
		echo ${usb_iproduct} > strings/0x409/product
		mkdir -p configs/c.1/strings/0x409
		echo "BeagleBone Composite" > configs/c.1/strings/0x409/configuration
		echo 500 > configs/c.1/MaxPower
		if [ ! "x${USB_NETWORK_RNDIS_DISABLED}" = "xyes" ]; then
			mkdir -p functions/rndis.usb0
			# first byte of address must be even
			echo ${cpsw_2_mac} > functions/rndis.usb0/host_addr
			echo ${cpsw_1_mac} > functions/rndis.usb0/dev_addr
			# Starting with kernel 4.14, we can do this to 
			# match Microsoft's built-in RNDIS driver. 
			# Earlier kernels require the patch below as a 
			# work-around instead: 
			# https://github.com/beagleboard/linux/commit/e94487c59cec8ba32dc1eb83900297858fdc590b
			if [ -f functions/rndis.usb0/class ]; then
				echo EF > functions/rndis.usb0/class
				echo 04 > functions/rndis.usb0/subclass
				echo 01 > functions/rndis.usb0/protocol 
			fi

			echo 1 > os_desc/use
			echo 0xCD > os_desc/b_vendor_code
			echo MSFT100 > os_desc/qw_sign
			echo "RNDIS" > functions/rndis.usb0/os_desc/interface.rndis/compatible_id 
			echo "5162001" > functions/rndis.usb0/os_desc/interface.rndis/sub_compatible_id
			mkdir -p configs/c.1
			ln -s configs/c.1 os_desc
			mkdir -p functions/rndis.usb0/os_desc/interface.rndis/Icons 
			echo 2 > functions/rndis.usb0/os_desc/interface.rndis/Icons/type 
			echo "%SystemRoot%\\system32\\shell32.dll,-233"	> functions/rndis.usb0/os_desc/interface.rndis/Icons/data 
			mkdir -p functions/rndis.usb0/os_desc/interface.rndis/Label
			echo 1 > functions/rndis.usb0/os_desc/interface.rndis/Label/type 
			echo "BeagleBone USB Ethernet" > functions/rndis.usb0/os_desc/interface.rndis/Label/data 
			ln -s functions/rndis.usb0 configs/c.1/
			usb0="enable"
		fi

		mkdir -p functions/acm.usb0
		ln -s functions/acm.usb0 configs/c.1/
		#ls /sys/class/udc v4.4.x-ti
		if [ -d /sys/class/udc/musb-hdrc.1.auto ] ; then
			echo musb-hdrc.1.auto > UDC
		else
			#v4.9.x-ti
			if [ -d /sys/class/udc/musb-hdrc.1 ] ; then
				echo musb-hdrc.1 > UDC
			fi
		fi
		echo "${log} g_multi Created"
#	else
#		echo "${log} FIXME: need to bring down g_multi first, before running a second time."
#	fi
}

usb_idVendor="0x1d6b"
usb_idProduct="0x0104"
usb_bcdDevice="0x0404"
usb_bcdUSB="0x0200"
usb_serialnr="000000"
usb_product="USB Device"
unset USB_NETWORK_RNDIS_DISABLED
unset USB_NETWORK_CDC_DISABLED
cpsw_2_mac="48:6f:73:74:50:43" # "HostPC"
cpsw_1_mac="42:61:64:55:53:42" # "BadUSB"


#load driver
sudo modprobe libcomposite

echo "${log} modprobe libcomposite"
modprobe libcomposite || true
if [ -d /sys/module/libcomposite ] ; then
	run_libcomposite
else
	if [ -f /sbin/depmod ] ; then
		/sbin/depmod -a
	fi
	echo "${log} ERROR: [libcomposite didn't load]"
fi

# start the console service on the tty
sudo systemctl start serial-getty@ttyGS0.service
#put this at the very end of the file:
ifconfig usb0 192.168.7.2 netmask 255.255.255.252 up
route add -net default gw 192.168.7.255
