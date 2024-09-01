#include "env.h"
#include "camera_server.h"
#include "bluetooth.h"

enum Led {
  RED,
  GREEN,
  YELLOW,
  BLUE
};

void batteryStatus(Led led, int cycles, float transitionTime, float highTime) {
  int ledPin[] {-1, -1, -1};
  
  switch (led) {
    case RED: 
      ledPin[0] = 0;
      break;
    case YELLOW: 
      ledPin[0] = 0;
      ledPin[1] = 1;
      break;
    case GREEN: 
      ledPin[0] = 1;
      break;
    case BLUE: 
      ledPin[0] = 2;
      break;
  }

  for (int i = 0; i < 3; i++) {
    ledcWrite(ledPins[i], 255);
  }

  unsigned long time;
  for (int i = 0; i < cycles; i++) {
    time = millis();
    while (millis() - time <= transitionTime * 1000) {
      for (int j = 0; j < 3; j++) {
        if (ledPin[j] == -1) {
          break;
        }
        ledcWrite(ledPins[ledPin[j]], (int) (255 * (float) (1 - (millis() - time) / (transitionTime * 1000))));
      }
    }
    delay(highTime * 1000);
    time = millis();
    while (millis() - time <= transitionTime * 1000) {
      for (int j = 0; j < 3; j++) {
        if (ledPin[j] == -1) {
          break;
        }
        ledcWrite(ledPins[ledPin[j]], (int) (255 * (float) ((millis() - time) / (transitionTime * 1000))));
      }
    }
  }
  for (int i = 0; i < 3; i++) {
    ledcWrite(ledPins[i], 255);
  }
}

void setup() { 
  Serial.begin(115200); 
  Serial.println(); 
  Serial.println(); 

  setupBLE("sight one");
  serverSetup();
} 
  
void loop() { 
    loopBLE();

  if (canUpdateCredentials) {
    canUpdateCredentials = false;

    setNetwork(ssid, password);
    notify("IP:" + WiFi.localIP());
  }
} 
