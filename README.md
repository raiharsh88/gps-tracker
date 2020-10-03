# GPS Tracker using googles geo location API.

This project uses Googles Geolocation API .

## Overview

Googles Geolocation API offers accurate location data in terms of latitude and longitude with upto 25M of accuracy . 

This works when the client send the information such as mac address or Cell tower Id of nearest WiFi access points or cell towers , as a response to this data the
googles endpoint responds with the location data (co-ordinates) along with estimated accuracy.

My project consits of SIM 800L (GSM Module) and ESP8266-12F (Wifi Module and MicroController) along with a few sensors such as MPU6050 6 Axis ( 3xis Accelerometer and 3 axis Gyroscope).

Esp8266 acts as host MCU and it is programmed in Arduino . It scans all the nearby accesspoints and collects the cell tower data using Wifi radio and GSM modules and sends the data in JSON format to our server (written in NodeJs) hosted on Heroku. 

Server on heroku sends the data to Google's geolocation API ,which returns location data and the server publishes this data to a noSQL database.

From the noSQL database the data can be read by web application or android app to give a visulal interface.


## Hardware

Esp8266(WiFi + MCU)

SIM800L(GSM module)

MPU 6050 6 Axis Motion sensor ( 3xis Accelerometer + 3 axis Gyroscope).

Proximity Sensor 

## Software 

Arduino for programming MCU.

NodeJS for server 


