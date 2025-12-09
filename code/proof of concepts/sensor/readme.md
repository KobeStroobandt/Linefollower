# QTR-Sensoren - Proof of Concept  

Dit document beschrijft een *proof of concept* die aantoont dat:

- Bewijzen dat de ESP32 **meerdere sensoren simultaan en onafhankelijk** kan uitlezen  
- De volledige ADC-range gebruiken  
- De sensor-raw-waarden rechtstreeks tonen  

---

## Hardware Configuratie

Pinmapping:

```cpp
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
```

---
## Minimale Setup

```cpp
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t LFR_SensorValue[SensorCount];

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configureer sensoren zoals in de lijnvolger
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){
      Sens01, Sens02, Sens03, Sens04,
      Sens05, Sens06, Sens07, Sens08
  }, SensorCount);
  qtr.setEmitterPin(IR);
}
```
---
## Proof-of-Concept Loop

```cpp
void loop() {
  // Lees Ruwe sensorwaarden
  qtr.read(LFR_SensorValue);

  // Toon de waarden onafhankelijk van elkaar
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(LFR_SensorValue[i]);
    Serial.print('\t');
  }
  Serial.println();

  delay(50); // snelle uitlezing voor real-time feedback
}
```
---

## Stappenplan voor demonstratie

1. ESP32 voeden via USB  
2. Seriële monitor openen op **115200 baud**  
3. Sensorarray boven wit/zwart papier bewegen  
4. Observeer:

   - Elke sensor rapporteert **een onafhankelijke ADC-waarde**  
   - Meetbereik varieert ruw tussen **0 (veel reflectie)** en **4095 (weinig reflectie)**  
   - Geen filtering, geen calibratie, pure ruwe data  

---

## Wat deze POC bewijst

Deze POC bewijst dat:

- 6+ QTR-sensoren simultaan uitgelezen kunnen worden  
- de ESP32 de volledige ADC-range benut  
- jouw pin-mapping volledig functioneel is  
- de uitlezing onafhankelijk is van PID, motorsturing of calibratie  


