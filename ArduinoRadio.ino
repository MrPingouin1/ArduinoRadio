#include "Radio.h"
#include "Message.h"
#include <RFM69.h>
#include <SPI.h>


#define FIRST_SLAVE_ID 11
#define NUMBER_MAX_SLAVES 10
#define NODEID FIRST_SLAVE_ID + NUMBER_MAX_SLAVES

Radio radio = Radio();
Message message = Message();

bool isMaster;

void setup() {
  while (!Serial);
  radio.start(NODEID);
  isMaster = !radio.chercherMaitre();
  if(isMaster){
    Serial.println("I'm the boss!!! bitch!");
  }
  else
    Serial.println("I'm your bitch, mr boss");
}

void loop() {
  static unsigned int loopCounter = 0;
  static unsigned int packetnum = 0;
  static int sender;
  
  if(!isMaster){
    static int nbErreurs = 0;
    
    if (loopCounter > 2000)
    {
      packetnum ++; 
      loopCounter= 0;
    
      // On construit une trame arbitraire pour test
      message.contenu[0] = 25;
      message.contenu[1] = radio.readTemperature(); 
      message.longueur = 2;
  
      if (!radio.sendMessage(message, 1)){
        if (nbErreurs > 3){
          isMaster = !radio.chercherMaitre();
        }
        nbErreurs ++;
      }
      else
        nbErreurs = 0;
    }
  }
  else{
    static int nbEsclaves = 0;
    
    sender = radio.receiveMessage(&message);
    if(sender>0)
    {
      // sender in list slave
      if(sender >= FIRST_SLAVE_ID && sender < FIRST_SLAVE_ID + nbEsclaves){
        if(radio.isJoinRequest(message)){
          message.contenu[0] = 1;
          message.contenu[1] = sender; 
          message.longueur = 2;
          radio.sendMessage(message, sender);
        }
        else{
          // dialogue
          
        }
      }
      // sender not in list slave and list slave not full
      else if(nbEsclaves < NUMBER_MAX_SLAVES){ 
        if(radio.isJoinRequest(message)){
          // On ajoute l'esclave en lui envoyant son nouvel ID
          message.contenu[0] = 1;
          message.contenu[1] = FIRST_SLAVE_ID + nbEsclaves; 
          message.longueur = 2;
          if (radio.sendMessage(message, sender)){
            nbEsclaves++;
          }
        }
        else{
          // On lui demande d'envoyer une join request
          message.contenu[0] = 118;
          message.contenu[1] = 0; 
          message.longueur = 2;
          radio.sendMessage(message, sender);
        }
      }
      // sender not in list and list slave full
      else{
        if(radio.isJoinRequest(message)){
          // On lui dit qu'on est complet
          message.contenu[0] = 0;
          message.contenu[1] = 0; 
          message.longueur = 2;
          radio.sendMessage(message, sender);
        }
        else{
          // On lui demande d'envoyer une join request (même si on est complet)
          message.contenu[0] = 118;
          message.contenu[1] = 0; 
          message.longueur = 2;
          radio.sendMessage(message, sender);
        }        
      }      
    }
  }

  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  loopCounter ++;
  delay(1);
}




