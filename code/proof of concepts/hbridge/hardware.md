# Hardware — TB6612FNG Proof of Concept

## Benodigde onderdelen
- TB6612FNG motorcontroller
- Arduino Uno/Nano
- 2× DC-motor
- 2× potmeter 10 kΩ (snelheidsregeling)
- 2× SPST schakelaar (richtingskeuze)
- Externe motorvoeding (VM)
- Arduino 5V voor logica (VCC)
- 100–470 µF condensator voor ontkoppeling
- Breadboard + jumper wires
- Gemeenschappelijke GND

---

## Pin-mapping (TB6612FNG → Arduino)

| TB6612FNG | Arduino |
|----------|---------|
| VCC | 5V |
| GND | GND |
| VM | motorvoeding |
| STBY | D2 |
| AIN1 | D4 |
| AIN2 | D5 |
| PWMA | D3 (PWM) |
| BIN1 | D12 |
| BIN2 | D13 |
| PWMB | D11 (PWM) |

### Potmeters
- Pot A → A0  
- Pot B → A1  

### Richtingsschakelaars
- Switch A → D6 (INPUT_PULLUP)  
- Switch B → D7 (INPUT_PULLUP)

---

## Ontkoppeling
- 100–470 µF tussen VM en GND  
- GND van Arduino moet verbonden zijn met GND van motorvoeding

---

## ASCII schema

