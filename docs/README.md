# MQTT
## Pricipe MQTT
Het MQTT-protocol (Message Queuing Telemetry Transport) wordt zeer veel gebruikt
in IoT toepassingen om sensordata te versturen en deze terug op te halen. MQTT
maakt gebruik van een Broker. De clients zijn in staat om zowel data te
publiceren als te abonneren (subscribe) bij een broker op data.

![MQTT](./assets/1d04d39cec5848e5caa2affdaa4ad38b.png)

In bovenstaande voorbeeld publiceert client B de dak temperatuur in het topic
temperature/roof en client A de vloer temperatuur in het topic
temperature/floor. Client A is geabonneerd (subscribe) bij de broker op het
topic temperature/roof afkomstig van client B.

## Waarom MQTT

De MQTT-broker is een zeer eenvoudig lichtgewicht server die perfect op een
raspberry pi kan draaien. Er zijn eveneens gratis publieke MQTT-brokers
beschikbaar zoals <https://www.hivemq.com/public-mqtt-broker/>

![HIVEMQ](./assets/380bb5c601fbfd6c6f319e8bff144f7e.png)

## Client

De Clients kunnen zowel een esp8266, esp32, Raspberry Pi of een ander toestel
dat verbonden is met het internet.

## Broker

Er zijn verschillende mogelijkheden om een MQTT-broker op te zetten:

-   Op een Raspberry pi. Een nadeel hieraan is dat de Raspberry pi niet publiek
    beschikbaar is.

-   Een gratis publieke MQTT-broker zoals
    <https://www.hivemq.com/public-mqtt-broker/>

-   Een betalende dienst eventueel op een virtuele machine.

-   …

## Gratis publieke MQTT-broker hivemq

## Berichten publiceren en abonneren op een topic

Installeer de PubSubClient bibliotheek in arduino IDE.

![Toevoegen bibliotheek](./assets/bc276f5ffc3e6e07107446f43486f934.png)

## ESP8266

Maak gebruik van het LOLIN (WEMOS) D1 R2 bord. Vertrek van het voorbeeld.

![Voorbeeld MQTT](./assets/VBMQTT.png)

Stel de SSID en het Wifi-paswoord in.

```cpp
const char* ssid = "........";
const char* password = "........";
```

Stel de broker in:

```cpp
const char* mqtt_server = "broker.hivemq.com";
```

Deze code is gebaseerd op <https://github.com/knolleary/pubsubclient>

Voorbeeldcode waarbij iedere 2 seconden “Hello world” + de waarde van een teller
gepubliceerd wordt in het topic “outTopic”.

Eveneens is er geabonneerd (subscribe) op het topic “inTopic”. Wanneer het eerste
karakter van de data op het topic inTopic een 1 is, licht de ingebouwde led op,
bij een andere karakter licht de ingebouwde led niet op.

```cpp
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "........";
const char* password = "........";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i \< length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
        digitalWrite(BUILTIN_LED, HIGH); 
        // Turn the LED on 
    } else {
        digitalWrite(BUILTIN_LED, LOW); // Turn the LED off
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
        Serial.println("connected");
        // Once connected, publish an announcement...
        client.publish("outTopic", "hello world");
        // ... and resubscribe
        client.subscribe("inTopic");
        } 
        else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
        }
    }
}

void setup() {
    pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    unsigned long now = millis();
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("outTopic", msg);
    }
}
```
Een verzonden topic kan bekeken worden op
<http://www.hivemq.com/demos/websocket-client/>

![HiveMQ Connection](./assets/77cd8aee6582070d654e83bf61302306.png)

Schrijf vervolgens in op het topic. Voorbeeld:

![HiveMQ Publish](./assets/0454e314d8d185becf7321d10ba47a74.png)

### Vragen

-   Om de hoeveel tijd wordt een bericht gepubliceerd in de voorbeeldcode? 
-   In welk topic wordt data gepubliceerd?
-   Welke data wordt er gepubliceerd?
-   Op welk topic is er geabonneerd?
-   Met welk teken moet een bericht beginnen zodat de ingebouwde led zou
    oplichten?

### Opdrachten

-   Pas de code aan zodat iedere 30 seconden een teller met 1 verhoogt en deze
    waarde gepubliceerd wordt in het topic esp/je eigen naam. Bewaar als
    mqttESP8266Oef1.
-   Pas de code aan zodat iedere 30 seconden de lichtsterkte doorgezonden wordt
    (analoge waarde). De lichtsterkte wordt gepubliceerd in het topic esp/je
    eigen naamLichtstrekte. Bewaar als mqttESP8266Oef2.
-   Wanneer een schakelaar ingedrukt is, stuur je de waarde 1 door naar het
    topic esp/je eigen naamLicht. Wanneer deze niet ingedrukt is, stuur je 0
    door. Laat vervolgens een 2de arduino hierop abonneren. Wanneer 1
    doorgezonden is, licht de ingebouwde led op.

## ESP32

Deze code is gebaseerd op <https://github.com/knolleary/pubsubclient>

