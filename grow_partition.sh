#!/bin/sh -e
if ! id | grep -q root; then
	echo "must be run as root"
	exit
fi

unset root_drive
root_drive="$(cat /proc/cmdline | sed 's/ /\n/g' | grep root= | awk -F 'root=' '{print $2}' || true)"

if [ ! "x${root_drive}" = "x" ] ; then
	boot_drive="${root_drive%?}1"
else
	echo "Error: script halting, could detect drive..."
	exit 1
fi

if [ -f /boot/SOC.sh ] ; then
	. /boot/SOC.sh
fi

if [ "x${boot_drive}" = "x/dev/mmcblk0p1" ] ; then
	drive="/dev/mmcblk0"
elif [ "x${boot_drive}" = "x/dev/mmcblk1p1" ] ; then
	drive="/dev/mmcblk1"
else
	echo "Error: script halting, could detect drive..."
	exit 1
fi

echo "Media: [${drive}]"

echo "${drive}p1" > /resizerootfs
conf_boot_startmb=${conf_boot_startmb:-"4"}
sfdisk_fstype=${sfdisk_fstype:-"L"}
if [ "x${sfdisk_fstype}" = "x0x83" ] ; then
	sfdisk_fstype="L"
fi

sfdisk_options="--force --no-reread --Linux --in-order --unit M"
test_sfdisk=$(LC_ALL=C sfdisk --help | grep -m 1 -e "--in-order" || true)
if [ "x${test_sfdisk}" = "x" ] ; then
	echo "sfdisk: 2.26.x or greater"
	sfdisk_options="--force --no-reread"
	conf_boot_startmb="${conf_boot_startmb}M"
fi

LC_ALL=C sfdisk ${sfdisk_options} ${drive} <<-__EOF__
	${conf_boot_startmb},,${sfdisk_fstype},*
__EOF__
