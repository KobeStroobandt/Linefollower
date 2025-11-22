# TB6612FNG Motorcontroller – Proof of Concept

Dit document beschrijft een minimalistische *proof of concept* die aantoont dat:

- Twee motoren **onafhankelijk** kunnen draaien  
- De snelheid **traploos regelbaar** is  
- De motoren eenvoudig van **draairichting** kunnen veranderen  
- De TB6612FNG motorcontroller **correct functioneert** als H-Bridge-vervanger  

Alle niet-relevante subsystemen (wifi, sensoren, PID, API, SPIFFS, enz.) zijn verwijderd.

---

## Hardware Configuratie

De gebruikte pinmapping komt direct uit het oorspronkelijke project:

```cpp
#define AIN1 11
#define BIN1 9
#define AIN2 12
#define BIN2 3
#define PWMA 13
#define PWMB 10
#define STBY 14
```

Motor offsets:

```cpp
const int offsetA = -1;
const int offsetB = 1;
```

Motor-objecten:

```cpp
#include <SparkFun_TB6612.h>

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);
```

Maximale snelheid:

```cpp
const int MAX_SPEED = 150;
```

---

## Minimale Setup

```cpp
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // TB6612FNG uit standby
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);
}
```

---

## Demonstratiefuncties

### Motor vooruit / achteruit / stop

```cpp
void testForward() {
  motor1.drive(100);
  motor2.drive(100);
}

void testBackward() {
  motor1.drive(-100);
  motor2.drive(-100);
}

void testStop() {
  motor1.drive(0);
  motor2.drive(0);
}
```

### Onafhankelijke motorcontrole

```cpp
void testIndependent() {
  motor1.drive(150);   // rechts harder vooruit
  motor2.drive(50);    // links zacht vooruit
}
```

### Traploze snelheidsregeling

```cpp
void testRamp() {
  for (int s = 0; s <= MAX_SPEED; s++) {
    motor1.drive(s);
    motor2.drive(MAX_SPEED - s);
    delay(20);
  }
}
```

### Richting omdraaien

```cpp
void testDirectionChange() {
  motor1.drive(120);
  motor2.drive(120);
  delay(1000);

  motor1.drive(-120);
  motor2.drive(-120);
  delay(1000);

  testStop();
}
```

---

## Proof-of-Concept Loop

```cpp
void loop() {

  Serial.println("Vooruit");
  testForward();
  delay(1000);

  Serial.println("Achteruit");
  testBackward();
  delay(1000);

  Serial.println("Stop");
  testStop();
  delay(500);

  Serial.println("Beide motoren onafhankelijk");
  testIndependent();
  delay(1500);

  Serial.println("Traploze snelheidsregeling");
  testRamp();

  Serial.println("Omgekeerde richting");
  testDirectionChange();
}
```

---

## Stappenplan voor demonstratie

1. Sluit TB6612FNG aan volgens de hardware mapping.

2. Upload de minimale POC-code.

3. Open de Serial Monitor op 115200 baud.

4. De code doorloopt automatisch de volgende tests:

- Motoren draaien vooruit
- Motoren draaien achteruit
- Motoren stoppen
- Motoren draaien onafhankelijk met verschillende snelheden
- Traploze snelheidsverandering via testRamp()
- Richting omkeren

5. Observeer dat beide motoren zichtbaar reageren op elke testfase.

6. De demonstratie is geslaagd wanneer beide motoren:

  - afzonderlijk reageren,
  - traploos van snelheid wisselen,
  - zowel vooruit als achteruit draaien.

## Wat deze POC bewijst

| Eigenschap | Bewijs |
|-----------|--------|
| **Onafhankelijke motorcontrole** | `motor1.drive()` en `motor2.drive()` worden apart aangestuurd |
| **Traploze snelheidsregeling** | Drive-waarden van -255 tot +255 |
| **Richting omkeerbaar** | Positieve waarde = vooruit, negatieve = achteruit |
| **TB6612FNG werkt correct** | Standby, PWM, IN1/IN2 en offsets worden gebruikt |

---
