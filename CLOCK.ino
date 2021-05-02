
#include <ArduinoJson.h>

#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

const char ssid[] = "HUAWEI-LDXTN6_HiLink";  //WIFI名称 修改这2个就可以了
const char pass[] = "Huaweiapp5566";  //WIFI密码

#include "font/ZdyLwFont_20.h"
#include "font/FxLED_32.h"

#include "img/pangzi/i0.h"
#include "img/pangzi/i1.h"
#include "img/pangzi/i2.h"
#include "img/pangzi/i3.h"
#include "img/pangzi/i4.h"
#include "img/pangzi/i5.h"
#include "img/pangzi/i6.h"
#include "img/pangzi/i7.h"
#include "img/pangzi/i8.h"
#include "img/pangzi/i9.h"

#include "img/temperature.h"
#include "img/humidity.h"
#include "img/watch_top.h"
#include "img/watch_bottom.h"

#include <TFT_eSPI.h> 
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  
TFT_eSprite clk = TFT_eSprite(&tft);


#include <TJpg_Decoder.h>

uint32_t targetTime = 0;   
byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

uint16_t bgColor = 0xFFFF;
String cityCode = "101320101";  

static const char ntpServerName[] = "ntp6.aliyun.com";
const int timeZone = 10;     


WiFiUDP Udp;
unsigned int localPort = 8000;

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
String num2str(int digits);
void sendNTPpacket(IPAddress &address);


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  // Return 1 to decode next block
  return 1;
}

byte loadNum = 6;
void loading(byte delayTime){
  clk.setColorDepth(8);
  
  clk.createSprite(200, 50);
  clk.fillSprite(0x0000);

  clk.drawRoundRect(0,0,200,16,8,0xFFFF);
  clk.fillRoundRect(3,3,loadNum,10,5,0xFFFF);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_GREEN, 0x0000); 
  clk.drawString("Connecting to WiFi",100,40,2);
  clk.pushSprite(20,110);
  clk.deleteSprite();
  loadNum += 1;
  if(loadNum>=194){
    loadNum = 194;
  }
  delay(delayTime);
}

void setup()
{
  Serial.begin(9600);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(0x0000);
  tft.setTextColor(TFT_BLACK, bgColor);

  targetTime = millis() + 1000; 

  Serial.print("Connecting WIFI ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    for(byte n=0;n<10;n++){ 
      loading(50);
    }
  }
  while(loadNum < 194){ 
    loading(1);
  }

  Serial.print("Local IP： ");
  Serial.println(WiFi.localIP());

  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  TJpgDec.drawJpg(0,0,watchtop, sizeof(watchtop));
  TJpgDec.drawJpg(0,220,watchbottom, sizeof(watchbottom));

  

  tft.setViewport(0, 20, 240, 200);
  tft.fillScreen(0x0000);
  tft.fillRoundRect(0,0,240,200,5,bgColor);
  //tft.resetViewport();


  tft.drawFastHLine(0,34,240,TFT_BLACK);
  
  tft.drawFastVLine(150,0,34,TFT_BLACK);
  
  tft.drawFastHLine(0,166,240,TFT_BLACK);
  
  tft.drawFastVLine(60,166,34,TFT_BLACK);
  tft.drawFastVLine(160,166,34,TFT_BLACK);


  
//  TJpgDec.drawJpg(161,171,temperature, sizeof(temperature));  
//  TJpgDec.drawJpg(159,130,humidity, sizeof(humidity));  
  
}

time_t prevDisplay = 0; 
unsigned long weaterTime = 0;

void loop(){
  
  if (now() != prevDisplay) {
    prevDisplay = now();
    digitalClockDisplay();
  }

  
  scrollBanner();
  imgAnim();
}

String scrollText[6];
//int scrollTextWidth = 0;

int currentIndex = 0;
int prevTime = 0;
TFT_eSprite clkb = TFT_eSprite(&tft);

