#include "Radio.h"
#include "Message.h"
#include <Arduino.h>

#include <RFM69.h>
#include <SPI.h>

// Output
#define SERIAL_BAUD 115200
#define RFM69_RST 4
#define LED 13

#define RFM69_CS 8
#define RFM69_IRQ 7
#define IS_RFM69HCW true
#define RFM69_IRQN 4

#define NETWORKID 17
#define MASTER_ID 1
#define FREQUENCY RF69_433MHZ // RF69_868MHZ RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey"

RFM69 Radio::radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);

void Radio::start(int nodeid){
  //Start
  Serial.begin(SERIAL_BAUD);
  Serial.println("Starting Init...");
  
  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, HIGH);
  delay(100);
  digitalWrite(RFM69_RST, LOW);
  delay(100);
    
  radio.initialize(FREQUENCY,nodeid,NETWORKID); 
  if (IS_RFM69HCW)
    radio.setHighPower(); // Only for RFM69HCW & HW!    
  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
  radio.encrypt(ENCRYPTKEY);
  
  pinMode(LED, OUTPUT);
  Serial.print("Radio Init done. Transmitting at ");
  Serial.print(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(" MHz");
}

void Radio::blinkLED (byte PIN, byte DELAY_MS, byte loops)
{
  for (byte i=0; i<loops; i++)
  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}

int Radio::readTemperature(){
  return (int) radio.readTemperature(4);
}

bool Radio::sendMessage (Message message,int receiver){
  bool succes = radio.sendWithRetry(receiver, message.contenu, message.longueur);
  if (succes) 
  { 
    Serial.print("OK > ");
  }
  else
  {
    Serial.print("FAIL > ");        
  }
  blinkLED(LED, 40,1);
  Serial.print("Message envoyé à ");
  Serial.println(receiver);
  message.printMessage();
  return succes;
}


uint8_t Radio::receiveMessage (Message *message){
  
  uint8_t sender = 0;
  if (radio.receiveDone())
  {
    sender = radio.SENDERID;
    for (int i=0; i< radio.DATALEN; i++){
      message->contenu[i] = radio.DATA[i];
    }
    message->longueur = radio.DATALEN;
    
    if (radio.ACKRequested())
      radio.sendACK();
    
    blinkLED(LED, 40, 3);    
    Serial.print("Message reçu de ");
    Serial.print(sender);
    message->printMessage();  
  } 
  return sender;
}

bool Radio::chercherMaitre(){
  int loopCounter = 0;
  int sender;

  Message message= Message();
  message.contenu[0] = 118;
  message.contenu[1] = 218;
  message.longueur = 2;
  if (sendMessage(message, MASTER_ID)){
    while (loopCounter < 2000){
      sender = receiveMessage(&message);
      if (sender == 1 && message.longueur == 2 && message.contenu[0] == (char) 1){
        message.printMessage();
        radio.setAddress(message.contenu[1]);
        return true;
      }
      loopCounter ++;
      delay(1);
    }
    exit(0);
  }
  else{    
    radio.setAddress(MASTER_ID);
    return false;
  }
}

bool Radio::isJoinRequest(Message message){
  return (message.longueur == 2) && (message.contenu[0] == (char) 118) && (message.contenu[1] == (char) 218);  
}

