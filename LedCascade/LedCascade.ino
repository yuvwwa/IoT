#define PERIOD_LED1 1
#define PERIOD_LED2 2
#define PERIOD_LED3 3
#define PERIOD_LED4 4
#define PERIOD_LED5 5

volatile unsigned int count = 0;
volatile uint8_t portb_state = 0;  

ISR(TIMER1_OVF_vect) {
  count++;
  
  if (count % PERIOD_LED1 == 0) portb_state ^= (1 << 0);
  if (count % PERIOD_LED2 == 0) portb_state ^= (1 << 1);
  if (count % PERIOD_LED3 == 0) portb_state ^= (1 << 2);
  if (count % PERIOD_LED4 == 0) portb_state ^= (1 << 3);
  if (count % PERIOD_LED5 == 0) portb_state ^= (1 << 4);
  
  PORTB = portb_state;  
  
  if (count >= 60) count = 0;  
}

void setup() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << TOIE1);
  DDRB |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
  PORTB = 0;  
  sei();
}

void loop() {}
