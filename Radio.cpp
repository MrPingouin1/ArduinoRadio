#include "Radio.h"
#include "Message.h"
#include <Arduino.h>

#include <RFM69.h>
#include <SPI.h>

// Output
#define SERIAL_BAUD 115200
#define RFM69_RST 4
#define LED 13

//UNKNOWN
#define RFM69_CS 8
#define RFM69_IRQ 7
#define IS_RFM69HCW true
#define RFM69_IRQN 4

// NETWORK
#define NETWORKID 17
#define MASTER_ID 1
#define FREQUENCY RF69_433MHZ // RF69_868MHZ RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey"

// THIS NODE
#define FIRST_SLAVE_ID 11
#define NUMBER_MAX_SLAVES 10
#define NODEID FIRST_SLAVE_ID + NUMBER_MAX_SLAVES

RFM69 Radio::radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);
Message message= Message();

void Radio::start(){
  //Start
  Serial.begin(SERIAL_BAUD);
  Serial.println("Starting Init...");
  
  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, HIGH);
  delay(100);
  digitalWrite(RFM69_RST, LOW);
  delay(100);
    
  radio.initialize(FREQUENCY,NODEID,NETWORKID); 
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
  Serial.print("Message ");
  message.printMessage();
  Serial.print(" envoyé à ");
  Serial.println(receiver);
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
    Serial.print("Message ");
    message->printMessage();   
    Serial.print(" reçu de ");
    Serial.print(sender);  
  } 
  return sender;
}

bool Radio::chercherMaitre(){
  int loopCounter = 0;
  int sender;

  message.joinRequest();
  
  if (sendMessage(message, MASTER_ID)){
    while (loopCounter < 2000){
      sender = receiveMessage(&message);
      if (sender == 1){
        if (message.isjoinResponseOK()){
          message.printMessage();
          radio.setAddress(message.joinResponseOK_getID());
          return true;
        }
        else if(message.isjoinResponseNotOK()){
          Serial.print("Refus du maître.");
          exit(0);
        }
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

void Radio::masterLoop(){
    static int nbEsclaves = 0;    
    int sender = receiveMessage(&message);
    
    if(sender>0)
    {
      // sender in list slave
      if(sender >= FIRST_SLAVE_ID && sender < FIRST_SLAVE_ID + nbEsclaves){
        if(message.isjoinRequest())
          message.joinResponseOK(sender);
        else{
          // dialogue
          
        }
      }
      // sender not in list slave and list slave not full
      else if(nbEsclaves < NUMBER_MAX_SLAVES){ 
        if(message.isjoinRequest()){
          message.joinResponseOK(FIRST_SLAVE_ID + nbEsclaves);
          nbEsclaves++;
        }
        else
          message.unknownDeviceResponse();
      }
      // sender not in list and list slave full
      else{
        if(message.isjoinRequest())
          message.joinResponseNotOK();
        else
          message.unknownDeviceResponse();
      }
      
      sendMessage(message, sender);      
    }
}

bool Radio::slaveLoop(){
    static int nbErreurs = 0;
    static unsigned int loopCounter = 0;
    
    if (loopCounter > 2000)
    {
      loopCounter= 0;
    
      // On construit une trame arbitraire pour test
      message.dataInformation(radio.readTemperature());
  
      if (!sendMessage(message, 1)){
        if (nbErreurs > 3){
          return false;
        }
        nbErreurs ++;
      }
      else
        nbErreurs = 0;
    }
    return true;
    loopCounter ++;
}

