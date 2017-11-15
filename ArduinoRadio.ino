#include "Radio.h"
#include "Message.h"
#include <RFM69.h>
#include <SPI.h>

Radio radio = Radio();

unsigned char etat;

void setup() {
  while (!Serial);
  radio.start();
  etat = radio.chercherMaitre();
  switch(etat){
    case 0 : Serial.println("Je suis le maître."); break;
    case 1 : Serial.println("Je suis un esclave."); break;
    default : Serial.println("Je suis solitaire."); break;
  }
}

void loop() {
  static unsigned int loopCounter = 0;

  switch(etat){
    case 0 : radio.masterLoop(); break;
    case 1 : 
      if (!radio.slaveLoop(loopCounter))
        etat = radio.chercherMaitre();
      break;
    default : 
    if (!radio.localLoop(loopCounter))
        etat = radio.chercherMaitre();
      break;
  }
  
  if (loopCounter > 2000)
    loopCounter = 0;
  loopCounter ++;
  Serial.flush();  
  delay(1);
}




