class IGT():

  __led_fs = ""
  __gpio_fs= "/sys/class/gpio/gpio"
  __btn_gpio = []

  def __init__(self, led, btn):
    self.__btn_gpio = btn
    self.__led_fs = led

  def btn_read(self, i):
    if i<0 or i>2:
      return -1

    gpio = self.__btn_gpio

    f = open(self.__gpio_fs+gpio[i]+"/value", "r")
    ret=f.read()
    f.close()
    return ret

  def led_write(self, i, v):
    if i<0 or i>5:
        return -1

    if v == 0:
      f = open(self.__led_fs+str(i)+"/brightness", "w")
      f.write("0")
      f.close()
    else:
      f = open(self.__led_fs+str(i)+"/brightness", "w")
      f.write("1")
      f.close()

  def led_read(self, i):
    if i<0 or i>5:
        return -1

    f = open(self.__led_fs+str(i)+"/brightness", "r")
    ret=int(f.read())
    f.close()
    return ret

class IGT30(IGT):
  __led_fs = "/sys/class/leds/igt30::usr"
  __btn_gpio = ["72", "73"]

  def __init__(self):
    super().__init__(self.__led_fs, self.__btn_gpio)

if __name__== '__main__':
  import time

  a=IGT30()

  while True:
    a.led(4,1)
    time.sleep(1)
    a.led(4,0)
    time.sleep(1)
    print (a.btn_read(0))
