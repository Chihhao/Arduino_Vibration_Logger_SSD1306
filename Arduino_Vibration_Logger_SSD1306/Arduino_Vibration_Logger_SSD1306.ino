//https://github.com/asukiaaa/MPU9250_asukiaaa
//https://github.com/jarzebski/Arduino-DS3231
//https://github.com/adafruit/Adafruit_SSD1306
//https://github.com/Marzogh/SPIMemory

#include "Adafruit_SSD1306.h"
#define OLED_RESET    4
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 //用64的話會記憶體不足
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include "MPU9250_asukiaaa.h"
MPU9250_asukiaaa mySensor;
float gX, gY, gZ;
float gShiftX=1.65;
float gShiftY=0.9;
float gShiftZ=0.1;
float gValue;

#include "DS3231.h"
DS3231 clock;
RTCDateTime dt;
char caRealTime[16];

#include "SPIMemory.h"
SPIFlash flash;
#define CAPA 134217728   //128M
#define ARRSZ 32
uint32_t gAdr=0;
char textAdr[ARRSZ];
#define TIME_INTERVAL 200  //每0.2秒記錄一筆資料
unsigned long lastWriteTime = 0;  

int iNowXPosition = 4;
int iLastY=0;

void setup() {
  Serial.begin(115200);
  clock.begin();
  
  flash.begin(MB(128));
  gAdr = findIdxOfFlash();
  delay(100);  
   
  //mySensor.beginAccel();
  mySensor.beginGyro();
  //mySensor.beginMag();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  initialScreen(); 
  display.display();

}

void loop() { 
  getSensorValue();
  //Serial.print(F("gValue: "));  Serial.println(gValue);
  
  if(iNowXPosition>4){
    display.drawLine(iNowXPosition-1 , iLastY, 
                     iNowXPosition , gValue, WHITE);     
  }
  else{
    display.drawPixel(iNowXPosition, gValue, WHITE);
  }
  iLastY = gValue;
  
  iNowXPosition++;  
  if(iNowXPosition>=128){
    iNowXPosition = 4;
    eraseVLine(iNowXPosition, 5);
    //initialScreen();
  }
  else{
    eraseVLine(iNowXPosition, 5);
  }
  
  display.display();  
  
  if(millis() - lastWriteTime > TIME_INTERVAL || 
     millis() < lastWriteTime){
       lastWriteTime = millis();
       writeLog();  
  }
  
}

void eraseVLine(int x, int w){
  int H = SCREEN_HEIGHT-1;
  int MH = SCREEN_HEIGHT/2;
  for(int i=0; i<w; i++){
    //display.drawFastVLine(x+i, 0, MH-1, BLACK);
    //display.drawFastVLine(x+i, MH+1, H-1, BLACK);
    display.drawFastVLine(x+i, 0, MH, BLACK);
    display.drawFastVLine(x+i, MH+1, MH, BLACK);
  }
}

void initialScreen(){
  int H = SCREEN_HEIGHT-1;
  int W = SCREEN_WIDTH-1;
  int MH = SCREEN_HEIGHT/2;
  
  display.clearDisplay();

  //水平線
  display.drawLine(3 , MH, W , MH, WHITE);

  //起點垂直線
  display.drawLine(3 , 0, 3 , H, WHITE);

  //刻度  
  for(int y=MH; y<H; y+=7){
    display.drawFastHLine(0, y, 3, WHITE);
  }  
  for(int y=MH; y>0; y-=7){
    display.drawFastHLine(0, y, 3, WHITE);
  }  
  
}

void getSensorValue(){
  gX=0;  gY=0;  gZ=0;
  if (mySensor.gyroUpdate() == 0) {
    gX = mySensor.gyroX();
    gY = mySensor.gyroY();
    gZ = mySensor.gyroZ();
  }
  gValue = (gX+gY+gZ)/3.0;
  gValue = map(gValue, -20, 20, 0, SCREEN_HEIGHT-1);
}

void getRealTime(){
  RTCDateTime dt = clock.getDateTime();
  int yy = dt.year;
  int mm = dt.month;
  int dd = dt.day;
  int hh = dt.hour;
  int mn = dt.minute;
  int ss = dt.second;
  sprintf(caRealTime, "%4d%02d%02d %02d%02d%02d", 
          yy, mm, dd, hh, mn, ss);
}

void writeLog(){
  char gValueString[10];
  dtostrf(gValue,3,1,gValueString);
  
  getRealTime();  
  char text[ARRSZ]; 
  sprintf(text, "%s %s", 
                caRealTime, gValueString);
  writeToFlash(text);  
}

void writeToFlash(char* str){
  if(gAdr>(CAPA-ARRSZ)){   
    myPrintHex(gAdr);
    Serial.println(F(" reset gAdr=0")); 
    gAdr=0; 
  }
  if(gAdr%4096==0){ erase4K(gAdr); }
  
  if (flash.writeCharArray(gAdr, str, ARRSZ, true)) {
    myPrintHex(gAdr);
    Serial.print(F(" W: ")); Serial.println(str);    
    Serial.print(textAdr); 
    
    /*
    Serial.print(F(" R: ")); 
    if (flash.readCharArray(gAdr, textAdr, ARRSZ)) {
      Serial.println(textAdr); }     
    else{
      Serial.println(F("read fail!")); }    
    */   
      
    gAdr+=ARRSZ;
  }
}

uint32_t findIdxOfFlash(){
  unsigned long adr=0;
  bool gotIndex=false;
  
  for(adr=0; adr<CAPA; adr+=ARRSZ){
    if(flash.readByte(adr)==0xFF){
      gotIndex=true;
      Serial.print(F("findIdx: "));
      myPrintHex(adr); Serial.println();      
      break;
    }
  }
  if(gotIndex==false){
    Serial.print("gotIndex==false");
    adr=0;    
  }
  return adr;  
}

void erase4K(unsigned long addr){
  myPrintHex(addr);
  if (flash.eraseSector(addr)) {
    Serial.println(F(" erase 4KB"));
  }
  else {
    Serial.println(F("Erasing sector failed"));
  } 
  //delay(10);
}

void myPrintHex(uint32_t inputInt32){
  if(inputInt32>0x0FFFFFFF){
    Serial.print("0x");
    Serial.print(inputInt32, HEX);
  }
  else if (inputInt32>0x00FFFFFF){
    Serial.print("0x0");
    Serial.print(inputInt32, HEX);
  }
  else if (inputInt32>0x000FFFFF){
    Serial.print("0x00");
    Serial.print(inputInt32, HEX);
  }
  else if (inputInt32>0x0000FFFF){
    Serial.print("0x000");
    Serial.print(inputInt32, HEX);
  }
  else if (inputInt32>0x00000FFF){
    Serial.print("0x0000");
    Serial.print(inputInt32, HEX);
  }
  else if (inputInt32>0x000000FF){
    Serial.print("0x00000");
    Serial.print(inputInt32, HEX);
  }
  else if (inputInt32>0x0000000F){
    Serial.print("0x000000");
    Serial.print(inputInt32, HEX);
  }
  else {
    Serial.print("0x0000000");
    Serial.print(inputInt32, HEX);
  }
  
}
