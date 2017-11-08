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
#define NODEID 1 
#define FREQUENCY RF69_433MHZ // RF69_868MHZ RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey"

RFM69 Radio::radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);

void Radio::start(){
  //Start
  Serial.begin(SERIAL_BAUD);
  Serial.println("Uart init done. Starting Init.");
  
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

void Radio::sendMessage (Message message,int receiver){
  Serial.println(radio.getFrequency());
  if (radio.sendWithRetry(receiver, message.contenu, message.longueur)) 
  { 
    Serial.print("OK > ");
  }
  else
  {
    Serial.print("FAIL > ");        
  }
  blinkLED(LED, 40,1);
}

void Radio::receiveMessage (){
  if (radio.receiveDone())
  {    
    Serial.print(F("Received Frame "));
    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("Sending ACK");
    }
    else 
    {
      Serial.println("NO ACK"); 
    }
    blinkLED(LED, 40,3);
  }  
}

