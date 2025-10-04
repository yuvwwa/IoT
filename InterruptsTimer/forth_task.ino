void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
}

void loop() {
  digitalWrite(5, (millis() / 1000) % 2);
  
  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 250) {
    int sensorValue = analogRead(A0);
    Serial.print("Время: ");
    Serial.print(millis());
    Serial.print(" мс, A0: ");
    Serial.println(sensorValue);
    lastRead = millis();
  }
}