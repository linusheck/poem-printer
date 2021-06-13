#ifndef MAIN_H
#define MAIN_H

#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "SoftwareSerial.h"
#include <WiFiManager.h>

#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <base64.hpp>
#include <Arduino.h>

void setup();
void loop();
void configModeCallback(WiFiManager *manager);
void wakeUp();
void goToSleep();
void setPrintingSpeed();
void printPoem();
void printWeather();
void printNews();
void print(String name);

#define TX_PIN 5 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 4 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial printerSerial(RX_PIN, TX_PIN);
WiFiManager wifiManager;

#define HEIGHT 24
#define WIDTH 48 // bytes
#define CHUNK_SIZE HEIGHT * WIDTH
String chunkSizeString = String(CHUNK_SIZE);

#define simplePrint(x) wakeUp(); printerSerial.print((char) 10); printerSerial.println(x); printerSerial.print((char) 10); printerSerial.print((char) 10); 
boolean wifiNotFound = false;

#define NO_CLICK 0
#define CLICK 1
#define DOUBLE_CLICK 2
#define TRIPLE_CLICK 3

#endif
