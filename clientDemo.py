from machine import ADC, Pin
import socket
import time
port = 60000
host = "192.168.1.101"
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) 
buf_head="s"
adc=ADC(0)
button=Pin(4,Pin.IN)
def mysend():
    while True:
        value1=adc.read()
        value2=button.value()
        buf="%s%d,%d,123,"%(buf_head,value1,value2)
        print(buf)
        s.sendto(buf,(host,port))
        time.sleep(1)
    return 1
