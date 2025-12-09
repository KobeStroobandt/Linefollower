#include <Arduino.h>

/* MOTOR */
#include <SparkFun_TB6612.h>

const int offsetA = -1;
const int offsetB = 1;

#define AIN1 11
#define BIN1 9
#define AIN2 12
#define BIN2 3
#define PWMA 13
#define PWMB 10
#define STBY 14

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

// This is the maximum speed the motors will be allowed to turn.
// (400 lets the motors go at top speed; decrease to impose a speed limit)
const int MAX_SPEED = 150;


/* SENSORS */
#include <QTRSensors.h>

#define IR 21
#define Sens01 47
#define Sens02 38
#define Sens03 39
#define Sens04 40
#define Sens05 41
#define Sens06 42
#define Sens07 2
#define Sens08 1

QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
int lastError = 0;


/* WIFI */
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
   

/* PREFERENCES */
#include <Preferences.h>

Preferences preferences;


/* Various */
bool start = false;
bool stop = false;
bool calibrate = false;
bool test = false;

const char* PARAM_P = "inputP";
const char* PARAM_I = "inputI";
const char* PARAM_D = "inputD";

uint16_t LFR_SensorValue[8];       /**< Array to Save Raw IR Sensor values of QTR-8RC */
uint16_t LFR_Position = 0;         /**< Variable to Save the QTR-8RC Sensor Position */
int16_t  LFR_Proportional = 0;     /**< Variable to Save the Proportional Output of PID Control Algorithm */
int16_t  LFR_LastProportional = 0; /**< Variable to Save the Previous Proportional Output of PID Control Algorithm */
int16_t  LFR_Derivative = 0;      /**< Variable to Save the Derivative Output of PID Control Algorithm */
int64_t  LFR_Integral = 0;         /**< Variable to Save the Integral Output of PID Control Algorithm */
int16_t  LFR_ControlOutput = 0;    /**< Variable to Save the Final Control Output of PID Control Algorithm */
int16_t  LFR_MotorSpeedLeft = 0;
int16_t  LFR_MotorSpeedRight = 0;
int16_t  QTR_LINE_MID_VALUE = 3500;
int16_t  LFR_MAX_MOTOR_SPEED = 180; /* absolute max = 255*/
int16_t  LFR_BASE_SPEED = 100;
int16_t  CALIBRATION_SPEED = 25;
int16_t  LFR_ASYM = 40;           // tss 0 en 100, bepaalt asymmetrie bij sturen
float PID_Kp = 0.1;
float PID_Ki = 0;
float PID_Kd = 4;

/* functions */
String processor(const String& var);
void doDrive();
void doTest();
void doTestDrive();
void doTestDrive2();
void doDrive2();
void doDrive3(bool test = false);
void doOutputSensorValues();

