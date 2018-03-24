#include <Wire.h>

#define pos1 2
#define neg1 4
#define ena1 3

#define pos2 2
#define neg2 4
#define ena2 5

#define pos3 7
#define neg3 8
#define ena3 6

#define pos4 10
#define neg4 11
#define ena4 9

int spd[4] = {127, 127, 127, 127};
//full spd as default
//use this to adjust the speed of different motors.
//0 -> motor1; 1 -> motor2; 2 -> motor3; 3 -> motor4;

void action(void);
//input are motor number, pos, neg, ena pins numbers.

char cmd[] = "0000";
//define a global string to receive command from information center


int value1, value2, value3, value4, value5;
void setup()
{
  pinMode(pos1, OUTPUT);
  pinMode(neg1, OUTPUT);
  pinMode(ena1, OUTPUT);
  pinMode(pos2, OUTPUT);
  pinMode(neg2, OUTPUT);
  pinMode(ena2, OUTPUT);
  pinMode(pos3, OUTPUT);
  pinMode(neg3, OUTPUT);
  pinMode(ena3, OUTPUT);
  pinMode(pos4, OUTPUT);
  pinMode(neg4, OUTPUT);
  pinMode(ena4, OUTPUT);
  digitalWrite(pos1, LOW);
  digitalWrite(neg1, LOW);
  digitalWrite(ena1, LOW);
  digitalWrite(pos2, LOW);
  digitalWrite(neg2, LOW);
  digitalWrite(ena2, LOW);
  digitalWrite(pos3, LOW);
  digitalWrite(neg3, LOW);
  digitalWrite(ena3, LOW);
  digitalWrite(pos4, LOW);
  digitalWrite(neg4, LOW);
  digitalWrite(ena4, LOW);
  analogWrite(ena1, spd[0]); //this four lines can be in "setup" function
  analogWrite(ena2, spd[1]);
  analogWrite(ena3, spd[2]);
  analogWrite(ena4, spd[3]);
  Wire.begin();        //
  Serial.begin(9600);  // 串口輸出
}

void loop()
{
  Wire.requestFrom(2, 1);

  while (Wire.available())
  {
    value1 = Wire.read();
    Serial.print(value1);
    Serial.print("\t");
  }
  delay(200);
 if(value1!=value5){
  delay(1000);
 }
  Wire.requestFrom(2, 1);

  while (Wire.available())
  {
    value2 = Wire.read();
    Serial.print(value2);
    Serial.print("\t");
  }
  delay(200);

  Wire.requestFrom(2, 1);

  while (Wire.available())
  {
    value3 = Wire.read();
    Serial.print(value3);
    Serial.print("\t");
  }
  delay(200);

  Wire.requestFrom(2, 1);

  while (Wire.available())
  {
    value4 = Wire.read();
    Serial.print(value4);
    Serial.print("\t");
  }
  delay(200);

  Wire.requestFrom(2, 1);

  while (Wire.available())
  {
    value5 = Wire.read();
    Serial.print(value5);
    Serial.print("\t");
  }
  delay(200);
  // get 5 signals to prevent error signal.
  
  if (value1 == value2 && value1 == value3 && value1 == value4 && value1 == value5) {
    cmd[0] = '0' + value1 / 100;
    cmd[1] = '0' + (value1 - (value1 / 100) * 100) / 10;
    cmd[2] = '0' + value1 - (value1 / 10) * 10;
    cmd[3] = '0';
    Serial.println( );
  }
  else {
    cmd[0]='0'; 
    cmd[1]='0'; 
    cmd[2]='0'; 
    cmd[3]='0'; 
  }
  Serial.println(cmd);
  action();
}

void action(void) {

  digitalWrite(pos1, LOW);
  digitalWrite(neg1, LOW);
  digitalWrite(pos2, LOW);
  digitalWrite(neg2, LOW);
  digitalWrite(pos3, LOW);
  digitalWrite(neg3, LOW);
  digitalWrite(pos4, LOW);
  digitalWrite(neg4, LOW);

  if (cmd[2] == '1') {
    digitalWrite(pos1, HIGH);
    digitalWrite(pos2, HIGH);
    return;
  }
  else if (cmd[2] == '2') {
    digitalWrite(neg1, HIGH);
    digitalWrite(neg2, HIGH);
    return;
  }
  else if (cmd[0] == '0' && cmd[1] == '0') {
    return;
  }
  else if (cmd[0] == '0' && cmd[1] == '1') {
    digitalWrite(pos3, HIGH);
    digitalWrite(pos4, HIGH);
    return;
  }
  else if (cmd[0] == '0' && cmd[1] == '2') {
    digitalWrite(neg3, HIGH);
    digitalWrite(neg4, HIGH);
    return;
  }
  else if (cmd[0] == '1' && cmd[1] == '0') {
    digitalWrite(neg3, HIGH);
    digitalWrite(pos4, HIGH);
    return;
  }
  else if (cmd[0] == '1' && cmd[1] == '1') {
    digitalWrite(pos4, HIGH);
    return;
  }
  else if (cmd[0] == '1' && cmd[1] == '2') {
    digitalWrite(neg3, HIGH);
    return;
  }
  else if (cmd[0] == '2' && cmd[1] == '0') {
    digitalWrite(pos3, HIGH);
    digitalWrite(neg4, HIGH);
    return;
  }
  else if (cmd[0] == '2' && cmd[1] == '1') {
    digitalWrite(pos3, HIGH);
    return;
  }
  else if (cmd[0] == '2' && cmd[1] == '2') {
    digitalWrite(neg4, HIGH);
    return;
  }
}
