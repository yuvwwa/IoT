const int LED_BUILDIN = 5;  

void setup() {
  cli();
  pinMode(LED_BUILDIN, OUTPUT);
  
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 255;
  
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  
  TIMSK2 |= (1 << TOIE2);
  sei();
}

ISR(TIMER2_OVF_vect) {
  digitalWrite(LED_BUILDIN, !digitalRead(LED_BUILDIN));
}

void loop() {
  // put your main code here, to run repeatedly:
}
