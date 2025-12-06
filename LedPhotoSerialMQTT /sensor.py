#!/usr/bin/env python3
"""
MQTT-публикация данных с датчика через Arduino.
Запрашивает 'p' → парсит 'SENSOR_VALUE:xxx' → шлёт в топик.
"""

import serial
import time
import paho.mqtt.client as mqtt
import random

#  Конфигурация
SERIAL_PORT = '/dev/ttyUSB0'
BAUD = 9600

BROKER = 'broker.emqx.io'
PORT = 1883
CLIENT_ID = f'sensor-{random.randint(10000, 99999)}'

TOPIC_LUX = 'iot/home/luminosity'
TOPIC_STAT = 'iot/home/sensor_status'

# Инициализация 
mqttc = mqtt.Client(client_id=CLIENT_ID)
arduino = None

def on_connect(client, _, __, rc):
    if rc == 0:
        print('[✓] MQTT: connected')
        client.publish(TOPIC_STAT, 'online', retain=True)
    else:
        print(f'[✗] MQTT error: {rc}')

mqttc.on_connect = on_connect
mqttc.will_set(TOPIC_STAT, 'offline (crash)', qos=1, retain=True)
mqttc.connect(BROKER, PORT, 60)
mqttc.loop_start()  # фоновый тред

try:
    arduino = serial.Serial(SERIAL_PORT, BAUD, timeout=1)
    time.sleep(1.5)  # дать ардуино стартануть
    print(f'[✓] Serial: {SERIAL_PORT}')
except Exception as e:
    exit(f'[✗] Serial init failed: {e}')

#  Основной цикл 
try:
    while True:
        arduino.write(b'p')
        raw = arduino.readline().decode().strip()

        if raw.startswith('SENSOR_VALUE:'):
            try:
                lux = int(raw.split(':', 1)[1])
                mqttc.publish(TOPIC_LUX, lux, qos=1)
                print(f'→ {lux}')
            except (ValueError, IndexError):
                print(f'[!] malformed: {raw}')
        time.sleep(5)

except KeyboardInterrupt:
    print('\n[!] interrupted')

finally:
    mqttc.publish(TOPIC_STAT, 'offline (graceful)', retain=True)
    mqttc.disconnect()
    mqttc.loop_stop()
    if arduino:
        arduino.close()
    print('[✓] cleaned up')
