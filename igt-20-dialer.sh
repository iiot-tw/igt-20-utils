#! /bin/bash

if [ "x$1" == "x" ]; then
  echo "mdial: Please specify Modem"
  exit 1
fi

  for i in {1..30}
  do
    echo "igt-dialer: check modem: $i"
    [ -e $1 ] && break
    sleep 1
  done
  if [ $i = 30 ]; then
    echo "igt-dialer: modem not found"
    exit 2
  fi

  while [ 1 ];
  do
    echo "igt-dialer: dialing"
    #at_init.py if for ublox-L210
    #python3 /neousys/at_init.py
    igt20 led1 1
# set Auto Reconnect = off in wvdial.conf    
    wvdial $2
    echo "igt-dialer: wait for redial"
    igt20 led1 0
    sleep 10
  done
