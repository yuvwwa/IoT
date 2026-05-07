// Right/left
#define DIR_RIGHT 4 
#define SPEED_RIGHT 5
#define DIR_LEFT 7 
#define SPEED_LEFT 6

#define FORWARD_RIGHT LOW
#define BACKWARD_RIGHT HIGH
#define FORWARD_LEFT HIGH
#define BACKWARD_LEFT LOW

// HC-SR04
#define TRIG_LEFT 8
#define ECHO_LEFT 9

#define TRIG_FRONT_LEFT 10
#define ECHO_FRONT_LEFT 11

#define TRIG_FRONT_RIGHT 12
#define ECHO_FRONT_RIGHT 13

// Скорость
int base_speed = 80;
int MAX_SPEED = 120;

// PID
float Kp = 2.0;
float Ki = 0.0;
float Kd = 1.0;

float previous_error = 0;
float integral = 0;

int target_distance_left = 15; // см

// Состояния
enum State { MOVE_FORWARD, TURN_RIGHT, SEARCH_WALL};
State state = MOVE_FORWARD;

// Движение
void move(bool right_dir, int right_speed, bool left_dir, int left_speed){
  digitalWrite(DIR_RIGHT, right_dir);
  digitalWrite(DIR_LEFT, left_dir);
  analogWrite(SPEED_RIGHT, right_speed);
  analogWrite(SPEED_LEFT, left_speed);
}

void forward(int speed){
  move(FORWARD_RIGHT, speed, FORWARD_LEFT, speed);
}

void rotate_right(int speed){
  move(BACKWARD_RIGHT, speed, FORWARD_LEFT, speed);
}

// HC-SR04 функция
long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // таймаут

  long distance = duration * 0.034 / 2; // см
  return distance;
}

// PID
int computePID(int distance_left) {
    float error = target_distance_left - distance_left;
    integral += error;
    float derivative = error - previous_error;
    previous_error = error;

    float output = Kp*error + Ki*integral + Kd*derivative;

    if(output > 40) output = 40;
    if(output < -40) output = -40;

    return (int)output;
}

// Движение вдоль стены
void follow_wall_left(int distance_left) {
    int correction = computePID(distance_left);

    int left_speed = base_speed - correction;
    int right_speed = base_speed + correction;

    if(left_speed > MAX_SPEED) left_speed = MAX_SPEED;
    if(left_speed < 0) left_speed = 0;

    if(right_speed > MAX_SPEED) right_speed = MAX_SPEED;
    if(right_speed < 0) right_speed = 0;

    move(FORWARD_RIGHT, right_speed, FORWARD_LEFT, left_speed);
}

void setup() {
  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);

  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);

  pinMode(TRIG_FRONT_LEFT, OUTPUT);
  pinMode(ECHO_FRONT_LEFT, INPUT);

  pinMode(TRIG_FRONT_RIGHT, OUTPUT);
  pinMode(ECHO_FRONT_RIGHT, INPUT);

  Serial.begin(9600);
}


void loop() {
  int dist_left = readDistance(TRIG_LEFT, ECHO_LEFT);
  delay(10);
  int dist_front_left = readDistance(TRIG_FRONT_LEFT, ECHO_FRONT_LEFT);
  delay(10);
  int dist_front_right = readDistance(TRIG_FRONT_RIGHT, ECHO_FRONT_RIGHT);

  Serial.print("L: "); Serial.print(dist_left);
  Serial.print(" FL: "); Serial.print(dist_front_left);
  Serial.print(" FR: "); Serial.println(dist_front_right);

  switch(state) {

    case MOVE_FORWARD:
      follow_wall_left(dist_left);

      if(dist_front_left < 10 || dist_front_right < 10) {
          state = TURN_RIGHT;
      }
      else if(dist_left > 25) {
          state = SEARCH_WALL;
      }
      break;

    case TURN_RIGHT:
      forward(60);
      delay(100);

      rotate_right(110);
      delay(350);

      state = MOVE_FORWARD;
      break;

    case SEARCH_WALL:
      forward(70);

      if(dist_left < 20) {
          state = MOVE_FORWARD;
      }
      break;
  }

  delay(50);
}
