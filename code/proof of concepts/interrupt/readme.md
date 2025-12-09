# Hardware Interrupt Start/Stop — Proof of Concept

Dit document beschrijft een *proof of concept* die aantoont dat:

- een fysieke drukknop een hardware interrupt kan triggeren  
- de interrupt kan worden gebruikt om een start/stop functie uit te voeren  
- de lijnvolger onmiddellijk reageert op de knop
- start/stop werkt zonder polling en zonder vertraging  
  
---

## Hardware Configuratie

Pinmapping:

```cpp
#define BUTTON_PIN 4
```

Variabele:

```cpp
volatile bool hwToggle = false;
```

Interrupt Service Routine (ISR):

```cpp
void IRAM_ATTR handleButtonInterrupt() {
  hwToggle = true;
}
```

## Minimale Setup

```cpp
pinMode(BUTTON_PIN, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonInterrupt, FALLING);
```

## Proof-of-Concept Loop

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

## Stappenplan voor demonstratie

1. Plaats de robot op de vloer.
   
3. Zet de ESP32 aan.
   
5. Druk op de hardware start/stop knop.
   
7. Observeer:
   - Eerste druk → lijnvolger start rijden  
   - Tweede druk → lijnvolger stopt  
   - LED of seriële output toont `HW START` / `HW STOP`  

---

## Wat deze POC bewijst

Deze POC bewijst dat:

- hardware interrupts betrouwbaar functioneren  
- een start/stop signaal correct verwerkt wordt  
- de lijnvolger direct reageert  
- de functionaliteit onafhankelijk is van WiFi-besturing  

