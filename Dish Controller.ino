void(* resetFunc) (void) = 0;  // declare reset fuction at address 0

#include "Wire.h"
#include <SoftwareSerial.h>
#include <MPU6050_light.h>

#define Mode  7
#define RX    13
#define TX    12
#define M1a   A0
#define M1b   A1
#define M2a   A3
#define M2b   A2

MPU6050 mpu1(Wire);
MPU6050 mpu2(Wire);
unsigned long timer = 0;

SoftwareSerial mySerial(RX, TX); // RX, TX

float pos[] = {0.0,0.0,0.0};

// Communication Variables
int rChar = 0;
String ReceiveMsg = "",btn = "";
String Position = "";


long Time = 0, th = 200;

void setup(){
  pinMode(Mode, OUTPUT);
  pinMode(M1a, OUTPUT);
  pinMode(M1b, OUTPUT);
  pinMode(M2a, OUTPUT);
  pinMode(M2b, OUTPUT);
  digitalWrite(Mode,LOW);
  Serial.begin(9600);
  mySerial.begin(9600);
  Wire.begin();
  Wire.setWireTimeout(3000 /* us  */, true /* reset_on_Timeout*/);
  mpu1.setAddress(0x68);
  mpu2.setAddress(0x69);
  
  byte status1 = mpu1.begin();
  byte status2 = mpu2.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status1);
  Serial.print('\t');
  Serial.println(status2);
  
  // Initialize MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
//  mpu1.calcOffsets(); // gyro and accelero
//  mpu2.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
}

void loop()
{
  mpu1.update();
  mpu2.update();  
  
  if((millis()-timer)>10){ // print data every 10ms
    // Calculate Pitch, Roll and Yaw
    pos[1] = mpu2.getAngleZ();
    pos[0] = mpu1.getAngleX();
    Serial.print(pos[0]);
    Serial.print(", ");
    Serial.print(pos[1]);
    Serial.print(", ");
    
//    Serial.print(pos[0]);
//    Serial.print(", ");
//    Serial.print(pos[1]);  
//    Serial.print(", ");
    Serial.println(btn);
    timer = millis();  
  }

  CheckSerial();
  motorDrive();
}


void SendMsg(String Msg){
  digitalWrite(Mode,HIGH);
  delay(10);
  mySerial.println(Msg);
  delay(10);
  digitalWrite(Mode,LOW);
}

void CheckSerial(){
  if (mySerial.available()) {
    char In = mySerial.read();
    if((In >= '0') && (In <= '9')){ReceiveMsg = In;}
//    Serial.println(ReceiveMsg);
    if(ReceiveMsg == "1")
      btn = "R";
    else if(ReceiveMsg == "2")
      btn = "U";
    else if(ReceiveMsg == "3")
      btn = "D";
    else if(ReceiveMsg == "4")
      btn = "L";
    else if(ReceiveMsg == "5")
      btn = "S";
    else if(ReceiveMsg == "0")
      btn = "";
    ReceiveMsg= "";
  }
  // Get Serial Data;
  if(btn == "S"){
    for(int a = 0; a < 3; a++)  
      pos[a] = 0;
    timer = millis();
    resetFunc(); //call reset
  }
  // Send Position with Delay
  if(millis() > (Time + th)){
    Position = "*";
    for(int a = 0; a < 3; a++){
      Position += String((int)pos[a]) + ",";
    }
    SendMsg(Position);
    Time = millis();
  }
}

void motorDrive(){
  if(btn == "U"){
    digitalWrite(M1a,HIGH);
    digitalWrite(M1b,LOW); 
  }
  else if(btn == "D"){
    digitalWrite(M1b,HIGH);
    digitalWrite(M1a,LOW);
  }
  else{
    digitalWrite(M1a,HIGH);
    digitalWrite(M1b,HIGH);
  }
  if(btn == "L"){
    digitalWrite(M2a,HIGH);
    digitalWrite(M2b,LOW);
  }
  else if(btn == "R"){
    digitalWrite(M2b,HIGH);
    digitalWrite(M2a,LOW);
  }
  else{
    digitalWrite(M2a,HIGH);
    digitalWrite(M2b,HIGH);
  }
}
