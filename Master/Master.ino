#include "Radio.h"
#include "Message.h"
#include <RFM69.h>
#include <SPI.h>

Radio radio = Radio();
Message message = Message();

void setup() {
  while (!Serial);
  radio.start();  
}

void loop() {
  static unsigned int loopCounter = 0;
  static unsigned int packetnum = 0;
  static int receiver =  11;
   
  radio.receiveMessage();  
  
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

    radio.sendMessage(message, receiver);
    message.printMessage();    
  }
  
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  loopCounter ++;
  delay(1);
}

