// Чтение показаний с A0 (фоторезистор).
// Команды: 'p' - одноразовый опрос, 's' - стрим каждые 2 сек.

#define PHOTO_PIN A0

bool stream_active = false;
uint32_t last_send = 0;
const uint16_t STREAM_MS = 2000;

void setup() {
  Serial.begin(9600);
  pinMode(PHOTO_PIN, INPUT);
}

void loop() {
  //  Приём команд 
  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {
      case 'p':
        stream_active = false;
        send();
        break;
      case 's':
        stream_active = true;
        Serial.println("STREAM_STARTED");
        break;
    }
  }

  //  если включён 
  if (stream_active && (millis() - last_send >= STREAM_MS)) {
    send();
    last_send = millis();
  }
}

void send() {
  int val = analogRead(PHOTO_PIN);
  Serial.print("SENSOR_VALUE:");
  Serial.println(val);
}