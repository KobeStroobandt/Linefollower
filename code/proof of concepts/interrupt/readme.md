# Hardware Interrupt Start/Stop — Proof of Concept (Lijnvolger)

Deze proof of concept toont aan dat de lijnvolger kan worden gestart en gestopt via een **fysieke drukknop**, aangestuurd door een **hardware interrupt**.  
Dit werkt volledig onafhankelijk van de bestaande WiFi-webinterface.

---

## Doel

Aantonen dat:

- een fysieke drukknop een hardware interrupt kan triggeren  
- de interrupt kan worden gebruikt om een start/stop functie uit te voeren  
- de lijnvolger onmiddellijk reageert op de knop  
- start/stop werkt zonder polling en zonder vertraging  
- volledig integreert in het bestaande lijnvolgerprogramma

---

## Hardware

| Onderdeel | Beschrijving |
|----------|--------------|
| Microcontroller | ESP32 |
| Drukknop | NO (normally open) |
| Weerstand | 10 kΩ pull-down (optioneel, interne pull-up gebruikt) |
| Aansluiting | Knop → GPIO 4 → GND |

**Wiring**

```
BUTTON_PIN → GPIO 4  
Andere pin knop → GND  
```

---

## Software-uitbreiding

### 1. Pin + variabele

```cpp
#define BUTTON_PIN 4
volatile bool hwToggle = false;
```

### 2. Interrupt Service Routine (ISR)

```cpp
void IRAM_ATTR handleButtonInterrupt() {
  hwToggle = true;
}
```

### 3. Initialisatie in setup()

```cpp
pinMode(BUTTON_PIN, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonInterrupt, FALLING);
```

### 4. Verwerking in loop()

```cpp
if (hwToggle) {
  hwToggle = false;

  if (!start) {
    start = true;
    stop  = false;
    Serial.println("HW START");
  } else {
    stop  = true;
    start = false;
    Serial.println("HW STOP");
  }

  delay(200);
}
```

---

## Testprocedure

1. Plaats de robot op de vloer.  
2. Zet de ESP32 aan.  
3. Druk op de hardware start/stop knop.  
4. Observeer:
   - Eerste druk → lijnvolger start rijden  
   - Tweede druk → lijnvolger stopt  
   - LED of seriële output toont `HW START` / `HW STOP`  

---

## Resultaat

Deze POC bewijst dat:

- hardware interrupts betrouwbaar functioneren  
- een start/stop signaal correct verwerkt wordt  
- de lijnvolger direct reageert  
- de functionaliteit onafhankelijk is van WiFi-besturing  