void scrollBanner(){
  if(millis() - prevTime > 2500){ 

    if(scrollText[currentIndex]){
  
      clkb.setColorDepth(8);
      clkb.loadFont(ZdyLwFont_20);
      
      for(int pos = 24; pos>0 ; pos--){
        scrollTxt(pos);
      }
      
      clkb.deleteSprite();
      clkb.unloadFont();
  
      if(currentIndex>=5){
        currentIndex = 0;  
      }else{
        currentIndex += 1;  
      }
      //Serial.println(currentIndex);
      
    }
    prevTime = millis();
  }
}

void scrollTxt(int pos){
    clkb.createSprite(148, 24); 
    clkb.fillSprite(bgColor);
    clkb.setTextWrap(false);
    clkb.setTextDatum(CC_DATUM);
    clkb.setTextColor(TFT_BLACK, bgColor); 
    clkb.drawString(scrollText[currentIndex],74,pos+12);
    clkb.pushSprite(2,4);
}

void imgAnim(){
  int x=80,y=94,dt=30;

  TJpgDec.drawJpg(x,y,i0, sizeof(i0));
  delay(dt);
  TJpgDec.drawJpg(x,y,i1, sizeof(i1));
  delay(dt);
  TJpgDec.drawJpg(x,y,i2, sizeof(i2));
  delay(dt);
  TJpgDec.drawJpg(x,y,i3, sizeof(i3));
  delay(dt);  
  TJpgDec.drawJpg(x,y,i4, sizeof(i4));
  delay(dt);  
  TJpgDec.drawJpg(x,y,i5, sizeof(i5));
  delay(dt);  
  TJpgDec.drawJpg(x,y,i6, sizeof(i6));
  delay(dt);  
  TJpgDec.drawJpg(x,y,i7, sizeof(i7));
  delay(dt);  
  TJpgDec.drawJpg(x,y,i8, sizeof(i8));
  delay(dt);  
  TJpgDec.drawJpg(x,y,i9, sizeof(i9));
  delay(dt);  

}

void digitalClockDisplay()
{
  
  clk.setColorDepth(8);


  clk.createSprite(140, 48);
  clk.fillSprite(bgColor);
  //clk.loadFont(FxLED_48);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor);
  clk.drawString(hourMinute(),70,24,7); 
  //clk.unloadFont();
  clk.pushSprite(28,40);
  clk.deleteSprite();
  

  clk.createSprite(40, 32);
  clk.fillSprite(bgColor);
  
  clk.loadFont(FxLED_32);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor); 
  clk.drawString(num2str(second()),20,16);
  
  clk.unloadFont();
  clk.pushSprite(170,60);
  clk.deleteSprite();



  clk.loadFont(ZdyLwFont_20);
  clk.createSprite(58, 32);
  clk.fillSprite(bgColor);


  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor);
  clk.drawString(week(),29,16);
  clk.pushSprite(1,168);
  clk.deleteSprite();
  

  clk.createSprite(98, 32);
  clk.fillSprite(bgColor);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_BLACK, bgColor);  
  clk.drawString(monthDay(),49,16);
  clk.pushSprite(61,168);
  clk.deleteSprite();
  
  
  clk.unloadFont();

}


String week(){
  String wk[7] = {"Sun","Mon","Tue","Wed","Thur","Fir","Sat"};
  String s =  wk[weekday()-1];
  return s;
}


String monthDay(){
  String s = String(month());
  s = s + "/" + day() + "/";
  return s;
}

String hourMinute(){
  String s = num2str(hour());
  s = s + ":" + num2str(minute());
  return s;
}

String num2str(int digits)
{
  String s = "";
  if (digits < 10)
    s = s + "0";
  s = s + digits;
  return s;
}

void printDigits(int digits)
{
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  //Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  //Serial.print(ntpServerName);
  //Serial.print(": ");
  //Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      //Serial.println(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; 
}


void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
