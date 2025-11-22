# Pinmapping
#define AIN1 11;

#define BIN1 9;

#define AIN2 12;

#define BIN2 3;

#define PWMA 13;

#define PWMB 10;

#define STBY 14;

# Motor offsets
const int offsetA = -1;

const int offsetB = 1;

# Motor objecten
#include <SparkFun_TB6612.h>

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);

Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

# Maximum snelheid
const int MAX_SPEED = 150;

# Minimale software voor de POC
void setup() {

  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(LED_BUILTIN, LOW);
  
  pinMode(STBY, OUTPUT);
  
  digitalWrite(STBY, HIGH);
  
}
