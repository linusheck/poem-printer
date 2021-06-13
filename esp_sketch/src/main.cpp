#include "main.h"
#include "config.h"

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
  simplePrint(F("Did not find WiFi network!\nPlease configure it."));
}

void loop() {
  // We model the double / triple click using a state machine
  // no click -> click -> double click -> triple click
  static uint8_t state = NO_CLICK;
  static uint64_t lastClick = millis();
  static bool currentlyPressing = false;

  static uint64_t debounceDelay = 50;
  static uint64_t lastDebounceTime = millis();
  static bool lastDebounceReading = HIGH;
  bool reading = digitalRead(13);

  if (reading != lastDebounceReading) {
    lastDebounceTime = millis();
    lastDebounceReading = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && !currentlyPressing) {
      if (state < TRIPLE_CLICK) {
        lastClick = millis();
        state++;
        currentlyPressing = true;
      }
    } else if (reading == HIGH) {
      currentlyPressing = false;
      if (state > NO_CLICK && (millis() - lastClick) > 1000) {
        // PRINT
        wakeUp();
        setPrintingSpeed();
        switch (state) {
          case NO_CLICK:
            break;
          case CLICK:
            printPoem();
            break;
          case DOUBLE_CLICK:
            printWeather();
            break;
          case TRIPLE_CLICK:
            printNews();
            break;
        }
        goToSleep();
        state = NO_CLICK;
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
  if (http.begin(client, String("http://") + BACKEND_URL + "/name")) {
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

void printNews() {
  print("news?time=" + String(millis()));
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
    if (http.begin(client, String("http://") + BACKEND_URL + "/" + name + 
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
