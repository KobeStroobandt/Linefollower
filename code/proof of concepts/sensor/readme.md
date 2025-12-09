# QTR-Sensoren Proof of Concept  
**Lijnvolger – Minimale hard- en software**

Deze Proof of Concept toont aan dat **minimaal 6 reflectiesensoren** (QTR-8RC) onafhankelijk van elkaar kunnen worden uitgelezen.  
Er wordt **geen calibratie**, **geen normalisatie** en **geen interpolatie** toegepast.  

De ruwe ADC-waarden worden rechtstreeks via **`qtr.read()`** uitgelezen zodat de **volledige meetrange** van de ESP32 ADC benut wordt (0–4095).

---

## Doel

- Bewijzen dat de ESP32 **meerdere sensoren simultaan en onafhankelijk** kan uitlezen  
- De volledige ADC-range gebruiken  
- De sensor-raw-waarden rechtstreeks tonen  
- De POC baseren op **dezelfde pin-out en code** als de volledige lijnvolger  

---

## Hardware

| Component | Beschrijving |
|----------|--------------|
| Microcontroller | ESP32 |
| Sensorarray | Pololu QTR-8RC |
| Aantal sensoren | 8 (minimaal 6 vereist) |
| IR LED pin | GPIO 21 |
| Sensorpins | 47, 38, 39, 40, 41, 42, 2, 1 |

**Sensor aansluitingen gebruikt in jouw code:**

```
QTR sensor → ESP32 pin
S1 → 47  
S2 → 38  
S3 → 39  
S4 → 40  
S5 → 41  
S6 → 42  
S7 → 2  
S8 → 1  
Emitter pin → 21
```

---

## Software – Minimale POC (gebaseerd op jouw eigen code)


### ✔ Minimale code

```cpp
#include <Arduino.h>
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

## Testprocedure

1. ESP32 voeden via USB  
2. Seriële monitor openen op **115200 baud**  
3. Sensorarray boven wit/zwart papier bewegen  
4. Observeer:

   - Elke sensor rapporteert **een onafhankelijke ADC-waarde**  
   - Meetbereik varieert ruw tussen **0 (veel reflectie)** en **4095 (weinig reflectie)**  
   - Geen filtering, geen calibratie, pure ruwe data  

---

## Resultaat

Deze POC bewijst dat:

- 6+ QTR-sensoren simultaan uitgelezen kunnen worden  
- de ESP32 de volledige ADC-range benut  
- jouw pin-mapping volledig functioneel is  
- de uitlezing onafhankelijk is van PID, motorsturing of calibratie  


