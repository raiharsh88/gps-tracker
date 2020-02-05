#include <gprs.h>
#include <SoftwareSerial.h>

char http_cmd[] = "GET /get HTTP/1.1\r\nHost: httpbin.org\r\nConnection: close\r\n\r\n";       //GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1

char buffer[512];



//String("GET ") + url + " HTTP/1.1\r\n" +
//               "Host: " + host + "\r\n" + 
//               "Connection: close\r\n\r\n";       //GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1





//SoftwareSerial serialSIM800(D2,D3);
GPRS gprs;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("GPRS - HTTP Connection Test...");  
  gprs.preInit();
  while(0 != gprs.init()) {
     delay(1000);
     Serial.println("init error");
  }  
  while(!gprs.join("airtelgprs.com")) {  //change "cmnet" to your own APN
      Serial.println("Error joining GPRS network");
      delay(2000);
  }
  // successful DHCP
  Serial.print("IP Address is ");
  Serial.println(gprs.getIPAddress());
  
  Serial.println("Init success, connecting to exploreembedded.com ...");
  
  if(0 == gprs.connectTCP("httpbin.org",80)) {
      Serial.println("Successfully connected to exploreembedded.com!");//AT+CIPSTART="TCP","secret-shelf-98881.herokuapp.com",80 
  }else{
      Serial.println("connect error");
      while(1);
  }

  Serial.println("waiting to fetch...");


  
  if(0 == gprs.sendTCPData(http_cmd))
  {      
    gprs.serialDebug();
  }
  
//  gprs.closeTCP();
//  gprs.shutTCP();
//  Serial.println("close");  
}

void loop() {    
    
}
