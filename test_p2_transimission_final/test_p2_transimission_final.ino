// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69

//SCL-A5 SDA-A4 ADO(hand)-7 ADO(finger)-8 GND-GND VC-3.3v
//VSS,RW-GND VDD-5v V0-10 RS-7 E-6 D4,D5,D6,D7-5,4,3,2
//A(VCC)-3.3v B(CSN)-9 C(MOSI)-11 D-NULL E(GND)-GND F(CE)-8 G(SCK)-13 H(MISO)-12

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <LiquidCrystal.h>
#include <SPI.h>
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"

int value;

#define luminance 10

MPU6050 accelgyro(0x68);
MPU6050 accelgyro2(0x69);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int16_t ax_hand, ay_hand, az_hand, ax_finger, ay_finger, az_finger;
int16_t gx_hand, gy_hand, gz_hand, gx_finger, gy_finger, gz_finger;
char cmd[] = "ABCD";
bool blinkState = false;

void sensor_get();
void serial_print();
void lcd_print();
void function(int x1, int y1, int z1, int x2, int y2, int z2);
char isaction(int x1, int y1, int z1, int x2, int y2, int z2);
char UoD(int x1, int y1, int z1, int x2, int y2, int z2);
char FoB(int x1, int y1, int z1, int x2, int y2, int z2);
char LoR(int x1, int y1, int z1, int x2, int y2, int z2);
void send2boat();
void setup() {

  delay(1000);
  Wire.begin();
  pinMode(luminance, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Your command:");
  analogWrite(luminance, 140);

  Mirf.cePin = 8;                //设置CE引脚为D8
  Mirf.csnPin = 9;        //设置CE引脚为D9
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();

  Mirf.setRADDR((byte *)"SSDYW"); //设置自己的地址（发送端地址），使用5个字符
  Mirf.payload = sizeof(value);
  Mirf.channel = 90;              //设置所用信道
  Mirf.config();

  Serial.begin(38400);
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  delay(1000);
  accelgyro2.initialize();

  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

}
void loop() {

  sensor_get(); //get value
  function(ax_hand, ay_hand, az_hand, ax_finger, ay_finger, az_finger);
  serial_print(); //print in the serial
  Serial.print('1');
  lcd_print();    // print in the lcd
  Serial.println('2');
  send2boat();
  Serial.println('3');

  delay(200);//time control
}

void send2boat() {
  Mirf.setTADDR((byte *)"WYDSS");//设置接收端地址
  value=0;
  if(cmd[0]=='L') value+=100;
  else if(cmd[0]=='R') value+=200;
  if (cmd[1]=='F') value+=10;
  else if(cmd[1]=='B') value+=20;
  if (cmd[2]=='U') value+=1;
  else if(cmd[2]=='D') value+=2;
  Serial.println(value);
  Mirf.send((byte *) &value);                //
  while(Mirf.isSending()) delay(1);         //直到发送成功，退出循环
}

void sensor_get() {
  accelgyro.getMotion6(&ax_hand, &ay_hand, &az_hand, &gx_hand, &gy_hand, &gz_hand);
  accelgyro2.getMotion6(&ax_finger, &ay_finger, &az_finger, &gx_finger, &gy_finger, &gz_finger);
}

void serial_print() {
  Serial.print("hand:");
  Serial.print("a/g:\t");
  Serial.print(ax_hand);
  Serial.print("\t");
  Serial.print(ay_hand);
  Serial.print("\t");
  Serial.print(az_hand);
  Serial.println("\t");

  Serial.print("Finger:");
  Serial.print("a/g:\t");
  Serial.print(ax_finger);
  Serial.print("\t");
  Serial.print(ay_finger);
  Serial.print("\t");
  Serial.println(az_finger);

  Serial.print("Your command is:");
  Serial.print(cmd[0]);
  Serial.print(cmd[1]);
  Serial.print(cmd[2]);
  Serial.print(cmd[3]);
  Serial.println(cmd[4]);
}

void lcd_print() {
  lcd.setCursor(0, 1);
  lcd.print(cmd[0]);
  lcd.print(cmd[1]);
  lcd.print(cmd[2]);
  lcd.print(cmd[3]);
  lcd.print(" t=");
  lcd.print(millis() / 1000);
  lcd.print("  +1s    ");

}

void function(int x1, int y1, int z1, int x2, int y2, int z2) {// x1, y1, z1 is the data of the finger sensor; x2, y2, z2 is the data of the handback sensor
  static char value[5];
  if (isaction(x1, y1, z1, x2, y2, z2) == '0') { // isaction(...) detects whether there is action. '0' for no action
    value[0] = '0'; // set all to '0'
    value[1] = '0';
    value[2] = '0';
    value[3] = '0'; // the last (No.4) digit controls whether it will move. '0' for static.
    value[4] = '\0'; // protect the string
    for (int i = 0; i < 5; i++)
      cmd[i] = value[i];
    return ;// function returns.
  }
  else if (isaction(x1, y1, z1, x2, y2, z2) == '1') {
    value[4] = '\0'; // protect the string
    value[3] = '1'; // the last digit is '1', means there is action
    if (UoD(x1, y1, z1, x2, y2, z2) == 'U') { // UoD(...) detects whether it will move up or down. 'U' for up, 'D'for down, '0' for static
      value[2] = 'U';
      value[0] = '0'; // don't move left or right
      value[1] = '0'; // don't move forward or backward
      for (int i = 0; i < 5; i++)
        cmd[i] = value[i];
      return;// when moving up or down, the airship don't move forward/ backward/ left/ right.
    }
    else if (UoD(x1, y1, z1, x2, y2, z2) == 'D') {
      value[2] = 'D';
      value[0] = '0';
      value[1] = '0';
      for (int i = 0; i < 5; i++)
        cmd[i] = value[i];
      return;
    }
    else { // the ship doesn't move up or down
      value[2] = '0'; // set the digit to '0'
      if (FoB(x1, y1, z1, x2, y2, z2) == 'F') value[1] = 'F'; // forward
      else if (FoB(x1, y1, z1, x2, y2, z2) == 'B') value[1] = 'B'; // backward
      else if (FoB(x1, y1, z1, x2, y2, z2) == '0') value[1] = '0'; // static
      if (LoR(x1, y1, z1, x2, y2, z2) == 'L') value[0] = 'L'; // left
      else if (LoR(x1, y1, z1, x2, y2, z2) == 'R') value[0] = 'R'; // right
      else if (LoR(x1, y1, z1, x2, y2, z2) == '0') value[0] = '0'; // static
      for (int i = 0; i < 5; i++)
        cmd[i] = value[i];
      return;
    }
  }
}

/**************************************************************/
// the following functions are position functions of hands. Please refer to photos.

char UoD(int x1, int y1, int z1, int x2, int y2, int z2) {
  //|x1|,|x2| are large; y1,y2,z1,z2 are small;
  //see photos
  if (x1 >= 10000 && x2 >= 10000) return 'U'; // up
  else if (x1 >= 10000 && x2 <= -10000) return 'D';// down
  else return '0';// not move up or down
}

char FoB(int x1, int y1, int z1, int x2, int y2, int z2) {
  if (x1 < -8000 || x1 > 8000) return '0'; // axis of hand won't be horizontal
  else if (z2 < 2000 && z2 > -2000) return '0'; // palm won't be vertical
  if (y1 < -13000 || y1 > 5000) return '0'; // axis of hand won't be horizontal
  else if (((z1 > 0 && z2 > 0) || (z1 < 0 && z2 < 0)) && z1 - z2 <= 3000 && z2 - z1 <= 3000) return 'F'; // palm is open --> finger and handback is in a line the same direction.
  else if (((z1 > 0 && z2 < 0) || (z1 < 0 && z2 > 0)) && z1 + z2 <= 5000 && z1 + z2 >= -4000 || (z1 < -10000 && z1 + z2 > 0) || (z1 > 17000 && z2 < -10000)) return 'B'; // palm is closed --> finger and handback is in a line opposite direction.
  else return '0';// not moving forward or backward
}

char LoR(int x1, int y1, int z1, int x2, int y2, int z2) {
  if (y1 < -11000 || y1 > 5000) return '0'; // axis of hand won't be horizontal
  else if (z1 >= 10000) return 'R'; // palm is upwards
  else if (z1 <= -12000) return 'L'; // palm is downwards
  else return '0';// not turning
}

char isaction(int x1, int y1, int z1, int x2, int y2, int z2) {
  if (UoD(x1, y1, z1, x2, y2, z2) == '0' &&
      FoB(x1, y1, z1, x2, y2, z2) == '0' &&
      LoR(x1, y1, z1, x2, y2, z2) == '0') return '0'; // not up or down or left or right or forward or backward.
  else return '1';
}

