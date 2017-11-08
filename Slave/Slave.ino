#include "Radio.h"
#include "Message.h"
#include <RFM69.h>
#include <SPI.h>

Radio radio = Radio();
Message message = Message();

uint8_t node_id = 11; // node ID de base
uint8_t maitre_id; // le maitre du node

void setup() {
  while (!Serial);
  radio.start(node_id);
  //chercherMaitre();
  maitre_id = 1;
}

bool chercherMaitre() {
  /*maitre_id = 1;
  Message mess;
  static unsigned int loopCounter;
  bool succes;
  do {
    loopCounter = 0;
    delay(1000);
    mess.radiopacket[0] = 255; 
    mess.radioPacketLen = 1;

    mess.printMessage();
    sendMessage(mess, maitre_id);

    do{
      succes = receiveMessage() && radio.SENDERID == maitre_id;
      if (succes){
        // Le maitre contacter nous a répondu
        Serial.print("Reponse du maitre :");
        afficherRep();
      }
      Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
      loopCounter ++;
      delay(1);
    } while (loopCounter < 2000);
    

    if (!succes){
      maitre_id ++;
    }
  } while (maitre_id <10 && !succes);
  return succes;
  */
  return true;
}

void loop() {
  static unsigned int loopCounter = 0;
  static unsigned int packetnum = 0;
  static int sender;
  
  if (radio.receiveMessage(&sender, &message)){
    Serial.print("Message reçu : ");
    message.printMessage();
  }  
  
  //Tous les 2000 passages
  if (loopCounter > 2000)
  {
    packetnum ++; 
    loopCounter= 0;
  
    // On construit une trame arbitraire pour test
    message.contenu[0] = 01;
    message.contenu[1] = radio.readTemperature(); 
    message.contenu[2] = packetnum >> 8;  // Les poids forts
    message.contenu[3] = packetnum ;      // Les poids faibles
    message.contenu[4] = 00;
    message.contenu[5] = 255;
  
    message.contenu[6] = 11;
    message.contenu[7] = 22;
    message.contenu[8] = 33;
    message.contenu[9] = 0;
    message.contenu[10] = 0;
    message.contenu[11] = 0;
    message.longueur = 15;

    radio.sendMessage(message, maitre_id);
    message.printMessage();    
  }
  
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  loopCounter ++;
  delay(1);
}




