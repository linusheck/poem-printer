#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "SoftwareSerial.h"
#include <WiFiManager.h>

#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <base64.hpp>

// SORRY NOT SORRY FOR THIS CODE.

#define TX_PIN 5 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 4 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial printerSerial(RX_PIN, TX_PIN);
WiFiManager wifiManager;

#define HEIGHT 24
#define WIDTH 48 // bytes
#define CHUNK_SIZE HEIGHT * WIDTH
String chunkSizeString = String(CHUNK_SIZE);

#define simplePrint(x) wakeUp(); printerSerial.print((char) 10); printerSerial.println(x); printerSerial.print((char) 10); printerSerial.print((char) 10); 
#define STYLE "<style>body { font-family: monospace; background: beige; } button {border: 0; background-color: #000; color: #fff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; font-family: monospace; } </style>"
#define PARAGRAPH "<p>SLOGAN OF YOUR PROJECT</p>"

// Have not tested this after inserting templates. If something doesn't work it's maybe because of these lines
const String BACKEND_URL = String("your.backend.url");
#define WIFI_NAME "" // name of your WiFi network
const String OPENWEATHERMAP_ID = String(""); // OpenWeatherMap location id
const String OPENWEATHERMAP_UNITS = String("metric"); // OpenWeatherMap units
const String OPENWEATHERMAP_LANG = String("en"); // OpenWeatherMap language

boolean wifiNotFound = false;

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, INPUT_PULLUP);

  digitalWrite(12, HIGH);

  Serial.begin(9600);
  printerSerial.begin(19200);

  wifiManager.setCustomHeadElement(STYLE);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect(WIFI_NAME);
  if (wifiNotFound) {
    simplePrint(F("Found WiFi network!"));
  }

  goToSleep();
}

void configModeCallback(WiFiManager *manager) {
  wifiNotFound = true;
  simplePrint(F("Did not found WiFi\nnetwork! Please\nconfigure it."));
}

void loop() {
  if (digitalRead(13) == LOW) {
    uint64_t m = millis();
    uint64_t debounceDelay = 50;
    uint64_t lastDebounceTime = millis();
    bool lastDebounceReading = LOW;
    while (true) {
      bool reading = digitalRead(13);

      if (reading != lastDebounceReading) {
        lastDebounceTime = millis();
        lastDebounceReading = reading;
      }

      if ((millis() - lastDebounceTime) > debounceDelay && reading == HIGH) {
        uint64_t interval = millis() - m;
        wakeUp();
        setPrintingSpeed();
        if (interval > 1000) {
          printWeather();
        } else {
          printPoem();
        }
        goToSleep();
        break;
      }
    }
  }
}

void wakeUp() {
  printerSerial.print((char) 0xff);
  delay(50);
  for (uint8 i = 0; i < 20; i++) {
    printerSerial.print((char) 0);
  }
  printerSerial.print((char) 27);
  printerSerial.print((char) 56);
  printerSerial.print((char) 0);
  printerSerial.print((char) 0);
}

void goToSleep() {
  printerSerial.print((char) 27);
  printerSerial.print((char) 56);
  printerSerial.print((char) 1);
  printerSerial.print((char) 0);
}

void setPrintingSpeed() {
  printerSerial.print((char) 27);
  printerSerial.print((char) 55);
  printerSerial.print((char) 7);
  printerSerial.print((char) 255);
  printerSerial.print((char) 15);
}

void printPoem() {
  WiFiClient client;
  HTTPClient http;

  // First of all, get the poem's name
  String poemName;
  if (http.begin(client, "http://" + BACKEND_URL + "/name")) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      poemName = http.getString();
    }
  }

  print("poem?name=" + poemName);
}

void printWeather() {
  print("weather?id=" + OPENWEATHERMAP_ID + "&units=" + OPENWEATHERMAP_UNITS + "&lang=" + OPENWEATHERMAP_LANG);
}

void print(String name) {
  WiFiClient client;
  HTTPClient http;

  int pos = 0;
  int length = CHUNK_SIZE + 1;
  while (pos < length) {
    // Enable bit image mode.
    printerSerial.print((char) 18);
    printerSerial.print((char) 86);
    printerSerial.print((char) HEIGHT);
    printerSerial.print((char) 0);

    String posString = String(pos);
    // Now, print it chunk by chunk
    if (http.begin(client, "http://" + BACKEND_URL + "/" + name + 
        "&length=" + chunkSizeString + "&from=" + pos)) {
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String content = http.getString();
        length = content.substring(0, content.indexOf("\n")).toInt();
        String base64 = content.substring(content.indexOf("\n") + 1);
        
        char src[base64.length()];
        base64.toCharArray(src, base64.length());
        unsigned char binary[CHUNK_SIZE];

        for (int i = 0; i < CHUNK_SIZE; i++) {
          binary[i] = 0;
        }
  
        decode_base64((unsigned char*) src, binary);

        for (int i = 0; i < CHUNK_SIZE; i++) {
          printerSerial.print((char) binary[i]);
        }
      }
    }
    pos += CHUNK_SIZE;
  }

  for (int i = 0; i < 4; i++) {
    printerSerial.print((char) 10);
  }
}