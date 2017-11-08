#include <Message.h>
#include <Arduino.h>

#include <RFM69.h>
#include <SPI.h>

char Message::radiopacket[65];
unsigned char Message::radioPacketLen;
RFM69* Message::radio;

void Message::setRadio(RFM69* r){
	radio=r;
}
		
void Message::printMessage(){
	Serial.print("Message : ["); 
	for (int i=0; i< radioPacketLen; i++)
	{
		Serial.print((unsigned char) radiopacket[i], DEC); 
		Serial.print(",");
	}
	Serial.println("]");
}

void Message::sendMessage (int receiver){
	Serial.println(radio->getFrequency());
  if (radio->sendWithRetry(receiver, radiopacket, radioPacketLen)) 
  { 
    Serial.print("OK > ");
  }
  else
  {
    Serial.print("FAIL > ");        
  }
}

void Message::receiveMessage (){
  if (radio->receiveDone())
  {    
    Serial.print(F("Received Frame "));
    if (radio->ACKRequested())
    {
      radio->sendACK();
      Serial.println("Sending ACK");
    }
    else 
    {
      Serial.println("NO ACK"); 
    }
  }  
}