// Управление светодиодом по UART: 'u' - вкл, 'd' - выкл, 'b' - мигание.
// Пин 13 - встроенный LED на большинстве плат (Uno/Nano).

#define LED_PIN 13

char cmd_mode = 'd';   // 'u': on, 'd': off, 'b': blink
bool led_on = false;
uint32_t blink_ts = 0;
const uint16_t BLINK_MS = 500;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  //  Приём команд 
  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {
      case 'u':
        cmd_mode = 'u';
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED_GOES_ON");
        break;
      case 'd':
        cmd_mode = 'd';
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED_GOES_OFF");
        break;
      case 'b':
        cmd_mode = 'b';
        Serial.println("LED_WILL_BLINK");
        break;
      default:
        // Игнорим мусор - не наш формат.
        break;
    }
  }

  //  Реакция на режим 'b' 
  if (cmd_mode == 'b') {
    uint32_t now = millis();
    if (now - blink_ts >= BLINK_MS) {
      blink_ts = now;
      led_on = !led_on;
      digitalWrite(LED_PIN, led_on);
    }
  }
}