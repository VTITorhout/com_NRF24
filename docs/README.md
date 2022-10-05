# NRF24

In heel veel toepassing komt het wel eens voor dat er nood is aan draadloze communicatie, maar in sommige gevallen kan het gebruik van WiFi of Bluetooth uit den boze zijn wegens te complex of een te groot vermogensverbruik. Denken we hierbij aan IoT toepassingen waar er datacaptatie nodig is op afstand, bij robotica waar kabelverbindingen niet mogelijk zijn of zelfs domotica, om eenvoudig een schakelaar bij te plaatsen zonder dat er hoeft gekapt en geboord te worden in muren. 

Zeker bij dit laatste voorbeeld is het gebruik van een draadloze technologie die weinig vermogen vereist bijzonder belangrijk, want het toestel moet gedurende enkele jaren meegaan op één batterij. 

Als het gaat over _low power_ draadloze communicatie is Nordic Semiconductors wellicht de grootste speler op de markt. Omwille van die reden vinden we dan ook in de elektronica heel dikwijls draadloze modules terug die gebruik maken van een IC van deze fabrikant. Hierbij is de NRF24 IC wellicht de meest gekende, die ook reeds zijn _counterfeits_ kent zoals de Si24. Deze laatste is echter volledig namaak, gebasseerd op een verouderd procestechnologie waardoor het verbruik hoger ligt en de gevoeligheid een stuk slechter is.

![NRF24 IC](./assets/nrf24_ic.png)

## De draadloze module

