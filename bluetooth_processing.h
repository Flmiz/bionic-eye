bool ssidState = false;
bool passwordState = false;
bool canUpdateCredentials = false;
String ssid = "";
String password = "";

void payloadProcessing(String data) {
  int signal = data.indexOf("|");
  if (signal == -1) {
   return;
  }

  String ssid_raw = data.substring(0, signal);
  String password_raw = data.substring(signal + 1);

  if (ssid_raw.startsWith("SSID")) {
    ssid = ssid_raw.substring(ssid_raw.indexOf(":") + 1);
    ssidState = true;
  }
  if (password_raw.startsWith("PASSWORD")) {
    password = password_raw.substring(0, password_raw.length() - 2).substring(password_raw.indexOf(":") + 1);
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
