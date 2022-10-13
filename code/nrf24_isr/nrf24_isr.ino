#include <SPI.h>    //needed for SPI communication with NRF24
#include <RF24.h>  //needed for L1, L2 and L4 communications with NRF24

#define ROLE_TX  true      //can be true, any other value will result in RX (even if left away)

#define SPI_CSN 7   //pin to select NRF24
#define SPI_CE  8   //pin to enable NRF24
#define NRF_IRQ 2   //interrupt pin on UNO

const byte address[6] = {0x2B, 0x96, 0x09, 0xB6, 0x35}; //unique address for NRF24

RF24 radio(SPI_CE, SPI_CSN);  //create NRF24 object to communicate with visualisation

uint8_t rxBuff[32];

bool newDataFromISR = false;

void isrNRF(void){
  bool tx,fail,rx;
  radio.whatHappened(tx,fail,rx);
  if(rx){ //we have only interest in receiving data
    //Serial.println("ISR:  RX data!");
    radio.read(&rxBuff,radio.getDynamicPayloadSize());
    //we'll resend the received data to the transmitter with the next ACK package
    radio.writeAckPayload(1, &rxBuff[0], 1);  //pipe, data, length --> send received byte back to transmitter
    //inform the loop that there is new data inside the rxBuffer
    newDataFromISR = true;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("NRF24 on Arduino through interrupt");
   #if defined(ROLE_TX) && (ROLE_TX == true)
    Serial.println("\tRole = transmitter");
   #else
    Serial.println("\tRole = receiver");
   #endif
  if(radio.begin()){
    if(radio.isChipConnected()){
      if(radio.isPVariant()){
        Serial.print("NRF:\tPro ");
        radio.enableDynamicPayloads();    //short messages (not 32 bytes)
        radio.enableAckPayload();         //piggy back data with ACK
        radio.setDataRate(RF24_250KBPS);  //set the data rate
      }else{
        Serial.print("NRF:\tBasic ");
        radio.setDataRate(RF24_1MBPS);    //set the data rate
      }
      Serial.println("radio detected");
      radio.setPALevel(RF24_PA_MAX);      //set the power of the device
      radio.setChannel(100);              //set the channel to 2,5GHz
      radio.setRetries(7,3);              //set the retries (space them in time)
      radio.maskIRQ(1,1,0);               //interrupt on rx
    }else{
      Serial.println("ERR:\tNo radio detected!");
      while(1); //no need to continue
    }
  }else{
    Serial.println("ERR:\tFailed to create RF24 object");
    while(1); //no need to continue
  }
  #if defined(ROLE_TX) && (ROLE_TX == true)
    radio.stopListening();                  //Set module as transmitter
    radio.openWritingPipe(address);         //set the address
  #else
    radio.openReadingPipe(1, address);  //set the address
    radio.startListening(); //Set module as receiver
    pinMode(NRF_IRQ, INPUT_PULLUP); //good practice to pull interrupt pin high, interrupt will be mostly inverted logic
    attachInterrupt(digitalPinToInterrupt(NRF_IRQ), isrNRF, FALLING); //Interrupt will occur on falling edge
  #endif
}

uint8_t cnt = 0;

void loop() {
  #if defined(ROLE_TX) && (ROLE_TX == true)
    if(radio.write(&cnt, 1)){
      if(radio.isAckPayloadAvailable()){
          Serial.print("NRF:\tTX OK, piggyback data received (");
          Serial.print(radio.getDynamicPayloadSize());
          Serial.print(" bytes): ");
          radio.read(&rxBuff, radio.getDynamicPayloadSize());
          for(uint8_t i=0;i<radio.getDynamicPayloadSize();i++){
            Serial.print("0x");
            Serial.print(rxBuff[i],HEX);
            Serial.print(" ");
          }
          Serial.println();
      }else{
          Serial.println("NRF:\tTX OK, no piggyback data");
      }
      cnt++;
    }else{
        Serial.println("NRF:\tTX failed!");
    }
    delay(1000);
  #endif
  if(newDataFromISR){
    //show the content of the received data
    Serial.print("NRF:\tRX: ");
    Serial.println(rxBuff[0]);
    //we are done doing things with the data, release the buffer
    newDataFromISR = false;
  }
}
