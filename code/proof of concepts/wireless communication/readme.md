# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een [laptop|smartphone] (schappen wat niet past), gebruik makend van Wifi (in te vullen)
<br />
### configuratie
```cpp
#include <WiFi.h>
#include <FS.h>
//#include <LittleFS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#define FORMAT_LITTLEFS_IF_FAILED true

const char* ssid = "KOBE-LIJNVOLGER";
const char* password = "P@ss4Kobe!";
//WiFiServer server(80);
AsyncWebServer server(80);
String header;
```
```cpp
void initWifi()
{

  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password, 6);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);


  // Define routes
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Start lijnvolger
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
    start = true;    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Stop lijnvolger
  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    stop = true;    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Calibreer lijnvolger
  server.on("/calibrate", HTTP_GET, [](AsyncWebServerRequest *request){
    calibrate = true;    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Test lijnvolger
  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
    test = true;    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Bijwerken PID waarden <ESP_IP>/get?inputIntHigh=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    // GET input value on <ESP_IP>/get?inputIntHigh=<inputMessage>
    if (request->hasParam(PARAM_P)) {
      inputMessage = request->getParam(PARAM_P)->value();
      PID_Kp = inputMessage.toFloat();
      preferences.putFloat("pid_p", PID_Kp);
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam(PARAM_I)) {
      inputMessage = request->getParam(PARAM_I)->value();
      PID_Ki = inputMessage.toFloat();
      preferences.putFloat("pid_i", PID_Ki);
    }
    else if (request->hasParam(PARAM_D)) {
      inputMessage = request->getParam(PARAM_D)->value();
      PID_Kd = inputMessage.toFloat();
      preferences.putFloat("pid_d", PID_Kd);      
    }
    else if (request->hasParam("LFR_MAX_MOTOR_SPEED")) {
      inputMessage = request->getParam("LFR_MAX_MOTOR_SPEED")->value();
      LFR_MAX_MOTOR_SPEED = inputMessage.toInt();
      preferences.putShort("max_speed", LFR_MAX_MOTOR_SPEED);      
    }
    else if (request->hasParam("CALIBRATION_SPEED")) {
      inputMessage = request->getParam("CALIBRATION_SPEED")->value();
      CALIBRATION_SPEED = inputMessage.toInt();
      preferences.putShort("cal_speed", CALIBRATION_SPEED);      
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    //request->send(200, "text/text", inputMessage);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  /*
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getHumidity().c_str());
  });
  
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getPressure().c_str());
  });
  */

  // Start server
  server.begin();
}
```
---

```cpp
void setup() {
  Serial.begin(115200);

  delay(2000);

  // init preferences
  preferences.begin("kobe-lijnvolger", false); 

  // Remove all preferences under the opened namespace
  //preferences.clear();

  // Read preferences
  PID_Kp           = preferences.getFloat("pid_p", PID_Kp);
  PID_Ki           = preferences.getFloat("pid_i", PID_Ki);
  PID_Kd           = preferences.getFloat("pid_d", PID_Kd);
  LFR_MAX_MOTOR_SPEED = preferences.getShort("max_speed", LFR_MAX_MOTOR_SPEED);
  CALIBRATION_SPEED   = preferences.getShort("cal_speed", CALIBRATION_SPEED);

  // configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){Sens01, Sens02, Sens03, Sens04, Sens05, Sens06, Sens07, Sens08}, SensorCount);
  qtr.setEmitterPin(IR);
  
  //calibrate();
  //*/

    
  // Initialize SPIFFS
  /*
  if (!LittleFS.begin(false)) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
    */
  if (!SPIFFS.begin(false)) {
    Serial.println("An Error has occurred while mounting Spiffs");
    return;
  } 
  
  digitalWrite(LED_BUILTIN, LOW);

 // start = true
  initWifi();
}
```
---
### opmerkingen

### gebruiksaanwijzing

verbinding maken

    Verbind je toestel met de Wifi "KOBE-LIJNVOLGER"
    Surf in je browser naar "192.168.4.1"
    Op de webpagina kan je de instellingen aanpassen


