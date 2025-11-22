# Start/Stop Drukknop – Proof of Concept (Hardware Interrupt)

Dit document beschrijft een minimale *proof of concept* die aantoont dat een **drukknop** kan worden gebruikt om een proces te starten en stoppen via een **hardware interrupt** op een Arduino/ESP32.

Deze POC is volledig **hardware- en softwarematig minimaal**.

---

## 🧩 Hardware Configuratie

**Benodigdheden:**

- Arduino / ESP32
- 1x Drukknop
- 1x 10 kΩ Pull-down weerstand (optioneel als interne pull-down niet gebruikt)
- Jumperkabels
- LED + 220 Ω weerstand (optioneel voor visuele feedback)

**Aansluitschema:**

- **Drukknop:**  
  - Eén pin → GPIO 2 (interrupt pin)  
  - Andere pin → GND  
- **LED (optioneel):**  
  - Anode → GPIO 13  
  - Cathode → GND via 220 Ω weerstand  

---

## 🧩 Minimale Software

```cpp
// Pin definities
const int buttonPin = 2;    // Knop verbonden met interrupt-capable pin
const int ledPin = 13;      // Optionele LED voor visuele feedback

volatile bool running = false;  // Status van het proces

// Interrupt service routine (ISR)
void toggleRunning() {
  running = !running;           // Toggle status bij elke knopdruk
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // interne pull-up gebruiken
  pinMode(ledPin, OUTPUT);
  
  // Hardware interrupt configureren: verander op falling edge
  attachInterrupt(digitalPinToInterrupt(buttonPin), toggleRunning, FALLING);
  
  Serial.begin(115200);
  Serial.println("Start/Stop POC gestart. Druk op de knop om te togglen.");
}

void loop() {
  // Visuele feedback via LED
  digitalWrite(ledPin, running ? HIGH : LOW);

  // Seriële feedback
  static bool lastState = false;
  if (lastState != running) {
    Serial.println(running ? "START" : "STOP");
    lastState = running;
  }

  // Simulatie van een proces
  if (running) {
    // Plaats hier de code van het proces dat gestart/gestopt wordt
  }
  
  delay(50); // kleine debounce / loop vertraging
}
```

---

## 🔁 Proof-of-Concept Teststappen

1. Sluit de drukknop aan op de interrupt-pin (GPIO 2) en GND.  
2. Sluit de optionele LED aan op GPIO 13 en GND.  
3. Upload de bovenstaande code naar de Arduino/ESP32.  
4. Open de Serial Monitor op 115200 baud.  
5. Druk op de knop:
   - Bij de eerste druk: LED gaat aan, Serial Monitor toont `START`.  
   - Bij de tweede druk: LED gaat uit, Serial Monitor toont `STOP`.  
   - Elke volgende druk toggelt de status opnieuw.  
6. Observeer dat het proces (hier gesimuleerd) alleen actief is wanneer `running == true`.

---

## ✅ Wat deze POC bewijst

| Eigenschap | Bewijs |
|------------|--------|
| **Start/stop functionaliteit** | Status `running` toggelt bij elke knopdruk |
| **Hardware interrupt correct** | Interrupt routine `toggleRunning()` reageert direct zonder polling |
| **Visuele en seriële feedback** | LED en Serial Monitor tonen duidelijk de toestand |

---

## 📦 Optioneel

Deze POC kan worden uitgebreid door het toggle-signaal te gebruiken om bijvoorbeeld:

- Motoren aan/uit te zetten  
- Een robot te starten/stoppen  
- Andere processen in je project te triggeren  