Voorbeeldcode waarbij iedere 2 seconden “Hello world” + de waarde van een teller
gepubliceerd wordt in het topic “outTopic”.

Eveneens is er geabonneerd (subscribe) op het topic “inTopic”. Wanneer het eerste
karakter van de data op het topic inTopic een 1 is, licht de ingebouwde led op,
bij een andere karakter licht de ingebouwde led niet op.

```cpp
#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "........";
const char* password = "........";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i \< length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
        digitalWrite(BUILTIN_LED, HIGH); 
        // Turn the LED on 
    } else {
        digitalWrite(BUILTIN_LED, LOW); // Turn the LED off
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
        Serial.println("connected");
        // Once connected, publish an announcement...
        client.publish("outTopic", "hello world");
        // ... and resubscribe
        client.subscribe("inTopic");
        } 
        else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
        }
    }
}

void setup() {
    pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    unsigned long now = millis();
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("outTopic", msg);
    }
}
```

Een verzonden topic kan gekeken worden op
<http://www.hivemq.com/demos/websocket-client/>

![HiveMQ Connection](./assets/77cd8aee6582070d654e83bf61302306.png)

Schrijf vervolgens in op het topic. Voorbeeld:

![HiveMQ Publish](./assets/0454e314d8d185becf7321d10ba47a74.png)

### Vragen

-   Om de hoeveel tijd wordt een bericht gepubliceerd in de voorbeeldcode?
-   In welk topic wordt data gepubliceerd?
-   Welke data wordt er gepubliceerd?
-   Op welk topic is er geabonneerd?
-   Met welk teken moet een bericht beginnen zodat de ingebouwde led zou
    oplichten?

### Opdrachten

-   Pas de code aan zodat iedere 30 seconden een teller met 1 verhoogt en deze
    waarde gepubliceerd wordt in het topic esp/je eigen naam. Bewaar als
    mqttESP32Oef1.
-   Pas de code aan zodat iedere 30 seconden de lichtsterkte doorgezonden wordt
    (analoge waarde). De lichtsterkte wordt gepubliceerd in het topic esp/je
    eigen naamLichtstrekte. Bewaar als mqttESP32Oef2.
-   Wanneer een schakelaar ingedrukt is, stuur je de waarde 1 door naar het
    topic esp/je eigen naamLicht. Wanneer deze niet ingedrukt is, stuur je 0
    door. Laat vervolgens een 2de arduino hierop abonneren. Wanneer 1
    doorgezonden is, licht de ingebouwde led op.

## Python op Raspberry pi

Voorbeeldcode waarbij iedere 30 seconden de waarde van een teller gepubliceerd
wordt in het topic “outTopic/teller”.

Eveneens is er geabonneerd (subscribe) op alles van het topic inTopic.

Bron: <https://www.hivemq.com/blog/mqtt-client-library-paho-python/>

```py
import paho.mqtt.client as paho
import time
teller=1
def on_publish(client, userdata, mid):
    print("mid: "+str(mid))

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.qos)+" "+str(msg.payload))

client = paho.Client()
client.on_publish = on_publish
client.on_subscribe = on_subscribe
client.on_message = on_message
client.connect("broker.mqttdashboard.com", 1883)
client.subscribe("inTopic/#", qos=1)
client.loop_start()

while True:
    (rc, mid) = client.publish("outTopic/teller", str(teller), qos=1)
    teller = teller+1
    time.sleep(30)
```

### Vragen

-   Om de hoeveel tijd wordt een bericht gepubliceerd in de voorbeeldcode? 
-   In welk topic wordt data gepubliceerd?
-   Welke data wordt er gepubliceerd? 
-   Op welk topic is er geabonneerd?

### Opdrachten

-   Pas de code aan zodat iedere 30 seconden een teller met 1 verhoogt en deze
    waarde gepubliceerd wordt in het topic je eigen naam/teller. Bewaar als
    mqttRaspberryPiOef1.
-   Pas de code aan zodat iedere 30 seconden de temperatuur doorgezonden wordt
    via een DHT11. De temperatuur wordt gepubliceerd in het topic je eigen
    naam/Temperatuur. Bewaar als mqttRaspberryPiOef2.
-   Wanneer een schakelaar ingedrukt is, stuur je de waarde 1 door naar het
    topic je eigen naam/Licht. Wanneer deze niet ingedrukt is, stuur je 0 door.
    Abonneer vervolgens een 2de raspberry pi hierop. Wanneer 1 doorgezonden is,
    licht een led op.


## Extra info

- Meer uitleg over MQTT <https://learn.adafruit.com/mqtt-adafruit-io-and-you>
- MQTT en appinventor <https://community.appinventor.mit.edu/t/esp32-mqtt-broker-publish-subscribe-thingspeak/10490>

[![Netlify Status](https://api.netlify.com/api/v1/badges/e78160d3-c387-40a5-b609-d22af50c0b9a/deploy-status)](https://app.netlify.com/sites/innovet-mqtt/deploys)