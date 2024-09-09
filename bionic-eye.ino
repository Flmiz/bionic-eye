#include "env.h"
#include "camera_server.h"
#include "bluetooth.h"

enum Led {
  RED,
  GREEN,
  YELLOW,
  BLUE
};

enum Mode {
  DYNAMIC,
  STATIC
};

const byte ledPins[] = {RED_PIN, GREEN_PIN, BLUE_PIN};
const byte chns[] = {0, 1, 2};
int red, green, blue;
int transistor = TRANSISTOR_PIN;

int vibration = VIBRATION_PIN;
int touch = TOUCH_PIN;

bool isTouching = false;
bool blockTouch;

int amountOfClicks = 0;
int maxAmountOfCLicks = 2;

Mode mode = DYNAMIC;

long longPressTime = 0;
int longPressDuration = 400;
long quickPressTime = 0;
int quickPressDuration = 500; 

int amountOfVibrations = 0;
int maxAmountOfVibrations = 2;

bool isVibrating = false;
bool wasVibrating = false;
bool canVibrate = false;
long vibrationTime = 0;
int vibrationDuration = 0;
int vibrationPauseDuration = 100;

void clickDetection() {
  if (canVibrate) {
    return;
  }

  if (digitalRead(touch)) {
    if (blockTouch) {
      return;
    }

    if (!isTouching) {
      if (amountOfClicks >= maxAmountOfCLicks && millis() - quickPressTime <= quickPressDuration) {
        amountOfClicks = 0;
        notify("MODE:QUICK-SHOT");

        blockTouch = true;
        canVibrate = true;

        vibrationTime = millis();
        vibrationDuration = 300;
        amountOfVibrations = 0;
        maxAmountOfVibrations = 1;
        isVibrating = true;
      }

      quickPressTime = millis();
      longPressTime = millis();
      amountOfClicks++;
      isTouching = true;
    }

    if (isTouching && millis() - longPressTime >= longPressDuration) {
      switch (mode) {
        case DYNAMIC:
          notify("MODE:STATIC");
          mode = STATIC;
          maxAmountOfVibrations = 3;
          break;
        case STATIC:
          notify("MODE:DYNAMIC");
          mode = DYNAMIC;
          maxAmountOfVibrations = 2;
          break;
      }

      amountOfClicks = 0;
      canVibrate = true;

      vibrationTime = millis();
      vibrationDuration = 250;
      amountOfVibrations = 0;
      isVibrating = true;

      blockTouch = true;
    }
  }
  else if (isTouching) {
    isTouching = false;
    blockTouch = false;
  }
}

void vibrate() {
  if (canVibrate) {
    if (isVibrating != wasVibrating) {
      digitalWrite(vibration, isVibrating);
      wasVibrating = isVibrating;
      vibrationTime = millis();
    }

    if (amountOfVibrations >= maxAmountOfVibrations && !isVibrating) {
      canVibrate = false;
      return;
    }

    if (isVibrating && millis() - vibrationTime >= vibrationDuration) {
      isVibrating = false;
      amountOfVibrations++;
    }
    else if(!isVibrating && millis() - vibrationTime >= vibrationPauseDuration) {
      isVibrating = true;
    }
  }
}

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

  pinMode(touch, INPUT_PULLDOWN);
  pinMode(vibration, OUTPUT);

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

  if (deviceConnected) {
    clickDetection();
    vibrate();
  }
} 
