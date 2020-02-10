#include "ESP8266WiFi.h"
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <gprs.h>
#include <MPU6050.h>
#define FREQUENCY    80

MPU6050 mpu;
String mac2  = "";
String mac1 = "";
float meanZ = 0;
float meanX = 0;
float meanY = 0;
float mean = 0;
char buffer[15000];
GPRS gprs;

float offset  = 0;


extern "C" {
#include "user_interface.h"
}




//POST /cgi-bin/process.cgi HTTP/1.1
//User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
//Host: www.tutorialspoint.com
//Content-Type: application/x-www-form-urlencoded
//Content-Length: length
//Accept-Language: en-us
//Accept-Encoding: gzip, deflate
//Connection: Keep-Alive
//
//licenseID=string&content=string&/paramsXML=string





void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  mpuInit();
  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);
  digitalWrite(D7, HIGH);
  digitalWrite(D6, LOW);

  // If you want, you can set accelerometer offsets
  // mpu.setAccelOffsetX();
  // mpu.setAccelOffsetY(78);
  // mpu.setAccelOffsetZ();

  checkSettings();




  
    Serial.println("GPRS - HTTP Connection Test...");
    gprs.preInit();
    while (0 != gprs.init()) {
      ESP.wdtFeed();
      delay(1000);
      Serial.println("init error");
    }
  
  
   while (!gprs.join("airtelgprs.com")) { 
      Serial.println("Error joining GPRS network");
      //wdt_reset();
      delay(1000);
  
    }



  //gprs.getLoc("AT+CIPGSMLOC=1,1");
  // successful DHCP
  Serial.print("IP Address is ");
  Serial.println(gprs.getIPAddress());

  Serial.println("Init success, connecting to Heroku server ...");


  //  Vector rawAccel = mpu.readRawAccel();
  //  Vector normAccel = mpu.readNormalizeAccel();
  //
  //  for (int i = 0; i < 200; i++) {
  //  Vector normAccel = mpu.readNormalizeAccel();
  //
  //    offset += normAccel.YAxis;
  //    if (i == 199) {
  //      offset = offset / 199;
  //      Serial.print("Offset: ");
  //      Serial.println(offset);
  //      delay(3000);
  //    }
  //  }
}

void loop() {

  //unsigned long counter  = millis();
  //while(analogRead(A0)>800){
  //  ESP.wdtFeed();
  //  if((millis()-counter)>= 1000){
  //    open_();
  //
  //    }
  //
  //  }

  unsigned long counter = millis();
  
  while ((millis() - counter) <= 30000) { 
        ESP.wdtFeed();
        
    if (analogRead(A0) >= 800) {
      delay(2000);
      for(int i = 0;i<=4;i++){
        digitalWrite(D2,HIGH);
        delay(200);
        digitalWrite(D2,LOW);
        delay(200);
        }
      if(analogRead(A0)>=800){
      
      Serial.println("Box is open");
      open_();
      break;
      }
    }
    else if ((millis() - counter) >= 20000) {
     // mac1 = "";
//      mac2 = "";
      sendToServer();
      break;
    }




  }




  //sendToServer();

  //ESP.deepSleep(10e6);


}

void checkSettings()
{
  Serial.println();

  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Clock Source:          ");
  switch (mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }

  Serial.print(" * Accelerometer:         ");
  switch (mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());

  Serial.println();
}



bool getWiFi() {

  WiFi.mode(WIFI_STA);
  delay(20);
  WiFi.disconnect();
  Serial.println("WiFi Enabled");
  delay(10);
  WiFi.forceSleepBegin();                  // turn off ESP8266 RF
  delay(1);                                // give RF section time to shutdown
  system_update_cpu_freq(FREQUENCY);
  delay(10);
  uint32_t free = system_get_free_heap_size();
  Serial.print("Heap = ");
  Serial.println(free);
  Serial.println("WiFi Disabled");

}


