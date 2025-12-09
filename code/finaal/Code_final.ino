#include "SerialCommand.h"
#include "EEPROMAnything.h"
#include "BluetoothSerial.h"
#include <EEPROM.h>


// 6396 mm
// kp -18
// kd -0.2
// diff 0.4
// power 220

// forward en backward omdraaien voor positieve kp en kd te gebruiken. 
#define MotorLeftForward 18
#define MotorLeftBackward 19
#define MotorRightForward 16
#define MotorRightBackward 17

#define SerialPort SerialBT
BluetoothSerial SerialPort;
SerialCommand sCmd(SerialPort);

unsigned long previous, calculationTime;
const int sensor[] = {14,27,26,25,33,32,35,34}; // dit moet je nog aanpassen stan en dan de defines hierboven ook.
int debugPosition;
volatile bool run = false;
bool shutoff = true;
float iTerm;
float lastErr;

struct param_t
{
  unsigned long cycleTime;
  int black[8];
  int white[8];
  int power;  // 50
  float diff; //0.5
  float kp;
  float ki;
  float kd;
} params;

#define EEPROM_SIZE sizeof(param_t)

void setup()
{
  SerialPort.begin("ESP32_matt");// aanpassen per esp

  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("calibrate", onCalibrate);
  sCmd.addCommand("run", onRun);
  sCmd.addCommand("shutoff", onShutoff);
  sCmd.setDefaultHandler(onUnknownCommand);

  for(int i = 0; i <8;i++) 
  {
    pinMode(sensor[i], INPUT);
  }

  pinMode(23,INPUT);
  attachInterrupt(23,aan,RISING);
  pinMode(22,INPUT);
  attachInterrupt(22,uit,RISING);

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, params);
  EEPROM.end();
  SerialPort.println("ready");
}

void loop()
{
  sCmd.readSerial();
  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;
    
    long normalised[8];
    for (int i = 0; i < 8; i++)
    {
      long value = analogRead(sensor[i]);
      normalised[i] = map(value, params.black[i], params.white[i], 0, 4096);
    }

    int index = 0;
    for (int i = 1; i < 8; i++) if (normalised[i] < normalised[index]) index = i;

    if(shutoff == true) {if(normalised[index] > 3000) run = false;}

    float position;
    if (index == 0) {position = -30;} //missch nog veranderen
    else if (index == 7) {position = 30;} //missch nog veranderen
    else
    {
      int sNul = normalised[index];
      int sMinEen = normalised[index - 1];
      int sPlusEen = normalised[index + 1];

      float b = sPlusEen - sMinEen;
      b = b/2;

      float a = sPlusEen - b - sNul;

      position = -b /(2*a);

      position += index;
      position -= 3.5;

      position *= 9.525;
    }
    debugPosition = position;

    float error = -position; //error = setpoint - input
    float output = error * params.kp;

    iTerm += params.ki * error;
    iTerm = constrain(iTerm, -510, 510);
    output += iTerm;

    output += params.kd * (error - lastErr);
    lastErr = error;

    output = constrain(output, -510, 510);

    int powerLeft = 0;
    int powerRight = 0;

    if (run) if (output >= 0)
    {
      powerLeft = constrain(params.power + params.diff * output, -255, 255);
      powerRight = constrain(powerLeft - output, -255, 255);
      powerLeft = powerRight + output;
    }
    else
    {
      powerRight = constrain(params.power - params.diff * output, -255, 255);
      powerLeft = constrain(powerRight + output, -255, 255);
      powerRight = powerLeft - output;
    }

    analogWrite(MotorLeftForward, powerLeft > 0 ? powerLeft : 0);
    analogWrite(MotorLeftBackward, powerLeft < 0 ? -powerLeft : 0);
    analogWrite(MotorRightForward, powerRight > 0 ? powerRight : 0);
    analogWrite(MotorRightBackward, powerRight < 0 ? -powerRight : 0);

  }

  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}

void onUnknownCommand(char *command)
{
  SerialPort.print("unknown command: \"");
  SerialPort.print(command);
  SerialPort.println("\"");
}

void onSet()
{
  char* param = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(param, "cycle") == 0)
  { 
    long newCycleTime = atol(value);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);

    params.ki *= ratio;

    params.cycleTime = newCycleTime;
  }
  else if (strcmp(param, "power") == 0) params.power = atol(value);
  else if (strcmp(param, "diff") == 0) params.diff = atof(value);
  else if (strcmp(param, "kp") == 0) params.kp = atof(value);
  else if (strcmp(param, "ki") == 0) 
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.ki = atof(value) * cycleTimeInSec;
  }
  else if (strcmp(param, "kd") == 0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.kd = atof(value) / cycleTimeInSec;
  }

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(0, params);
  EEPROM.commit();
  EEPROM.end();
}

void onDebug()
{
  SerialPort.print("cycle time: ");
  SerialPort.println(params.cycleTime);

  SerialPort.print("black: ");
  for(int i = 0; i < 8; i++)
  {
    SerialPort.print(params.black[i]);
    SerialPort.print(" ");
  }
  SerialPort.println(" ");

  SerialPort.print("white: ");
  for(int i = 0; i < 8; i++)
  {
    SerialPort.print(params.white[i]);
    SerialPort.print(" ");
  }
  SerialPort.println();

  SerialPort.print("position: ");
  SerialPort.println(debugPosition);

  SerialPort.print("kp: ");
  SerialPort.println(params.kp);
  
  float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
  float ki = params.ki / cycleTimeInSec;
  SerialPort.print("Ki: ");
  SerialPort.println(ki);

  float kd = params.kd * cycleTimeInSec;
  SerialPort.print("Kd: ");
  SerialPort.println(kd);

  SerialPort.print("power: ");
  SerialPort.println(params.power);

  SerialPort.print("diff: ");
  SerialPort.println(params.diff);
  
  /* parameters weergeven met behulp van het debug commando doe je hier ... */
  
  SerialPort.print("calculation time: ");
  SerialPort.println(calculationTime);
  calculationTime = 0;
}

void onCalibrate()
{
  char* param = sCmd.next();

  if (strcmp(param, "black") == 0)
  {
    SerialPort.print("start calibrating black... ");
    for (int i = 0; i < 8; i++) params.black[i]=analogRead(sensor[i]);
    SerialPort.println("done");
  }
  else if (strcmp(param, "white") == 0)
  {
    SerialPort.print("start calibrating white... ");    
    for (int i = 0; i < 8; i++) params.white[i]=analogRead(sensor[i]);  
    SerialPort.println("done");      
  }

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(0, params);
  EEPROM.commit();
  EEPROM.end();
}

void onRun()
{
  char* param = sCmd.next();

  if (strcmp(param, "aan") == 0) 
  {
    run = true;
    iTerm = 0;
  }
  else if (strcmp(param, "uit") == 0) run = false; 
}

void onShutoff()
{
  char* param = sCmd.next();

  if (strcmp(param, "aan") == 0) 
  {
    shutoff = true;
    iTerm = 0;
  }
  else if (strcmp(param, "uit") == 0) shutoff = false; 
}

void aan()
{
  run = true;
}

void uit()
{
  run = false;
}

