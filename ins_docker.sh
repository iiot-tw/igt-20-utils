apt-get update
apt-get install -y apt-transport-https software-properties-common
wget -qO -  https://download.docker.com/linux/debian/gpg | sudo apt-key add -
add-apt-repository "deb [arch=armhf] https://download.docker.com/linux/debian $(lsb_release -cs) stable"
apt-get update
apt-get install -y docker-ce
update-initramfs -u -t
reboot