void sendToServer() {


  Serial.println("Setup done");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  char http_cmd[500] = "POST /postcred HTTP/1.1\r\nHost: gps-test-server.herokuapp.com\r\nContent-Type: application/json\r\nContent-Length: %3d";       //GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1
  char header_closing[] = "\r\nConnection: close\r\n\r\n";



  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  if (n > 3)n = 3;

  root["apCount"] = n;
  boolean copy = false;
  uint8_t dec = 0;


  while (dec < n) {
    if (mac1 == WiFi.BSSIDstr(dec)) {
      copy = true;
      break;


    }
    else if (mac2 == WiFi.BSSIDstr(dec)) {
      copy = true;
      break;

    }
    dec++;

  }


  if (copy) {
    Serial.println("Same AP");
    Serial.print("Copy :");
    Serial.println(copy);
    delay(1000);
    return;
  }

  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");

  }
  for (int i = 0; i < n; ++i) {
    //Print SSID and RSSI for each network found
   // root["Ap" + String(i)] = WiFi.SSID(i);
    root["macAddress" + String(i)] = WiFi.BSSIDstr(i);
    root["RSSI" + String(i)] =  WiFi.RSSI(i);
    if (i == 0) {
      mac1 = WiFi.BSSIDstr(i);

    } else if (i == 1) {
      mac2 = WiFi.BSSIDstr(i);
    }
    delay(10);
  }


  //Serial.println("1");
  size_t length_ = root.measureLength(); // returns 17

  size_t jsonSize = length_ + 1;
  char json[jsonSize] ;

  root.printTo(json, jsonSize);
  int jsonArrayLen = strlen(json);


  sprintf(http_cmd, http_cmd, jsonArrayLen);



  root.prettyPrintTo(Serial);
  Serial.println();
  for (int i = 0; i <= jsonArrayLen; i++) {
    Serial.print(json[i]);
  }





  strcat(http_cmd, header_closing);
  strncat(http_cmd, json, jsonArrayLen);
  int command_len = strlen(http_cmd);
  Serial.print("Arraylen = ");
  Serial.println(command_len);

  for (int i = 0; i <= command_len; i++) {
    Serial.print(http_cmd[i]);
  }


  Serial.println();












  if (0 == gprs.connectTCP("gps-test-server.herokuapp.com", 80)) {
    Serial.println("Successfully connected to exploreembedded.com!");//AT+CIPSTART="TCP","secret-shelf-98881.herokuapp.com",80
  } else {
    Serial.println("connect error");
    while (1);
  }

  Serial.println("waiting to fetch...");


  if (0 == gprs.sendTCPData(http_cmd))
  {
    gprs.serialDebug();
  }

  gprs.closeTCP();
  delay(500);

  //gprs.sendCmd("AT+SAPBR=0,1\r\n");
  //  gprs.shutTCP();

  //  Serial.println("close");


}

void mpuInit() {
  Serial.println("Initialize MPU6050");

  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_4G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
}

void motion() {



  unsigned long counter = millis();
  float lastMean = 0;
  unsigned long change = 0;
  unsigned long thisChange = 0;
  unsigned long prevChange = 0;
  while ((millis() - counter ) < 15000) {






    Vector normAccel = mpu.readNormalizeAccel();
    Vector rawAccel = mpu.readRawAccel();
    meanZ = normAccel.ZAxis;
    meanX = normAccel.XAxis;
    meanY = normAccel.YAxis;


    mean  = meanZ + meanX + meanY;

    if (mean > lastMean) {
      if ((mean - lastMean) >= 15) {
        change++;
        thisChange  = millis();
        delay(60);
      }
    }
    else if (lastMean > mean) {

      if ((lastMean - mean) >= 15) {
        change++;
        thisChange  = millis();
        delay(60);

      }
    }
    lastMean = mean;



    Serial.print("Mean :");
    Serial.println(mean);


    ESP.wdtFeed();


    if (thisChange - prevChange >= 200) {
      change = 0;

    }
    else {
      if (change >= 60)break;
    }
    prevChange  = thisChange;

  }

  Serial.print("  Change : ");
  Serial.println(change);

  if (change >= 58) {
    digitalWrite(D6, HIGH);

  }
  else {
    digitalWrite(D6, LOW);
  }
  //  Serial.print("  Znorm = ");
  //  Serial.print(meanZ);
  //  Serial.print("  Xnorm = ");
  //  Serial.print(meanX);
  //  Serial.print("  Ynorm = ");
  //  Serial.println(meanY);

  // if(millis()-counter>=3000){
  //    digitalWrite(D0,LOW);
  //    delay(1000);
  //
  //    digitalWrite(D0,HIGH);
  //    delay(10);
  //    mpuInit();
  //    break;
  //
  //    }

  //ESP.wdtFeed();



}


void open_() {

//mac2  = "";
//mac1 = "";

  
  sendToServer();
  char http_cmd[] = "GET /open HTTP/1.1\r\nHost: gps-test-server.herokuapp.com\r\nConnection: close\r\n\r\n";
Serial.println("Box is open");



  if (0 == gprs.connectTCP("gps-test-server.herokuapp.com", 80)) {
    Serial.println("Successfully connected to Server!");//AT+CIPSTART="TCP","secret-shelf-98881.herokuapp.com",80
  } else {
    Serial.println("connect error");
    while (1);
  }

  Serial.println("waiting to fetch...");


  if (0 == gprs.sendTCPData(http_cmd))
  {
    gprs.serialDebug();
  }

  gprs.closeTCP();


  //gprs.sendCmd("AT+SAPBR=0,1\r\n");
  //  gprs.shutTCP();






}
