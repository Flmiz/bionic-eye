#include "env.h"
#include "camera_server.h"
#include "bluetooth.h"

void setup() { 
  Serial.begin(115200); 

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