// put function declarations here:
void doCalibrate()
{
  start = false;
  stop = false;
  calibrate = false;

  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode

  // 2.5 ms RC read timeout (default) * 10 reads per calibrate() call
  // = ~25 ms per calibrate() call.
  // Call calibrate() 400 times to make calibration take about 10 seconds.
  /*
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  */
	for(uint8_t i=0; i<40; i++)   /**< Calibrate the QTR-8RC Sensor */
	{
    motor1.drive( CALIBRATION_SPEED);
    motor2.drive(-CALIBRATION_SPEED);      /**< Rotates the Robot */
		delay(20);
    motor1.drive(0);
    motor2.drive(0);
  	qtr.calibrate();    
    delay(20);
	}
  for(uint8_t i=0; i<80; i++)   /**< Calibrate the QTR-8RC Sensor */
	{
    motor1.drive(-CALIBRATION_SPEED);
    motor2.drive( CALIBRATION_SPEED);      /**< Rotates the Robot */
		delay(20);
    motor1.drive(0);
    motor2.drive(0);
  	qtr.calibrate();    
    delay(20);
	}
  for(uint8_t i=0; i<40; i++)   /**< Calibrate the QTR-8RC Sensor */
	{
    motor1.drive( CALIBRATION_SPEED);
    motor2.drive(-CALIBRATION_SPEED);      /**< Rotates the Robot */
		delay(20);
    motor1.drive(0);
    motor2.drive(0);
  	qtr.calibrate();    
    delay(20);
	}
/*
	motor1.drive(0);          
  motor2.drive(0);
	delay(500);
	motor1.drive(-CALIBRATION_SPEED);
  motor2.drive( CALIBRATION_SPEED);      
	for(uint8_t i=0; i<80; i++)  
	{
		qtr.calibrate();  
		delay(20);
	}
	motor1.drive(0);          
  motor2.drive(0);
	delay(500);
	motor1.drive( CALIBRATION_SPEED);
  motor2.drive(-CALIBRATION_SPEED);      
	for(uint8_t i=0; i<40; i++)   
	{
		qtr.calibrate();  
		delay(20);
	}
  */
	/*
  // Auto-calibration: turn right and left while calibrating the
  // sensors.
  for(int i = 0; i < 80; i++)
  {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70))
    {
      motor1.drive(-80);
      motor2.drive( 80);
    }
    else
    {
      motor1.drive( 80);
      motor2.drive(-80);
    }
    qtr.calibrate();

    // Since our counter runs to 80, the total delay will be
    // 80*20 = 1600 ms.
    delay(20);
  }
  */

  /*
  for(int counter = 0; counter < 80; counter++)
  {
    
    if(counter < 20 || counter >= 60)
    {
      motor1.drive(-40);
      motor2.drive( 40);
    }
    else
    {
      motor1.drive( 40);
      motor2.drive(-40);
    }
    qtr.calibrate();

    // Since our counter runs to 80, the total delay will be
    // 80*20 = 1600 ms.
    delay(20);
  }
    */
  motor1.brake();
  motor2.brake();
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration
  delay(20);
  
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();
  

  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  
  delay(1000);
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

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

void doStop()
{
  motor1.drive(0);
  motor2.drive(0);
  start = false;
  stop  = false;
  test  = false;
}

void doOutputSensorValues()
{
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(LFR_SensorValue[i]);
    Serial.print('\t');
  }
  Serial.println(LFR_Position);

}

void loop() {
  if (calibrate)
  {
    doCalibrate();
  }
  else if (start && !stop)
  {
    doDrive2();
    //doDrive3(false);
  }
  else if (stop)
  {
    doStop();
  }
  else if (test)
  {
    doDrive3(true);
    //doTestDrive2();
  }
}

void doTest()
{
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);

  delay(250);
}

void doTestDrive()
{
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);


  int error = position - 3500;
  int speedDifference = error / 4 + 6 * (error - lastError);

  lastError = error;
  int m2Speed = MAX_SPEED + speedDifference;
  int m1Speed = MAX_SPEED - speedDifference;

  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;
  if (m1Speed > MAX_SPEED)
    m1Speed = MAX_SPEED;
  if (m2Speed > MAX_SPEED)
    m2Speed = MAX_SPEED;

  Serial.print(m1Speed);
  Serial.print(',');
  Serial.print(m2Speed);
  Serial.println();

  delay(250);
}

void doTestDrive2()
{
  start = false;

  LFR_Position = qtr.readLineBlack(LFR_SensorValue);    /**< Reads the QTR-8RC Line Sensor to Get the Line Position */
  LFR_Proportional = LFR_Position - QTR_LINE_MID_VALUE;             /**< Computes the Proportional Output of PID Control Algorithm */
  LFR_Derivative = LFR_Proportional - LFR_LastProportional;         /**< Computes the Derivative Output of PID Control Algorithm */
  LFR_Integral += LFR_Proportional;                                 /**< Computes the Integral Output of PID Control Algorithm */
  LFR_LastProportional = LFR_Proportional;                          /**< Saves the Old Proportional Output of PID Control Algorithm */

  LFR_ControlOutput = LFR_Proportional*PID_Kp + LFR_Integral*PID_Ki + LFR_Derivative*PID_Kd;    /**< Computes the Final Control Output of PID Control Algorithm */

  if(LFR_ControlOutput > LFR_MAX_MOTOR_SPEED)
  {
    LFR_ControlOutput = LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }
  if(LFR_ControlOutput < -LFR_MAX_MOTOR_SPEED)
  {
    LFR_ControlOutput = -LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }

  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(LFR_SensorValue[i]);
    Serial.print('\t');
  }
  Serial.println(LFR_Position);

  if(LFR_ControlOutput < 0)
  {
    Serial.print(LFR_MAX_MOTOR_SPEED + LFR_ControlOutput);
    Serial.print(',');
    Serial.print(LFR_MAX_MOTOR_SPEED);
    Serial.println();
  }
  else
  {
    Serial.print(LFR_MAX_MOTOR_SPEED);
    Serial.print(',');
    Serial.print(LFR_MAX_MOTOR_SPEED - LFR_ControlOutput);
    Serial.println();
  }
  delay(250);
}

