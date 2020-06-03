import psutil
import time
import logging
from tb_device_mqtt import TBDeviceMqttClient

def led(i, v):
    if i<0 or i>5:
        return -1

    if v == 0:
      f = open("/sys/class/leds/igt30::usr"+str(i)+"/brightness", "w")
      f.write("0")
      f.close()
    else:
      f = open("/sys/class/leds/igt30::usr"+str(i)+"/brightness", "w")
      f.write("1")
      f.close()
# dependently of request method we send different data back
def on_server_side_rpc_request(request_id, request_body):
    print(request_id, request_body)
    if request_body["method"] == "sendCommand":
        print(2)
        client.send_rpc_reply(request_id, {"CPU percent": psutil.cpu_percent()})
    elif request_body["method"] == "getValue":
        client.send_rpc_reply(request_id, {"value": 0})
    elif request_body["method"] == "setValue":
        if request_body["params"] == True:
           print(1)
           led(4,1)
        else:
           print(0)
           led(4,0)


client = TBDeviceMqttClient("demo.thingsboard.io", "wjH4O6ArXM535TBSPyxO")
client.set_server_side_rpc_request_handler(on_server_side_rpc_request)
client.connect()
while True:
    time.sleep(1)
    #print( psutil.cpu_percent())
    #print( psutil.virtual_memory().percent)