De NRF24 alleen als IC is niet bruikbaar. Deze heeft nog allerlei randcomponenten nodig voordat deze kan gebruikt worden. In de handel zijn dan ook veel (identieke, volgens het voorbeeld van Nordic) modules te vinden waarop deze IC is gemonteerd, samen met zijn randcomponenten:
1. Antenne (meestal opgenomen als PCB trace)
1. RF filter
1. Voedingsstabilisatie (condensatoren, waarbij meestal __te weinig__ is voorzien. Zie hiervoor verder bij [voedingsprobleem](#voedingsproblemen))
1. Oscillator
1. Breakout pinnen (meestal, soms via [castellations](https://www.eurocircuits.com/blog/castellations-on-a-pcb/))

![NRF24 randcomponenten](./assets/nrf24_components.png)

### Spectrum

De NRF24 is ontworpen om te werken in de 2,4GHz ISM band (vandaar ook zijn naam: **N**ordic **R**adio **F**requency **2**.**4**). Deze band is wellicht bij iedereen gekend als de band voor WiFi, wat klopt. De IC deelt het spectrum met deze van WiFi (en nog veel andere toepassingen, behorende tot de **I**ndustrial, **S**cientific en **M**edical werelden). 

![WiFi spectrum](./assets/wifi_channels.png)

Op bovenstaande afbeelding zijn de kanalen te vinden waarop je via WiFi kan communiceren (14 in totaal, waarbij er slechts 3 zijn die geen overlap kennen, namelijk 1, 6 en 11). Ieder kanaal neemt 22MHz in beslag. Deze grote bandbreedte is essentieel om snelheden te halen van 100'den Mbits (afhankelijk van de gebruikte modulatietechniek). Merk op dat de laagste (center) frequentie 2412MHz is, dus als ondergrens 2412-11=2401MHz. De hoogste (center) frequentie bedraagt 2484MHz, dus 2484+11=2496MHz. De ITU heeft  bepaald dat op wereldwijd niveau alle frequenties tussen 2,4GHz en 2,5GHz door iedereen zonder licentie mogen gebruikt worden. WiFi valt hier dus mooi binnen.

Voor de NRF24 is dit echter anders. Deze heeft een maximale datarate van enkele MBit's, waardoor de vereiste bandbreedte per kanaal stukken kleiner is. Voor het gemak van uitleg is de manier van moduleren achterwege gelaten (voor wie interesse hierin heeft, de NRF24 gebruikt [GFSK](https://en.wikipedia.org/wiki/Frequency-shift_keying#Gaussian_frequency-shift_keying)). Dit resulteert in veel meer kanalen die kunnen gebruikt worden.

Volgens de datasheet kan de IC gebruikt worden bij volgende frequenties:

> The nRF24L01+ is designed for operation in the world wide ISM frequency band at 2.400 - 2.4835GHz.

Wanneer we echter verder kijken in de datasheet zien we het volgende:

> * 126 RF channels
> * 1MHz non-overlapping channel spacing at 1Mbps
> * 2MHz non-overlapping channel spacing at 2Mbps

De NRF24 kan ingesteld worden om 126 kanalen te gebruiken, en bij 1Mbit hebben deze elk maximaal 1MHz bandbreedte. Het eerste kanaal start met een frequentie van 2400MHz, het volgende 2401MHz enzoverder. De aandachtige lezer zal wellicht opgemerkt hebben dat de kanalen vanaf 97 en verder niet gebruikt worden door WiFi, en dat deze dan ook bij uitstek geschikt zijn om ongestoord te communiceren. Wel is op te merken dat dit resulteert in een frequentie die buiten het door het ITU toegekende spectrum valt, en men mogelijk illegaal bezig is.

### Voeding en vermogen

De NRF24 werkt op 3,3V en **NIET** op 5V. Een spanningsbereik van 1,9 tot 3,6V is acceptabel. Bij lagere spanningen zal de module niet werken (wat op zich geen probleem is), bij hogere spanningen zal de module stuk gaan (wat wel een probleem is). De digitale pinnen (in- en uitgangen) zijn echter wel 5V compatibel, en kunnen dus rechtstreeks aangesloten worden op bijvoorbeeld een Arduino [UNO](#uno). Bij een [ESP8266](#esp8266) en een [ESP32](#esp32) is er totaal geen probleem.

Het zendvermogen van de module is instelbaar in een viertal stappen, waarbij ieder vermogen in een zeker verbruik resulteert. Met het extra vermogen zal er wel een grotere afstand kunnen overbrugd worden, maar zal ook een groter verbruik gepaard gaan. Houd rekening dat de voeding deze pieken (tijdens zenden) moet kunnen leveren. Zie hiervoor [voedingsprobleem](#voedingsproblemen).

| Versterking [dBm] | Verbruik [mA] | Code  |
| :---------------: | :-----------: | :----:|
| 0dBm | 11,3mA | `RF24_PA_MAX` |
| -6dBm | 9mA | `RF24_PA_HIGH` |
| -12dBm | 7,5mA | `RF24_PA_LOW` |
| -18dBm | 7mA | `RF24_PA_MIN` |

Naast het vermogen die essentieel is om te kunnen zenden zal de controller en de ontvangstlogica in de chip ook nog een deel vermogen opnemen, namelijk 13,5mA! Dit zal voortdurend gebeuren tijdens het luisteren (via `void startListening(void)`). Dit verbruik kan echter grotendeels worden uitgeschakeld via commando's, om het zo langer op een zelfde batterijlading te kunnen uithouden. Dit valt echter buiten het bestek van deze module, en voor hen die hierin interesse hebben wordt er verwezen naar volgende [youtube video](https://youtu.be/MvjpmsH2wKI) en de juiste commando's hiervoor, namelijk `void powerDown(void)` en `void powerUp(void)`.

### Aansluitingen

![NRF24 pinout](./assets/nrf24_pinout.png)

De NRF24 communceert met de _host_ controller via een SPI-bus. Deze heeft volgende aansluitingen van doen:
* MOSI (**M**aster **O**ut **S**lave **I**n): Data van _host_ controller naar NRF24
* MISO (**M**aster **I**n **S**lave **O**ut): Data van NRF24 naar _host_ controller
* SCK (**S**erial **C**loc**K**): Transmissie klok van _host_ controller naar NR24

Naast de essentiële aansluitingen zijn er ook nog drie andere aansluitingen, waarvan de eerste twee essentieel zijn voor de goede werking
* CSN (**C**hip **S**elect i**N**verted): Op de SPI bus kunnen meerdere slaves aanwezig zijn. Het laagmaken van deze pin activeert de NRF24 om deel te nemen aan buscommunicatie.
* CE (**C**hip **E**nable): Deze pin moet altijd hoog zijn opdat de NRF24 zou kunnen werken. Deze laagmaken plaatst de NRF24 in ultra low power mode (900nA).
* IRQ (**I**nterrupt **R**e**Q**uest): De NRF24 kan deze pin hoog/laag maken bij ontvangst van data om zo de _host_ controller te informeren van ontvangst. Zie hiervoor [volgend voorbeeld](#irqs)

## Het gebruikte protocol

Om (draadloze) communicatie te kunnen doen werken is er nood aan afspraken. Er zijn afspraken gemaakt over de signaalniveau's die uitgestuurd mogen worden, welke modulatie er toegepast wordt enzoverder. Dit zijn afspraken op niveau 0 (fysische laag) van het [OSI-model](https://nl.wikipedia.org/wiki/OSI-model). Natuurlijk moeten zowel zender als ontvanger dezelfde afspraken hanteren, of deze zullen elkaar niet verstaan. 

![OSI model](./assets/osi.png)

Maar communicatie gaat verder dan dit. Naast de afspraken op niveau 0 moeten ook concrete afspraken gemaakt worden op de laag er boven, namelijk laag 1 (datalink laag):
* Wanneer begint en wanneer eindigt een bericht?
* Hoe weet een toestel dat de data voor hem bestemd is?
* Hoe ben je zeker dat de ontvangen data niet corrupt is?

Bij de modulatietechniek wordt hier niet over gesproken, aangezien de modulatietechniek zich situeert op niveau 0 van het OSI model. Nordic Semiconductors heeft echter wel een eigen protocol toegevoegd bovenop de modulatietechniek, namelijk het _enhanced ShockBurst_ protocol, wat zich situeert op niveau 1. In dit protocol (zie hieronder) is er ruimte gelaten voor de data van de gebruiker, die op zijn beurt een hogere laag van het OSI model kan implementeren. Meestal zal dit meteen laag 7 zijn (applicatie laag).

### (Enhanced) ShockBurst

### Auto ACK

# Gebruik van de NRF24

## Voedingsproblemen

## Interfacing

### UNO

### ESP8266

### ESP32

## Voorbeeldcode

### Basis

#### UNO

#### ESP8266

#### ESP32

### Piggyback data

### IRQ's

#### UNO

#### ESP8266

#### ESP32

### Structures

#### Structure

#### Union
