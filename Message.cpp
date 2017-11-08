#include "Message.h"
#include <Arduino.h>

char Message::contenu[65];
unsigned char Message::longueur;

void Message::printMessage(){
	Serial.print("-> Message : ["); 
	for (int i=0; i< longueur -1; i++)
	{
		Serial.print((unsigned char) contenu[i], DEC); 
		Serial.print(",");
	}
  Serial.print((unsigned char) contenu[longueur -1], DEC);
	Serial.println("]");
}
