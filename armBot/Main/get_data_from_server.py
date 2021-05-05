# -*- coding: utf-8 -*-
from iot10086 import Iot10086
import socket
import time

apikey='p2CLc6GlMAsKnBwSQ80H3TeQMec='
apiurl='http://api.heclouds.com/devices/710289579/datastreams/'

iot = Iot10086(apikey, apiurl)
server_addr = ('localhost', 2300)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# 定时刷新界面接收小程序端发送的数据
try:
    s.connect(server_addr)
    while True:
        iot_post_status = iot.get_data('raspberryPi')
        colors = iot_post_status
        print('colors',colors['data']['current_value'])
        s.send(colors['data']['current_value'].encode())
except:
    print("wuhu!")
