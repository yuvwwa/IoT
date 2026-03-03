//left
#define DIR_1 4 
#define SPEED_1 5

//right
#define DIR_2 7
#define SPEED_2 6

//поменять местами
#define FORWARD_LEFT LOW
#define BACKWARD_LEFT HIGH
#define FORWARD_RIGHT HIGH
#define BACKWARD_RIGHT LOW

void move(
  bool left_dir, int left_speed, bool right_dir, int right_speed
){
  digitalWrite(DIR_1, left_dir);
  digitalWrite(DIR_2, right_dir);
  analogWrite(SPEED_1, left_speed);
  analogWrite(SPEED_2, right_speed);
}

void forward(int speed){
  move(FORWARD_LEFT, speed, FORWARD_RIGHT, speed);
}

void backward(int speed){

}

void turn_left(int steepness){ //крутизна поворота

}

void turn_right(int steepness){ //крутизна поворота

}

void rotate_left(int ){ //

}

void rotate_right(int ){ //

}

void setup() {
  // put your setup code here, to run once:
  pinMode(DIR_1, OUTPUT);
  pinMode(DIR_2, OUTPUT);
  pinMode(SPEED_1, OUTPUT);
  pinMode(SPEED_2, OUTPUT);

  move(FORWARD_LEFT, 255, FORWARD_RIGHT, 0);
  delay(2000);
  move(FORWARD_LEFT, 0, FORWARD_RIGHT, 255);
  delay(2000);
  move(FORWARD_LEFT, 0, FORWARD_RIGHT, 0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
