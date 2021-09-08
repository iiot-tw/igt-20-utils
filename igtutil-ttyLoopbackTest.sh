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

if [ "x$1" != "x/dev/ttyS2" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send>"
  echo "Please specify correct device."
  exit 1
fi

if [ "x$2" != "x115200" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send>"
  echo "Please specify correct baud rate."
  exit 2
fi

if [ "x$3" = "x" ]; then
  echo "Usage: $0 /dev/ttyS2 115200 <string_to_send>"
  echo "Please specify a string to send"
  exit 3
fi

device=$1
speed=$2
message=$3

if [ "x$message" = "xechoback" ]; then
  echo "Running in Echoback server mode:"
  echo "device: $device"
  echo "speed:  $speed"
fi

dumpFile=/dev/shm/ttydump

stty -F $device $speed raw -echo -crtscts

exec 3<$device

run=true;
count=0;

while $run;
do
  if [ "$message" = "echoback" ]; then
    read x < $device
    sleep 0.1s
    echo $x
    if [ "$x" = "echostop" ]; then
      run=false
      x="server stop"
    fi
    echo $x > $device
  else
    cat <&3 > $dumpFile &
    PID=$!
    echo "Message sent: $message"
    echo $message > $device
    sleep 0.2s
    kill $PID
    wait $PID 2>/dev/null

    response=$(<$dumpFile)
    echo "Response rcv: $response"

    count=$((count+1));

    if [ "x$message" != "x$response" ]; then
      echo "$count: FAILED!!"
      run=false;
    else
      echo "$count: PASSED!!"
      run=false;  #remark this line to enable an infinite test
    fi
  fi
done

exec 3<&-
