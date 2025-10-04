volatile unsigned long myMillisValue = 0;

unsigned long MyMillis() {
  unsigned long result;
  noInterrupts();
  result = myMillisValue;
  interrupts();
  return result;
}

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 249;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  
  Serial.println("begin");
}

ISR(TIMER1_COMPA_vect) {
  myMillisValue++;
}

void loop() {
  unsigned long currentTime = MyMillis();
  
  digitalWrite(5, (currentTime / 1000) % 2);
  
  static unsigned long lastTime = 0;
  if (currentTime - lastTime >= 1000) {
    Serial.print(currentTime);
    Serial.println(" мс");
    lastTime = currentTime;
  }
}

