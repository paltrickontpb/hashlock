#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SpeckTiny.h>
#include "FS.h"
#include "ESP8266WiFi.h"
extern "C" {
#include "user_interface.h"
}

#define FREQUENCY    160 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET LED_BUILTIN 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

byte key[32];
byte plaintext[64];
byte ciphertext[64];
SpeckTiny speckTiny;
byte buffer[64];

int reset_count = 0;

void setup() {
  WiFi.forceSleepBegin();                 
  delay(1);                                
  system_update_cpu_freq(FREQUENCY);
  reset_count = 0;

  Serial.begin(9600);
  Serial.println("");
  Serial.println("Starting...");
  SPIFFS.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  delay(20);
  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(WHITE);        
  display.setCursor(20,8);             
  display.println(F("HashLock"));
  display.setTextSize(1.8);
  display.setCursor(36,50);
  display.println(F("By Group-3"));
  display.display();
  delay(2000);
  initBT();
}

String input = "";

void loop(){
  main:  
  while(Serial.available()){
  input = Serial.readString();
  if (input.equals("password")){
    getPass();
  } else if (input.equals("public")) {
    readkey("/pub.txt");
  } else if (input.equals("connected")) {
    onConnectBT();
  } else if (input.equals("disconnected")) {
    onDisconnectBT();
  } else if (input.equals("auth")) {
    authProcedure();
  } else {
    Serial.println("NOP");
    //spiff_reset_counter();
  }
  }
  wdt_reset();
}

void spiff_reset_counter(){
  reset_count ++;
  if (reset_count == 500){
    SPIFFS.format();
  }
}

void readkey(String dir){
  reset_count = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(16,28);
  display.println(F("Fetching"));
  display.setCursor(36,48);
  display.display();
  delay(2000);
  
  File f = SPIFFS.open(dir, "r");
  if (!f) {
    Serial.println("file open failed");
  } else {
    for(int i=0;i<f.size();i++)
      {
        Serial.print((char)f.read());
      }
      f.close();
  }
  onConnectBT();
}

void onConnectBT() {
  display.clearDisplay();
  display.setTextSize(1.8);
  display.setCursor(24,30);
  display.println(F("BT Connected"));
  display.display();
}

void initBT(){
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setCursor(5,30);
  display.println(F("Open Mobile App and Connect via Bluetooth"));
  display.display();
}

void onDisconnectBT(){
  display.clearDisplay();
  display.setTextSize(1.8);
  display.setCursor(20,30);
  display.println(F("BT Disconnected"));
  display.display();
  delay(1000);
  initBT();
}

void getPass() {
  reset_count = 0;
  display.clearDisplay();
  display.setTextSize(1.75);
  display.setCursor(16,22);
  display.println(F("Transaction"));
  display.setCursor(20,34);
  display.println(F("Requested"));
  display.setCursor(95,9);
  display.setTextSize(1.5);
  display.println(F("Y ->"));
  display.setCursor(95,55);
  display.println(F("N ->"));
  display.display();
  while (digitalRead(12) == HIGH and digitalRead(14) == HIGH) {
    wdt_reset();
  }
  
  if (digitalRead(12) == LOW){
    readkey("/priv.txt");
    onConnectBT();
  } else if (digitalRead(14) == LOW) {
    //denial
    display.clearDisplay();
    display.setTextSize(1.5);
    display.setCursor(15,30);
    display.println(F("Request Denied !"));
    display.display();
    Serial.println("Denied");
    delay(2000);
    onConnectBT();
  }
  return;
}

void authProcedure(){
  return;
}

void encryptData() {
  speckTiny.setKey(key,32);
  speckTiny.encryptBlock(buffer, plaintext);
  if(memcmp(buffer, ciphertext, 64)==0)
    return;
}

void decryptData() {
  speckTiny.setKey(key,32);
  speckTiny.decryptBlock(buffer,ciphertext);
  if(memcmp(buffer, plaintext, 64)==0)
    return;
}
