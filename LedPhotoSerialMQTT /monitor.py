#!/usr/bin/env python3
"""
Простой MQTT-прослушиватель для всей ветки iot/home/#.
Логирует всё в формате: [время] [топик] payload
"""

import paho.mqtt.client as mqtt
import time
import random

BROKER = 'broker.emqx.io'
PORT = 1883
CLIENT_ID = f'monitor-{random.randint(10000, 99999)}'
TOPIC = 'iot/home/#'

def on_connect(_, __, ___, rc):
    if rc == 0:
        print('[✓] connected → subscribing to', TOPIC)
        mqttc.subscribe(TOPIC)
    else:
        print(f'[✗] connection failed: rc={rc}')

def on_message(_, __, msg):
    ts = time.strftime('%H:%M:%S')
    try:
        payload = msg.payload.decode()
    except:
        payload = msg.payload.hex()
    print(f'[{ts}] {msg.topic:<25} → {payload}')

mqttc = mqtt.Client(client_id=CLIENT_ID)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect(BROKER, PORT, 60)

print(' waiting for messages (Ctrl+C to stop)')
try:
    mqttc.loop_forever()
except KeyboardInterrupt:
    print('\n[!] monitor stopped')
