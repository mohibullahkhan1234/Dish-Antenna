#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define Mode  11
#define RX    3
#define TX    2


SoftwareSerial mySerial(RX, TX); // RX, TX
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);



bool Receive = false;
String ReceiveMsg = "";
String pos[] = {"","",""};
int Seperator[] = {0, 0, 0};
int sep = 0;
int btn = 0,p_Btn=0;
long StartTime = 0;
long ST_Th = 5000;
int rst = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  lcd.begin(16, 2);
  pinMode(Mode, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  digitalWrite(Mode,LOW);
  lcd.clear();
}

void loop() {
  while (mySerial.available()) {
    if (mySerial.available()) {
      char In = mySerial.read();
      ReceiveMsg += In;
      Receive = true;
    }
    delay(5);
  }
  if(Receive){
    if(StartTime == 0){
      StartTime = millis();
    }
    if(millis() > (StartTime + ST_Th)){  
      if(rst < 1){
        lcd.clear();  
//        SendMsg("*5*");
        rst++;
      }
      for(int i = 0 ; i<3 ; i++){
        if(i == 0)
          Seperator[i] = ReceiveMsg.indexOf(",");
        else
          Seperator[i] = ReceiveMsg.indexOf(",",Seperator[i-1]+1);
      }
      pos[0] = ReceiveMsg.substring(ReceiveMsg.indexOf("*")+1,Seperator[0]);
      pos[1] = ReceiveMsg.substring(Seperator[0]+1,Seperator[1]);
      pos[2] = ReceiveMsg.substring(Seperator[1]+1,Seperator[2]);
      for(int a = 0 ; a < 3 ; a++){
        if(pos[a].length() == 1){
           pos[a] = "   " + pos[a];
        }
        else if(pos[a].length() == 2){
           pos[a] = "  " + pos[a];
        }
        else if(pos[a].length() == 3){
           pos[a] = " " + pos[a];
        }
      }
      ReceiveMsg = ReceiveMsg.substring(ReceiveMsg.indexOf("*")+1,Seperator[2]);
      btn = getButton();
      if(btn != p_Btn){
        SendMsg("*" + String(btn)+"*");
        p_Btn = btn;
      }
      Serial.print(ReceiveMsg);
      Serial.print(" Pitch = ");
      Serial.print(pos[0]);
      Serial.print(" Roll = ");
      Serial.print(pos[1]);  
      Serial.print(" Yaw = ");
      Serial.print(pos[2]);
      Serial.print(" Btn = ");
      Serial.println(btn);
      lcd.setCursor(0,0);
      lcd.print("[SLOW]");  
      lcdprint(7, 0, " El[");
      lcdprint(15 - pos[0].length(), 0, pos[0]);
      lcdprint(15, 0, "]");
      if(btn == 1){
        lcdprint(0, 1, "[RIGHT]");  
      }
      else if(btn == 2){
        lcdprint(0, 1, "[ UP ] ");  
      }
      else if(btn == 3){
        lcdprint(0, 1, "[DOWN] ");  
      }
      else if(btn == 4){
        lcdprint(0, 1, "[LEFT] ");  
      }
      else{
        lcdprint(0, 1, "[STOP] ");  
      }
      lcdprint(7, 1, " AZ[");
      lcdprint(15 - pos[1].length(), 1, pos[1]);
      lcdprint(15, 1, "]");
      ReceiveMsg = "";
      Receive = false;  
    }
    else{
      lcd.clear();
      lcdprint(0, 0, "Initializing....");
    }
  }
}

void SendMsg(String Msg){
  digitalWrite(Mode,HIGH);
  delay(10);
  mySerial.print(Msg);
  delay(10);
  digitalWrite(Mode,LOW);
}

int getButton(){
  int val=analogRead(A0);
  int btn = 0;
//  Serial.print(val);
//  Serial.print("   ");
  
  if((val>=0&&val<=50)||!digitalRead(A1))
    btn = 1;
  else if((val>=50&&val<=150)||!digitalRead(A2))
    btn = 2;
  else if((val>=150&&val<=350)||!digitalRead(A3))
    btn = 3;
  else if((val>=350&&val<=500)||!digitalRead(A4))
    btn = 4;
  else if((val>=500&&val<=750)||!digitalRead(A5))
    btn = 5;
  
//  Serial.println(btn);
  return btn;
}

void lcdprint(int c, int r, String Msg){
  lcd.setCursor(c,r);
  lcd.print(Msg);
}
