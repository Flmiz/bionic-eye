bool ssidState = false;
bool passwordState = false;
bool canUpdateCredentials = false;
String ssid = "";
String password = "";

void payloadProcessing(String data) {
  int i = data.indexOf(":");
  if (i == -1) {
   return;
  }

  if (data.startsWith("SSID")) {
    ssid = data.substring(i + 1);
    ssidState = true;
  }
  else if (data.startsWith("PASSWORD")) {
    password = data.substring(i + 1);
    passwordState = true;
  }

  if (ssidState && passwordState) {
    canUpdateCredentials = true;
    ssidState = false;
    passwordState = false;

    Serial.println("SSID: " + ssid);
    Serial.println("PASSWORD: " + password);
  }

}
