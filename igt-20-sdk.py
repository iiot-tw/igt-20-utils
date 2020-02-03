def led(i, v):
    if i<0 or i>5:
        return -1

    if v == 0:
      f = open("/sys/class/leds/igt20::usr"+str(i)+"/brightness", "w")
      f.write("0")
      f.close()
    else:
      f = open("/sys/class/leds/igt20::usr"+str(i)+"/brightness", "w")
      f.write("1")
      f.close()

def btn_read(i):
  if i<0 or i>2:
    return -1

  btn_gpio = ["68","67"]

  f = open("/sys/class/gpio/gpio"+btn_gpio[i]+"/value", "r")
  ret=f.read()
  f.close()

  return ret

def di_read(i):
  if i<0 or i>4:
    return -1

  di_gpio = ["26","27","23","61"]

  f = open("/sys/class/gpio/gpio"+di_gpio[i]+"/value", "r")
  ret=f.read()
  f.close()

  return ret

def do_write(i,v):
  if i<0 or i>4:
    return -1

  do_gpio = ["44","45","46","47"]

  if v==0 :
    f = open("/sys/class/gpio/gpio"+do_gpio[i]+"/value", "w")
    f.write("0")
    f.close()
  else:
    f = open("/sys/class/gpio/gpio"+do_gpio[i]+"/value", "w")
    f.write("1")
    f.close()

def do_enable(v):

  doen_gpio = "65"

  if v==0 :
    f = open("/sys/class/gpio/gpio"+doen_gpio+"/value", "w")
    f.write("1")
    f.close()
  else:
    f = open("/sys/class/gpio/gpio"+doen_gpio+"/value", "w")
    f.write("0")
    f.close()

import time

do_idx=3

do_enable(1)

do_write(do_idx,1)

#just make sure my relay have enough time to contact
time.sleep(1)

for idx in range(0,4):
  print("DI"+str(idx)+": "+ str(di_read(idx)))

do_write(do_idx,0)

do_enable(0)
