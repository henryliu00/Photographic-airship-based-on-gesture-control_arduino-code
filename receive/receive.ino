#include <SPI.h>  
#include <Mirf.h>  
#include <nRF24L01.h>  
#include <MirfHardwareSpiDriver.h>  
  #include <Wire.h>
int value;  
  
void setup()  
{  
   Wire.begin(2);  
    Wire.onRequest(motorcmd);

  Serial.begin(38400);  
  Mirf.spi = &MirfHardwareSpi;  
  Mirf.init();  
  Mirf.csnPin=10;
  Mirf.cePin=9;
  Mirf.setRADDR((byte *)"WYDSS"); //设置自己的地址（接收端地址），使用5个字符  
  Mirf.payload = sizeof(value);     
  Mirf.channel = 90;   //设置使用的信道  
  Mirf.config();   
  Serial.println("Listening...");  //开始监听接收到的数据  
  
}  
  
void loop()  
{  
  if(Mirf.dataReady()) {  //当接收到程序，便从串口输出接收到的数据  
    Mirf.getData((byte *) &value);  
    Serial.print("Got data: ");  
    Serial.println(value);  
  }
  Wire.onRequest(motorcmd);
  delay(200);  
}  
void motorcmd()
{
  Wire.write(value);
 // Wire.write("Hello "); // 送出 6 個 char 給 IIC 上的主人
}
