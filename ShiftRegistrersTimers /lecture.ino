
// Контроллер семисегментных индикаторов через каскадные сдвиговые регистры
// таймер-счетчик (00-59) с управлением через Serial


#include <avr/interrupt.h>


// Тип дисплея: true для общего катода, false для общего анода
#define COMMON_CATHODE false

// Определение пинов через прямое управление портами
#define DATA_DDR   DDRD
#define DATA_PORT  PORTD
#define DATA_BIT   (1 << 7)  // Пин D7 - линия данных

#define CLOCK_DDR  DDRD
#define CLOCK_PORT PORTD
#define CLOCK_BIT  (1 << 3)  // Пин D3 - тактовый сигнал

#define LATCH_DDR  DDRD
#define LATCH_PORT PORTD
#define LATCH_BIT  (1 << 5)  // Пин D5 - защелка данных

// Параметры таймера для генерации секундных интервалов
#define PRESCALER_VALUE 1024
#define COMPARE_REGISTER 15624  

// Ограничения счетчика и настройки Serial
#define COUNT_LIMIT 60
#define SERIAL_WAIT_TIME 25
#define BUFFER_CAPACITY 4


// Битовые маски для отображения цифр 0-9
// Структура битов: [DP G F E D C B A]
const uint8_t SEGMENTS_CODE[10] = {
  0b10111011,  // Цифра 0
  0b00001010,  // Цифра 1
  0b01110011,  // Цифра 2
  0b01011011,  // Цифра 3
  0b11001010,  // Цифра 4
  0b11011001,  // Цифра 5
  0b11111101,  // Цифра 6
  0b00001011,  // Цифра 7
  0b11111011,  // Цифра 8
  0b11011111   // Цифра 9
};


volatile uint8_t timeCounter = 0;         // Основной счетчик времени (0-59)
volatile int16_t userInput = -1;          // Буфер пользовательского ввода (-1 = нет данных)
volatile bool isInitialized = false;      // Флаг инициализации системы



// Передача одного байта в каскадные регистры
void shiftOutByte(uint8_t data) {
  for (int8_t i = 7; i >= 0; i--) {
    // Установка линии данных в зависимости от текущего бита
    if (data & (1 << i)) {
      DATA_PORT |= DATA_BIT;      // Данные = HIGH
    } else {
      DATA_PORT &= ~DATA_BIT;     // Данные = LOW
    }
    
    // Генерация тактового импульса для сдвига бита
    CLOCK_PORT |= CLOCK_BIT;      // Такт = HIGH
    CLOCK_PORT &= ~CLOCK_BIT;     // Такт = LOW
  }
}

// Получение битовой маски для отображения цифры
uint8_t encodeDigit(uint8_t digit) {
  if (digit > 9) {
    digit = 0;  // Защита от некорректных значений
  }
  return SEGMENTS_CODE[digit];
}

// Обновление обоих дисплеев новым значением
void refreshDisplay(uint8_t value) {
  // Ограничение значения диапазоном 0-59
  if (value >= COUNT_LIMIT) {
    value = value % COUNT_LIMIT;
  }
  
  // Разделение числа на десятки и единицы
  uint8_t tensDigit = value / 10;
  uint8_t onesDigit = value % 10;
  
  // Получение битовых паттернов для каждой цифры
  uint8_t tensPattern = encodeDigit(tensDigit);
  uint8_t onesPattern = encodeDigit(onesDigit);
  
  // Начало передачи данных (защелка в низкое состояние)
  LATCH_PORT &= ~LATCH_BIT;
  
  // Последовательная передача байтов (сначала десятки, затем единицы)
  shiftOutByte(tensPattern);
  shiftOutByte(onesPattern);
  
  // Фиксация данных на выходах (защелка в высокое состояние)
  LATCH_PORT |= LATCH_BIT;
}


ISR(TIMER1_COMPA_vect) {
  int16_t receivedValue;
  int16_t displayValue;
  
  // Атомарное чтение и сброс пользовательского ввода
  cli();
  receivedValue = userInput;
  userInput = -1;  // Сброс после считывания
  sei();
  
  // Проверка состояния инициализации
  if (!isInitialized) {
    // Ожидание первого значения от пользователя
    if (receivedValue != -1) {
      timeCounter = receivedValue;
      isInitialized = true;
    }
    displayValue = timeCounter;
  } else {
    // Инкремент основного счетчика
    timeCounter++;
    if (timeCounter >= COUNT_LIMIT) {
      timeCounter = 0;  // Циклический сброс при достижении 60
    }
    
    // Определение значения для отображения
    if (receivedValue != -1) {
      // Пользовательский ввод переопределяет текущий тик
      displayValue = receivedValue;
    } else {
      // Отображение значения основного счетчика
      displayValue = timeCounter;
    }
  }
  
  // Обновление дисплеев с новым значением
  refreshDisplay(displayValue);
}


void setup() {
  // Настройка пинов как выходов
  DATA_DDR  |= DATA_BIT;
  CLOCK_DDR |= CLOCK_BIT;
  LATCH_DDR |= LATCH_BIT;
  
  // Инициализация Serial для связи с ПК
  Serial.begin(9600);
  
  // Конфигурация Timer1 для генерации прерываний каждую секунду
  cli();  // Отключение глобальных прерываний
  
  TCCR1A = 0;  // Сброс регистра управления A
  TCCR1B = 0;  // Сброс регистра управления B
  TCNT1  = 0;  // Сброс счетчика
  
  // Режим CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << WGM12);
  
  // Установка предделителя 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  
  // Значение сравнения для частоты 1 Гц
  OCR1A = COMPARE_REGISTER;
  
  // Включение прерывания по совпадению
  TIMSK1 |= (1 << OCIE1A);
  
  sei();  // Включение глобальных прерываний
}



void loop() {
  // Статические переменные для накопления входных данных
  static char serialBuffer[BUFFER_CAPACITY];
  static uint8_t bufferPos = 0;
  static unsigned long lastInputTime = 0;
  
  // Чтение данных из Serial порта
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    
    // Накопление только цифр в пределах размера буфера
    if (isDigit(incomingByte) && bufferPos < 2) {
      serialBuffer[bufferPos] = incomingByte;
      bufferPos++;
      lastInputTime = millis();  // Запоминание времени последнего символа
    }
  }
  
  // Обработка накопленных данных по таймауту
  if (bufferPos > 0 && (millis() - lastInputTime > SERIAL_WAIT_TIME)) {
    // Завершение строки и преобразование в число
    serialBuffer[bufferPos] = '\0';
    int parsedValue = atoi(serialBuffer);
    
    // Атомарная передача значения в обработчик прерывания
    cli();
    userInput = parsedValue;
    sei();
    
    // Сброс буфера для следующего ввода
    bufferPos = 0;
  }
}