#!/usr/bin/env python3
"""
Подписка на уровень освещённости -> управление LED на Arduino.
Если lux < порог - 'u', иначе - 'd'.
"""

import serial
import time
import paho.mqtt.client as mqtt
import random

#  Настройки 
SERIAL_PORT = '/dev/ttyUSB1'
BAUD = 9600

BROKER = 'broker.emqx.io'
PORT = 1883
CLIENT_ID = f'actuator-{random.randint(10000, 99999)}'

TOPIC_LUX = 'iot/home/luminosity'
TOPIC_LIGHT = 'iot/home/light_status'
TOPIC_STAT = 'iot/home/actuator_status'

THRESHOLD = 10  # при значении ниже - включаем свет

arduino = None

def on_connect(client, _, __, rc):
    if rc == 0:
        print('[✓] MQTT: connected')
        client.publish(TOPIC_STAT, 'online', retain=True)
        client.subscribe(TOPIC_LUX)
        print(f'[✓] subscribed to {TOPIC_LUX}')
    else:
        print(f'[✗] MQTT rc={rc}')

def on_message(_, __, msg):
    global arduino
    if not (arduino and arduino.is_open):
        return

    try:
        lux = int(msg.payload)
        cmd = b'u' if lux < THRESHOLD else b'd'
        state = 'ON' if cmd == b'u' else 'OFF'

        arduino.write(cmd)
        mqttc.publish(TOPIC_LIGHT, state, retain=True)
        print(f' lux={lux} → LED {state}')

    except Exception as e:
        print(f'[!] bad message: {msg.payload} ({e})')

#  Инициализация 
try:
    arduino = serial.Serial(SERIAL_PORT, BAUD, timeout=1)
    time.sleep(1.5)
    print(f'[✓] Serial: {SERIAL_PORT}')
except Exception as e:
    exit(f'[✗] serial init failed: {e}')

mqttc = mqtt.Client(client_id=CLIENT_ID)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.will_set(TOPIC_STAT, 'offline (crash)', qos=1, retain=True)
mqttc.connect(BROKER, PORT, 60)

try:
    mqttc.loop_forever()
except KeyboardInterrupt:
    print('\n[!] interrupted')
finally:
    mqttc.publish(TOPIC_STAT, 'offline (graceful)', retain=True)
    mqttc.disconnect()
    if arduino:
        arduino.close()
    print('[✓] cleaned up')
