#include "Radio.h"
#include "Message.h"
#include <RFM69.h>
#include <SPI.h>

Radio radio = Radio();

bool isMaster;

void setup() {
  while (!Serial);
  radio.start();
  isMaster = !radio.chercherMaitre();
  if(isMaster){
    Serial.println("Je suis le maître.");
  }
  else
    Serial.println("Je suis un esclave.");
}

void loop() {
  static int sender;
  
  if(isMaster){
    radio.masterLoop();
  }
  else{
    if (!radio.slaveLoop())
      isMaster = !radio.chercherMaitre();
  }

  Serial.flush();  
  delay(1);
}