void doDrive3(bool test)
{

  LFR_Position = qtr.readLineBlack(LFR_SensorValue);    /**< Reads the QTR-8RC Line Sensor to Get the Line Position */
  
  /*
  // too Right, Robot will go left
  if(LFR_Position > 3800)  
  {
      motor2.drive( LFR_BASE_SPEED);
      motor1.drive(-LFR_BASE_SPEED);
      return;    
  }

  // too Left, Robot will go right
  if(LFR_Position < 2900)
  {  
      motor2.drive( LFR_BASE_SPEED);
      motor1.drive(-LFR_BASE_SPEED);
      return;
  }
*/
  
  
  LFR_Proportional = LFR_Position - QTR_LINE_MID_VALUE;             /**< Computes the Proportional Output of PID Control Algorithm */
  LFR_Derivative = LFR_Proportional - LFR_LastProportional;         /**< Computes the Derivative Output of PID Control Algorithm */
  LFR_Integral += LFR_Proportional;                                 /**< Computes the Integral Output of PID Control Algorithm */
  LFR_LastProportional = LFR_Proportional;                          /**< Saves the Old Proportional Output of PID Control Algorithm */

  LFR_ControlOutput = LFR_Proportional*PID_Kp + LFR_Integral*PID_Ki + LFR_Derivative*PID_Kd;    /**< Computes the Final Control Output of PID Control Algorithm */

  //if (abs(LFR_ControlOutput) < 5)
  if (LFR_ControlOutput < 0)
  {
    LFR_MotorSpeedRight = LFR_BASE_SPEED + (abs(LFR_ControlOutput) * (100+LFR_ASYM) / 100);
    LFR_MotorSpeedLeft  = LFR_BASE_SPEED + (abs(LFR_ControlOutput) * (100-LFR_ASYM) / 100);
  }
  else
  {
    LFR_MotorSpeedRight = LFR_BASE_SPEED + (abs(LFR_ControlOutput) * (100-LFR_ASYM) / 100);
    LFR_MotorSpeedLeft  = LFR_BASE_SPEED + (abs(LFR_ControlOutput) * (100+LFR_ASYM) / 100);
  }

  if(LFR_MotorSpeedLeft > LFR_MAX_MOTOR_SPEED)
  {
    LFR_MotorSpeedLeft = LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }
  if(LFR_MotorSpeedLeft < -LFR_MAX_MOTOR_SPEED)
  {
    LFR_MotorSpeedLeft = -LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }
  if(LFR_MotorSpeedRight > LFR_MAX_MOTOR_SPEED)
  {
    LFR_MotorSpeedRight = LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }
  if(LFR_MotorSpeedRight < -LFR_MAX_MOTOR_SPEED)
  {
    LFR_MotorSpeedRight = -LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }

  // waarde negatief -> lijn ligt rechts -> motor links meer sturen dan motor rechts
  // waarde positief -> lijn ligt links -> ...
  // motor 2 = links
  // motor 1 = rechts

  if (test)
  {
    Serial.print("ControlOutput:");
    Serial.println(LFR_ControlOutput);
    doOutputSensorValues();
  }

  if (test)
  {
    Serial.print(LFR_MotorSpeedLeft);
    Serial.print(',');
    Serial.print(LFR_MotorSpeedRight);
    Serial.println();
  }
  else
  {
    motor1.drive(LFR_MotorSpeedLeft);
    motor2.drive(LFR_MotorSpeedRight);    /**< Drives the Motor According to the Control Output */
  }
}

