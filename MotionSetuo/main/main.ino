// Сканируем пины 2–13
void setup() {
  Serial.begin(9600);
}

long testPair(int trig, int echo) {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 20000);
  long distance = duration * 0.034 / 2;

  return distance;
}

void loop() {
  for (int trig = 2; trig <= 13; trig++) {
    for (int echo = 2; echo <= 13; echo++) {
      if (trig == echo) continue;

      long d = testPair(trig, echo);

      // фильтр — показываем только реальные значения
      if (d > 5 && d < 200) {
        Serial.print("TRIG=");
        Serial.print(trig);
        Serial.print(" ECHO=");
        Serial.print(echo);
        Serial.print(" -> ");
        Serial.println(d);

        delay(300); // чтобы успеть увидеть
      }
    }
  }

  Serial.println("---- cycle ----");
}