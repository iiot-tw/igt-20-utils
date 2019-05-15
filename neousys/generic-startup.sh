#!/bin/sh -e

#eMMC flasher just exited single user mode via: [exec /sbin/init]
#as we can't shudown properly in single user mode..
#unset are_we_flasher
#are_we_flasher=$(grep init-eMMC-flasher /proc/cmdline || true)
#if [ ! "x${are_we_flasher}" = "x" ] ; then
#	systemctl poweroff || halt
#	exit
#fi

#Regenerate ssh host keys
if [ -f /etc/ssh/ssh.regenerate ] ; then
	echo "generic-board-startup: regenerating ssh keys"
	systemctl stop sshd
	rm -rf /etc/ssh/ssh_host_* || true

	if [ -e /dev/hwrng ] ; then
		# Mix in the output of the HWRNG to the kernel before generating ssh keys
		dd if=/dev/hwrng of=/dev/urandom count=1 bs=4096 2>/dev/null
		echo "generic-board-startup: if=/dev/hwrng of=/dev/urandom count=1 bs=4096"
	else
		echo "generic-board-startup: WARNING /dev/hwrng wasn't available"
	fi

	dpkg-reconfigure openssh-server
	sync
	if [ -s /etc/ssh/ssh_host_ed25519_key.pub ] ; then
		rm -f /etc/ssh/ssh.regenerate || true
		sync
		systemctl start sshd
	fi
fi

if [ -f /neousys/firstboot ] ; then
	echo "generic-board-startup (Neousys): IGT-20 first boot route"

## by Raymond: get mac address for static ip with connman
	mac_address="/proc/device-tree/ocp/ethernet@4a100000/slave@4a100200/mac-address"
	if [ -f ${mac_address} ] && [ -f /var/lib/connman/settings ] ; then
		raymond_mac=$(hexdump -e '1/1 "%02x"' ${mac_address})
		if [ -d /var/lib/connman/ethernet_${raymond_mac}_cable ] ; then
			rm -rf /var/lib/connman/ethernet_${raymond_mac}_cable
			echo "generic-board-startup (Neousys): delete existing connman configure file"
		fi
		mkdir /var/lib/connman/ethernet_${raymond_mac}_cable
		wfile=/var/lib/connman/ethernet_${raymond_mac}_cable/settings

		echo "[ethernet_${raymond_mac}_cable]" >${wfile}
		echo "Name=Wired" >>${wfile}
		echo "AutoConnect=true" >>${wfile}
		echo "IPv4.method=manual" >>${wfile}
		echo "IPv4.netmask_prefixlen=24" >>${wfile}
		echo "IPv4.local_address=192.168.8.2" >>${wfile}
		echo "IPv4.gateway=192.168.8.1" >>${wfile}

		echo "generic-board-startup (Neousys): restart connman"
		service connman restart
		echo "generic-board-startup (Neousys): IGT-20 first boot route complete"
		rm -rf /neousys/firstboot || true
	else
		echo "generic-board-startup (Neousys): No MAC or no connman found"
	fi
fi

if [ -f "/neousys/resizerootfs" ]; then
	drive=$(cat /neousys/resizerootfs)
	if [ ! "x${drive}" = "x" ] ; then
	        if [ "x${drive}" = "x/dev/mmcblk0" ] || [ "x${drive}" = "x/dev/mmcblk1" ] ; then
	                resize2fs ${drive}p2 >/var/log/resize.log 2>&1 || true
	        else
	                resize2fs ${drive} >/var/log/resize.log 2>&1 || true
	        fi
	fi
	rm -rf /neousys/resizerootfs || true
	sync
fi

if [ -f /proc/device-tree/model ] ; then
	board=$(cat /proc/device-tree/model | sed "s/ /_/g")
	echo "generic-board-startup: [model=${board}]"

	case "${board}" in
	Neousys_IGT-2[01])
		script="igt20-startup.sh"
		;;
	*)
		script="generic.sh"
		;;
	esac

	if [ -f "/neousys/${script}" ] ; then
		echo "generic-board-startup (Neousys): [startup script=/neousys/${script}]"
		/bin/sh /neousys/${script}
	else
		echo "generic-board-startup (Neousys): startup script /neousys/${script} not found"
	fi
fi
