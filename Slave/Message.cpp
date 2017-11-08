#include "Message.h"
#include <Arduino.h>

char Message::contenu[65];
unsigned char Message::longueur;

void Message::printMessage(){
	Serial.print("Message : ["); 
	for (int i=0; i< longueur; i++)
	{
		Serial.print((unsigned char) contenu[i], DEC); 
		Serial.print(",");
	}
	Serial.println("]");
}