void doDrive2()
{
  test = false;

  LFR_Position = qtr.readLineBlack(LFR_SensorValue);    /**< Reads the QTR-8RC Line Sensor to Get the Line Position */
  LFR_Proportional = LFR_Position - QTR_LINE_MID_VALUE;             /**< Computes the Proportional Output of PID Control Algorithm */
  LFR_Derivative = LFR_Proportional - LFR_LastProportional;         /**< Computes the Derivative Output of PID Control Algorithm */
  LFR_Integral += LFR_Proportional;                                 /**< Computes the Integral Output of PID Control Algorithm */
  LFR_LastProportional = LFR_Proportional;                          /**< Saves the Old Proportional Output of PID Control Algorithm */

  LFR_ControlOutput = LFR_Proportional*PID_Kp + LFR_Integral*PID_Ki + LFR_Derivative*PID_Kd;    /**< Computes the Final Control Output of PID Control Algorithm */

  if(LFR_ControlOutput > LFR_MAX_MOTOR_SPEED)
  {
    LFR_ControlOutput = LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }
  if(LFR_ControlOutput < -LFR_MAX_MOTOR_SPEED)
  {
    LFR_ControlOutput = -LFR_MAX_MOTOR_SPEED;    /**< Keeps The Motor Speed in Limit */
  }

  if(LFR_ControlOutput < 0)
  {
    motor1.drive(LFR_MAX_MOTOR_SPEED + LFR_ControlOutput);
    motor2.drive(LFR_MAX_MOTOR_SPEED);    /**< Drives the Motor According to the Control Output */
  }
  else
  {
    motor1.drive(LFR_MAX_MOTOR_SPEED);
    motor2.drive(LFR_MAX_MOTOR_SPEED - LFR_ControlOutput);    /**< Drives the Motor According to the Control Output */
  }
}

void doDrive()
{
    test = false;

    uint16_t sensors[8];

  // Get the position of the line.  Note that we *must* provide the "sensors"
  // argument to readLine() here, even though we are not interested in the
  // individual sensor readings
  int position = qtr.readLineBlack(sensors);

  // Our "error" is how far we are away from the center of the line, which
  // corresponds to position 2500.
  int error = position - 2500;

  // Get motor speed difference using proportional and derivative PID terms
  // (the integral term is generally not very useful for line following).
  // Here we are using a proportional constant of 1/4 and a derivative
  // constant of 6, which should work decently for many Zumo motor choices.
  // You probably want to use trial and error to tune these constants for
  // your particular Zumo and line course.
  int speedDifference = error / 4 + 6 * (error - lastError);

  lastError = error;

  // Get individual motor speeds.  The sign of speedDifference
  // determines if the robot turns left or right.
  int m2Speed = MAX_SPEED + speedDifference;
  int m1Speed = MAX_SPEED - speedDifference;

  // Here we constrain our motor speeds to be between 0 and MAX_SPEED.
  // Generally speaking, one motor will always be turning at MAX_SPEED
  // and the other will be at MAX_SPEED-|speedDifference| if that is positive,
  // else it will be stationary.  For some applications, you might want to
  // allow the motor speed to go negative so that it can spin in reverse.
  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;
  if (m1Speed > MAX_SPEED)
    m1Speed = MAX_SPEED;
  if (m2Speed > MAX_SPEED)
    m2Speed = MAX_SPEED;

  motor2.drive(m1Speed);
  motor1.drive(m2Speed);
}


// Replaces placeholders in html with values
String processor(const String& var){
  //Serial.println(var);
  
  if(var == "PARAM_P"){
    return PARAM_P;
  }
  else if (var == "PARAM_I"){
    return PARAM_I;
  }
  else if (var == "PARAM_D"){
    return PARAM_D;
  }
  else if (var == "PID_Kp"){
    return (String)PID_Kp;
  }
  else if (var == "PID_Ki"){
    return (String)PID_Ki;
  }
  else if (var == "PID_Kd"){
    return (String)PID_Kd;
  }
  else if (var == "LFR_MAX_MOTOR_SPEED")
  {
    return (String)LFR_MAX_MOTOR_SPEED;
  }
  else if (var == "CALIBRATION_SPEED")
  {
    return (String)CALIBRATION_SPEED;
  }
  
  return String();
}
 