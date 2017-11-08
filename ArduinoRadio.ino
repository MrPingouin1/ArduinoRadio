#include "Radio.h"
#include "Message.h"
#include <RFM69.h>
#include <SPI.h>

#define NODEID 3525

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
    static int listeEsclaves[10];
    static int nbEsclaves = 0;
    
    sender = radio.receiveMessage(&message);
    if(sender>0)
    {
      if(sender not in && liste n'est full){        
        if(radio.isJoinRequest(message)){
          message.contenu[0] = 1;
          message.contenu[1] = nbEsclaves + 11; 
          message.longueur = 2;
          radio.sendMessage(message, sender);
          listeEsclaves[nbEsclaves]=nbEsclaves + 11;
          nbEsclaves++;
        }
      }
      else{
        // je suis pas ton pote
      }      
    }
  }

  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  loopCounter ++;
  delay(1);
}




