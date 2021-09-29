# This script is created as an EXAMPLE for specific purpose on Neousys IGT series.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHORS SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
#
#!/bin/bash

function finish() {
  run=false
}

trap finish SIGINT

if [ "x$1" != "x/dev/ttyS2" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send> [last]"
  echo "Please specify correct device."
#  exit 1
fi

if [ "x$2" != "x115200" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send> [last]"
  echo "Please specify correct baud rate."
#  exit 2
fi

if [ "x$3" = "x" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send> [last]"
  echo "Please specify a string to send"
  exit 3
fi

if [ "x$4" != "x" ]; then
    longtest=true
else
    longtest=false
fi

device=$1
speed=$2
message=$3

dumpFile=/dev/shm/ttydump

stty -F $device $speed raw -echo

exec 3<$device

run=true
count=0
atcmd=false
server=false
failcount=0
timeout_max=5

clear

SECONDS=0

while $run;
do
  if [ $longtest = true ]; then
    duration=$SECONDS
    echo -e "\033[1;1f$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."  
  fi
  if [ "x${message:0:3}" = "xat+" ]; then
    echo "AT Command Detected"
    echo -ne "Message sent: $message\n"
    echo -ne "$message\r" > $device
    atcmd=true
    #sleep 0.1s
  else
    echo "Message sent: $message"
    echo $message > $device
    #sleep 0.1s
  fi
  count=$(($count+1))

  timeout=0
  response=""
  
  while [ $timeout -lt $timeout_max ] && [ "x$response" == "x" ];
  do
    #if [ $run = false ]; then break; fi
    read -t1 response <&3 #$device
    timeout=$((timeout+1))
    echo -n .
  done
  
  if [ $atcmd = true ]; then
    echo "READ AT Command RESPONSE"
    read -t1 response <&3 #$device
    echo "Message recv: $response"
    run=false    
  else
    echo -e "\033[KMessage recv: $response"
    if [ "x$response" = "x$message" ]; then
	  failrate=$(bc<<<"scale=2;$failcount*100/$count")
      echo -n "$count: PASSED!!"
	  if [ $longtest = false ]; then
        run=false
	  fi
    else
	  failcount=$(($failcount+1))
	  failrate=$(bc<<<"scale=2;$failcount*100/$count")
      echo -n "$count: FAILED!!"
      if [ $longtest = false ]; then
	    run=false;
	  fi
    fi
	echo "Fail: $failcount ($failrate%)"
    #message=$count
  fi
done

exec 3<&-
