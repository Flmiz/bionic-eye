#include "env.h"
#include "camera_server.h"
#include "bluetooth.h"

const byte ledPins[] = {RED_PIN, GREEN_PIN, BLUE_PIN};
const byte chns[] = {0, 1, 2};
int red, green, blue;
int transistor = TRANSISTOR_PIN;

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

  for (int i = 0; i < 3; i++) { //setup the pwm channels,1KHz,8bit
      ledcAttachChannel(ledPins[i], 1000, 8, chns[i]);
    }

    pinMode(transistor, OUTPUT);

    // set the resolution to 12 bits (0-4095)
    analogReadResolution(12);

    digitalWrite(transistor, HIGH);

    int analogVolts = analogReadMilliVolts(1);
    Serial.println("ADC millivolts value = ");
    Serial.print(analogVolts);

    digitalWrite(transistor, LOW);

    if (analogVolts > 2900) {
      batteryStatus(GREEN, 2, 0.5, 1.0);
    }
    else if (analogVolts > 2500) {
      batteryStatus(YELLOW, 2, 0.5, 1.0);
    }
    else {
      batteryStatus(RED, 3, 0.5, 1.0);
    }

  Serial.println(); 
  Serial.println(); 

  setupBLE(DEVICE_NAME);
  serverSetup();
} 
  
void loop() { 
  loopBLE();

  if (canUpdateCredentials) {
    canUpdateCredentials = false;

    setNetwork(ssid, password);
    notify("IP:" + WiFi.localIP().toString());
  }
} 
