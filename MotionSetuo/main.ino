//right
#define DIR_RIGHT 4 
#define SPEED_RIGHT 5

//left
#define DIR_LEFT 7
#define SPEED_LEFT 6

#define FORWARD_RIGHT HIGH
#define BACKWARD_RIGHT LOW
#define FORWARD_LEFT HIGH
#define BACKWARD_LEFT LOW

void move(
  bool left_dir, int left_speed, bool right_dir, int right_speed
){
  digitalWrite(DIR_RIGHT, left_dir);
  digitalWrite(DIR_LEFT, right_dir);
  analogWrite(SPEED_RIGHT, left_speed);
  analogWrite(SPEED_LEFT, right_speed);
}

void forward(int speed){
  move(FORWARD_RIGHT, speed, FORWARD_LEFT, speed);
}

void backward(int speed){
  move(BACKWARD_RIGHT, speed, BACKWARD_LEFT, speed);
}

void turn_left(int steepness_right, int steepness_left){ //крутизна поворота
  // steepness is how steep is turn
  move(FORWARD_RIGHT, steepness_right, FORWARD_LEFT, steepness_left);

}

void turn_right(int steepness_right, int steepness_left){ //крутизна поворота
  move(FORWARD_RIGHT, steepness_right, FORWARD_LEFT, steepness_left);
}

void rotate_left(int steepness_right, int steepness_left){ //
  move(FORWARD_RIGHT, steepness_right, FORWARD_LEFT, steepness_left);
}

void rotate_right(int steepness_right, int steepness_left){ //
  move(FORWARD_RIGHT, steepness_right, FORWARD_LEFT, steepness_left);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);

  //move(FORWARD_RIGHT, 255, FORWARD_LEFT, 0);
  //delay(2000);
  //move(FORWARD_RIGHT, 0, FORWARD_LEFT, 255);
  //delay(2000);
  //move(FORWARD_RIGHT, 0, FORWARD_LEFT, 0);
  //delay(2000);
  //move(FORWARD_RIGHT, 255, FORWARD_LEFT, 255);
  //delay(2000);
  //move(FORWARD_RIGHT, 0, FORWARD_LEFT, 0);
  //delay(2000);
  //backward(200);
  //turn_left(255, 120);
  rotate_right(0, 255);
  delay(2000);
  rotate_left(255, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
