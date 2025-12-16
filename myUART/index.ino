#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define F_CPU 16000000UL

// пины
#define PIN_TX 3 
#define PIN_RX 2 

// буферы
#define BUF_SIZE 64
#define BUF_MASK (BUF_SIZE - 1)

// длительность бита
volatile uint16_t bit_period_ticks = 208;

// TX
volatile char tx_buf[BUF_SIZE];
volatile uint8_t tx_head = 0;
volatile uint8_t tx_tail = 0;
volatile bool tx_active = false;
volatile uint8_t tx_byte;
volatile uint8_t tx_bit_pos;

// RX
volatile char rx_buf[BUF_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;
volatile bool rx_sampling = false;
volatile uint8_t rx_byte;
volatile uint8_t rx_bit_pos;
volatile bool rx_ignore_first = true;


void uart_set_baudrate(int rate) {
  bit_period_ticks = (F_CPU / 8) / rate;
}

uint8_t uart_available() {
  return (rx_head - rx_tail) & BUF_MASK;
}

char uart_read() {
  if (rx_head == rx_tail) return 0;
  char data = rx_buf[rx_tail];
  rx_tail = (rx_tail + 1) & BUF_MASK;
  return data;
}

void uart_send(char b) {
  uint8_t next = (tx_head + 1) & BUF_MASK;
  if (next == tx_tail) return;

  tx_buf[tx_head] = b;
  tx_head = next;

  noInterrupts();
  if (!tx_active) {
    tx_active = true;
    tx_byte = tx_buf[tx_tail];
    tx_tail = (tx_tail + 1) & BUF_MASK;
    tx_bit_pos = 0;

    PORTD &= ~_BV(PIN_TX);

    OCR1A = TCNT1 + bit_period_ticks;
    TIFR1 |= _BV(OCF1A);
    TIMSK1 |= _BV(OCIE1A);
  }
  interrupts();
}

bool uart_read_string(char *rx_data) {
  bool any = false;
  uint8_t i = 0;
  while (uart_available()) {
    rx_data[i++] = uart_read();
    any = true;
  }
  rx_data[i] = '\0';
  return any;
}


void setup() {
  DDRD |= _BV(PIN_TX);
  PORTD |= _BV(PIN_TX);
  DDRD &= ~_BV(PIN_RX);
  PORTD |= _BV(PIN_RX);

  Serial.begin(9600);

  noInterrupts();

  TCCR1A = 0;
  TCCR1B = _BV(CS11);
  TCNT1 = 0;

  EICRA = _BV(ISC01);
  EIMSK |= _BV(INT0);

  uart_set_baudrate(9600);

  interrupts();
}

void loop() {
  char buf[65];

  if (uart_read_string(buf)) {
    Serial.print(buf);
  }

  if (Serial.available()) {
    uart_send(Serial.read());
  }
}


ISR(INT0_vect) {
  if (rx_sampling) return;

  rx_sampling = true;
  rx_bit_pos = 0;
  rx_byte = 0;

  EIMSK &= ~_BV(INT0);

  OCR1B = TCNT1 + bit_period_ticks + (bit_period_ticks >> 1);
  TIFR1 |= _BV(OCF1B);
  TIMSK1 |= _BV(OCIE1B);
}

// RX sampling
ISR(TIMER1_COMPB_vect) {
  OCR1B += bit_period_ticks;

  if (rx_bit_pos < 8) {
    if (PIND & _BV(PIN_RX))
      rx_byte |= (1 << rx_bit_pos);
    rx_bit_pos++;
  }
  else {
    if (PIND & _BV(PIN_RX)) {
      if (!rx_ignore_first) {
        uint8_t next = (rx_head + 1) & BUF_MASK;
        if (next != rx_tail) {
          rx_buf[rx_head] = rx_byte;
          rx_head = next;
        }
      }
      rx_ignore_first = false;
    }

    rx_sampling = false;
    TIMSK1 &= ~_BV(OCIE1B);
    EIFR |= _BV(INTF0);
    EIMSK |= _BV(INT0);
  }
}

// TX
ISR(TIMER1_COMPA_vect) {
  if (tx_bit_pos < 8) {
    if (tx_byte & 1) PORTD |= _BV(PIN_TX);
    else             PORTD &= ~_BV(PIN_TX);

    tx_byte >>= 1;
    tx_bit_pos++;
    OCR1A += bit_period_ticks;
  }
  else if (tx_bit_pos == 8) {
    PORTD |= _BV(PIN_TX); // стоп-бит
    tx_bit_pos++;
    OCR1A += bit_period_ticks;
  }
  else {
    if (tx_head != tx_tail) {
      tx_byte = tx_buf[tx_tail];
      tx_tail = (tx_tail + 1) & BUF_MASK;
      tx_bit_pos = 0;
      PORTD &= ~_BV(PIN_TX);
      OCR1A += bit_period_ticks;
    } else {
      tx_active = false;
      TIMSK1 &= ~_BV(OCIE1A);
    }
  }
}