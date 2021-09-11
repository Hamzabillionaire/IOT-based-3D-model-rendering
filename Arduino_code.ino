#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <String.h>
#include <stdio.h> 

//const char* ssid = "StormFiber";
//const char* password = "03400908256";

const char* ssid = "PTCL-BB";
const char* password = "11223344";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
//char  replyPacket1[] = " kha faizan";


//#include <SPI.h>
//#include <WiFi.h>
//WiFiServer server(80);

//const char* ssid="Note 7 lite";
//const char* password = "faixy123";





int ledPin = 13;

//initializations
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

// MPU9250 Slave Device Address
const uint8_t MPU9250SlaveAddress = 0x68;

// Pins for serial data
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor of accelerometer and gyroscope 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU9250 few configuration register addresses
const uint8_t MPU9250_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU9250_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU9250_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU9250_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU9250_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU9250_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU9250_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU9250_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU9250_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU9250_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU9250_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

void setup() {
  Serial.begin(115200);
  Wire.begin(sda, scl);
  MPU9250_Init();



 // Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);


  /*pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);

  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.begin(ssid,password);

  Serial.println();
  Serial.print("Connecting");

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }
  
  digitalWrite(ledPin,HIGH);
  Serial.println();

  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );*/
  
}



void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU9250
void MPU9250_Init(){
  delay(150);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU9250SlaveAddress, MPU9250_REGISTER_USER_CTRL, 0x00);
}
void loop() {
  float Ax, Ay, Az, T, Gx, Gy, Gz;
  
  Read_RawValue(MPU9250SlaveAddress, MPU9250_REGISTER_ACCEL_XOUT_H);
  
  //divide each with their sensitivity scale factor
  Ax = (float)AccelX/AccelScaleFactor;
  Ay = (float)AccelY/AccelScaleFactor;
  Az = (float)AccelZ/AccelScaleFactor;
  T = (float)Temperature/340+36.53; //temperature formula
  Gx = (float)GyroX/GyroScaleFactor;
  Gy = (float)GyroY/GyroScaleFactor;
  Gz = (float)GyroZ/GyroScaleFactor;

// Calculating the Roll and the Pitch from the accelerometer data
  accAngleX = (atan(Ay / sqrt(pow(Ax, 2) + pow(Az, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58)
  accAngleY = (atan(-1 * Ax / sqrt(pow(Ay, 2) + pow(Az, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)

//reading gyroscope data
  previousTime = currentTime;        // The Previous time is stored before the actual time read
  currentTime = millis();            // Current time
  elapsedTime = (currentTime - previousTime) / 1000; // Dividing by 1000 to get seconds

// Correcting the outputs with the calculated error values
  Gx = Gx - 1.94; // GyroErrorX ~(1.94)
  Gy = Gy ; // GyroErrorY ~
  Gz = Gz ; // GyroErrorZ ~ 

// Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by seconds to get the angle in degrees
  gyroAngleX = gyroAngleX + Gx * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + Gy * elapsedTime;
  yaw =  yaw + Gz * elapsedTime;
// Complementary filter - combining the acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
// Print the values on the serial monitor
//  Serial.print(roll);
//  Serial.print(",");
//  Serial.print(pitch);
//  Serial.print(",");
//  Serial.println(yaw);
    
  Serial.println("Roll!");
  char replyPacket[30];
  gcvt(roll, 30, replyPacket);
  Serial.println(replyPacket);
  Serial.println("Pitch!");
  char replyPacket2[30];
  gcvt(pitch, 30, replyPacket2);
  Serial.println(replyPacket2);
  
  
  Serial.println("Yaw!");
  char replyPacket3[30];
  gcvt(yaw, 15, replyPacket3);
  Serial.println(replyPacket3);
  Udp.beginPacket("192.168.43.28", 4210);
   //Serial.println("Roll1");
   Udp.write(replyPacket);
  // Serial.println("Pitch1!");
   Udp.write(replyPacket2);
   //Serial.println("Yaw1!");
   Udp.write(replyPacket3);
   Udp.endPacket();
  
  int packetSize = Udp.parsePacket();
  
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.write(",");
    Udp.write(replyPacket2);
    Udp.write(",");
    Udp.write(replyPacket3);
    Udp.endPacket();
  }


  //wifi


 /* WiFiClient client = server.available();
  if (client) {

    if (client.connected()) {
      Serial.println("Connected to client");
    }*/
/*
  Serial.print("Ax: "); Serial.print(Ax);
  Serial.print(" Ay: "); Serial.print(Ay);
  Serial.print(" Az: "); Serial.print(Az);
  Serial.print(" T: "); Serial.print(T);
  Serial.print(" Gx: "); Serial.print(Gx);
  Serial.print(" Gy: "); Serial.print(Gy);
  Serial.print(" Gz: "); Serial.println(Gz);
*/

//calculating error
/*
  while (c<200){
    float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
AccErrorX = AccErrorX + ((atan((Ay) / sqrt(pow((Ax), 2) + pow((Az), 2))) * 180 / PI));
AccErrorY = AccErrorY + ((atan(-1 * (Ax) / sqrt(pow((Ay), 2) + pow((Az), 2))) * 180 / PI));
c++;
  }
      AccErrorX = AccErrorX / 200;
      AccErrorY = AccErrorY / 200;
      c = 0;
      // Read gyro values 200 times
      while (c < 200) {
        // Sum all readings
        GyroErrorX = GyroErrorX + (Gx);
        GyroErrorY = GyroErrorY + (Gy);
        GyroErrorZ = GyroErrorZ + (Gz);
        c++;
      }
      //Divide the sum by 200 to get the error value
      GyroErrorX = GyroErrorX / 200;
      GyroErrorY = GyroErrorY / 200;
      GyroErrorZ = GyroErrorZ / 200;
      // Print the error values on the Serial Monitor
      Serial.print("AccErrorX: ");
      Serial.println(AccErrorX);
      Serial.print("AccErrorY: ");
      Serial.println(AccErrorY);
      Serial.print("GyroErrorX: ");
      Serial.println(GyroErrorX);
      Serial.print("GyroErrorY: ");
      Serial.println(GyroErrorY);
      Serial.print("GyroErrorZ: ");
      Serial.println(GyroErrorZ);
    
  */
  delay(10);
}
//}
