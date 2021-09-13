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
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send>"
  echo "Please specify correct device."
#  exit 1
fi

if [ "x$2" != "x115200" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send>"
  echo "Please specify correct baud rate."
#  exit 2
fi

if [ "x$3" = "x" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send>"
  echo "Please specify a string to send"
  exit 3
fi

device=$1
speed=$2
message=$3

dumpFile=/dev/shm/ttydump

stty -F $device $speed raw -echo

exec 3<$device

run=true
count=1
timeout=0
atcmd=false
server=false

if [ "x$message" = "xechoback" ]; then
  server=true
  echo "Running in Echoback server mode:"
  echo "device: $device"
  echo "speed:  $speed"
elif [ "x${message:0:3}" = "xat+" ]; then
  echo "AT Command Detected"
  echo -ne "Message sent: $message\n"
  echo -ne "$message\r" > $device
  atcmd=true
  sleep 0.1s
else
  echo "Message sent: $message"
  echo $message > $device
  sleep 0.1s
fi


while $run;
do
  read -t1 response <&3 #$device
  if [ "x$response" = "x" ]; then
    timeout=$((timeout+1))
    echo -n .
    if [ $timeout -gt 10 ] && [ $server = false ]; then
       run=false
    fi
    continue
  fi
  
  if [ $atcmd = true ]; then
    echo "READ AT Command RESPONSE"
    read -t1 response <&3 #$device
  fi

  count=$((count+1))
  if [ $server = true ]; then
      echo "$count: echoback"
    if [ "x$response" = "xechostop" ]; then
      message="server stop"
      run=false
    else
      message=$response
    fi
    echo "Message recv: $response"
  elif [ $atcmd = true ]; then
    echo "Message recv: $response"
    run=false    
  else
    echo "Message recv: $response"
    if [ "x$response" = "x$message" ]; then
      echo "$count: PASSED!!"
      run=false;  #remark this line to enable an infinite test
    else
      echo "$count: FAILED!!"
      run=false;
    fi
    message=$count
  fi

  if [ $run = true ]; then
    echo "Message sent: $message"
    echo $message > $device
  fi
done

exec 3<&-
