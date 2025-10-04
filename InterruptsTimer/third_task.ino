const int LED_INT = 5;       // Светодиод, управляемый прерыванием Таймера2
const int LED_MODE1 = 6;     // Светодиод для режима 1 (управляемый Serial)
const int LED_MODE2 = 7;     // Светодиод для режима 2 (управляемый Serial)

// Для управления миганием по прерыванию Timer2
volatile int timer2_overflow_count = 0;
const int TIMER2_TARGET_OVERFLOWS = 31; 

// Для управления режимами мигания через Serial
int currentBlinkMode = 0; // 0 - нет активных режимов, 1 - Mode1, 2 - Mode2

unsigned long previousMillisMode1 = 0;
const long intervalMode1 = 1000; 

unsigned long previousMillisMode2 = 0;
const long intervalMode2 = 250; 


void setup() {
  pinMode(LED_INT, OUTPUT);
  pinMode(LED_MODE1, OUTPUT);
  pinMode(LED_MODE2, OUTPUT);

  Serial.begin(9600);

  delay(1000);


  Serial.println("Arduino Blink Modes Control:");
  Serial.println("  '0': Turn off mode-controlled LEDs");
  Serial.println("  '1': Activate Slow Blink (LED on pin 6)");
  Serial.println("  '2': Activate Fast Blink (LED on pin 7)");

  cli();

  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 

  TIMSK2 |= (1 << TOIE2); 

  sei();
}

ISR(TIMER2_OVF_vect) {
  timer2_overflow_count++; 

  if (timer2_overflow_count >= TIMER2_TARGET_OVERFLOWS) {
    digitalWrite(LED_INT, !digitalRead(LED_INT)); 
    timer2_overflow_count = 0; 
  }
}


void loop() {
  if (Serial.available()) {
    char command = Serial.read(); 

    if (command == '\r' || command == '\n') {
      return; 
    }

    switch (command) {
      case '0':
        currentBlinkMode = 0;
        digitalWrite(LED_MODE1, LOW); // Выключаем LED_MODE1
        digitalWrite(LED_MODE2, LOW); // Выключаем LED_MODE2
        Serial.println("Mode-controlled LEDs OFF.");
        break;
      case '1':
        currentBlinkMode = 1;
        digitalWrite(LED_MODE2, LOW); // Выключаем LED_MODE2, если он был активен
        Serial.println("Activated Mode 1: Slow Blink (LED on pin 6)");
        break;
      case '2':
        currentBlinkMode = 2;
        digitalWrite(LED_MODE1, LOW); // Выключаем LED_MODE1, если он был активен
        Serial.println("Activated Mode 2: Fast Blink (LED on pin 7)");
        break;
      default:
        Serial.println("Unknown command. Use '0', '1', or '2'.");
        break;
    }
  }

  // Управление миганием в зависимости от выбранного режима

  // Режим 1: Медленное мигание на LED_MODE1
  if (currentBlinkMode == 1) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisMode1 >= intervalMode1) {
      previousMillisMode1 = currentMillis;
      digitalWrite(LED_MODE1, !digitalRead(LED_MODE1)); // Переключаем состояние LED_MODE1
    }
  }

  // Режим 2: Быстрое мигание на LED_MODE2
  if (currentBlinkMode == 2) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisMode2 >= intervalMode2) {
      previousMillisMode2 = currentMillis;
      digitalWrite(LED_MODE2, !digitalRead(LED_MODE2)); // Переключаем состояние LED_MODE2
    }
  }
}
