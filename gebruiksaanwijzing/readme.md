# Gebruiksaanwijzing

### opladen / vervangen batterijen
Voordat we de batterij loskoppelen van de robot zetten we eerst de schakelaar af.
Nadat we dit gedaan hebben kunnen we de batterij uit de robot schuiven en kunnen we de connector losmaken.
Nu we de batterij los hebben gekoppeld van de robot kunnen we hem opladen aan de hand van een oplaadstation.

### draadloze communicatie
#### verbinding maken
Het verbinden van een laptop/ smartphone met de robot gaat als volgt.
We verbinden ons toestel met de WiFi van de robot, deze heet "KOBE-LIJNVOLGER".
Wanneer ons toestel is verbonden met de robot kunnen we in onze browser surfen naar "192.168.4.1".
Nu komen we uit op een webpagina, waarop we de robot kunnen besturen en enkele instellingen kunnen aanpassen.

#### commando's
start
stop
speed
calibration speed
set kp [0..]  
set ki [0..]  
set kd [0..]  
calibrate

### kalibratie
Zet de robot in het midden van een zwarte lijn.
De robot zal nu 45 graden naar rechts draaien en daarna 90 graden in de andere richting en dan terug 45 graden naar rechts.
Tijdens deze bewegingen worden de waarden die de sensor meet verwerkt en wordt de sensor gekalibreerd.

### settings
De robot rijdt stabiel met volgende parameters:  
Kp: 0.10
Ki: 0.00
Kd: 0.00
speed: 180 of minder
calibration speed: 50

### start/stop button
De knop start en stopt de robot. 
De knop bevindt zich vanboven op de robot.
